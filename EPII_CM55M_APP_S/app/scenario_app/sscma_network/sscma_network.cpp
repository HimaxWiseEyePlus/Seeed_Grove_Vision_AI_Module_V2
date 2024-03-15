#include "sscma_network.h"

#include <cm_backtrace.h>
#include <edgelab.h>

using namespace edgelab;

#define DEMO_WIFI_SSID   "Eureka"
#define DEMO_WIFI_PASS   "31415926"

#define DEMO_MQTT_SERVER "homeassistant.local"
#define DEMO_MQTT_USER   "seeed"
#define DEMO_MQTT_PASS   "xiao"

/* MQTT message received callback */
void demo_mqtt_cb(char* top, int tlen, char* msg, int mlen) {
    el_printf(">> MQTT Subscription message received <<\r\n");
    el_printf(" topic:   ");
    for(int i=0; i< tlen; i++) {
        el_printf("%c", top[i]);
    }
    el_printf("\n");
    el_printf(" message: ");
    for(int i=0; i< mlen; i++) {
        el_printf("%c", msg[i]);
    }
    el_printf("\n");
}

static void app(void *arg)
{
    Device *device = Device::get_device();
    device->init();
    Network *net = device->get_network();

    el_printf(" Network Demo\n");
    uint32_t cnt_for_retry = 0;
    net->init();
    while (net->status() != NETWORK_IDLE) {
        el_sleep(100);
		if(cnt_for_retry++ > 50) {
            net->init();
            cnt_for_retry = 0;
        }
    }
    el_printf(" Network initialized!\n");

    cnt_for_retry = 0;
    net->join(DEMO_WIFI_SSID, DEMO_WIFI_PASS);
	while (net->status() != NETWORK_JOINED)
	{
		el_sleep(100);
        if(cnt_for_retry++ > 100) {
            net->join(DEMO_WIFI_SSID, DEMO_WIFI_PASS);
            cnt_for_retry = 0;
        }
	}
    el_printf(" WIFI joined!\n");

	cnt_for_retry = 0;
    net->connect(DEMO_MQTT_SERVER, DEMO_MQTT_USER, DEMO_MQTT_PASS, demo_mqtt_cb);
	while (net->status() != NETWORK_CONNECTED)
	{
		el_sleep(100);
        if(cnt_for_retry++ > 100) {
            net->connect(DEMO_MQTT_SERVER, DEMO_MQTT_USER, DEMO_MQTT_PASS, demo_mqtt_cb);
            cnt_for_retry = 0;
        }
	}
    el_printf(" MQTT connected!\n");

	char buf[128] = {0};
    int cnt = 0;
	net->subscribe("XIAO/SUB", MQTT_QOS_1);
	while (true)
	{
		el_sleep(2000);
        sprintf(buf, "hello mqtt %d", cnt++);
        net->publish("XIAO/PUB", buf, strlen(buf), MQTT_QOS_0);
	}
}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
extern "C" int app_main(void)
{
    cm_backtrace_init("CmBacktrace", "V1.0.0", "V1.0.0");

    if (xTaskCreate(app, "app", 1024*4 , NULL, 1, NULL) != pdPASS)
    {
        printf("app creation failed!.\r\n");
        while (1)
            ;
    }

    vTaskStartScheduler();

    for (;;)
        ;
}