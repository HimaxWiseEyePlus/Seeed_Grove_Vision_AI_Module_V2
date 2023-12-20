

#ifndef HXEVENT_H_
#define HXEVENT_H_

#include "stdint.h"



#define HX_EVENT_DEFAULT_PRIORITY       (0xFF) //LOW LEVEL
#define HX_EVENTQUE_MAXSIZE             (64)
#define HX_EVENT_RETURN_NULL	        (0x00)
#define HX_EVENT_RETURN_DONE	        (0x01)
#define HX_EVENTQUE_DEFAULT_PROC        (0x00)

typedef uint8_t (*hx_event_cbfunc_t)();

typedef void    (*hx_idle_cbfunc_t)();

typedef uint8_t hx_event_t;

typedef struct _evt_t {
    uint8_t             event_prio;
    hx_event_cbfunc_t   event_cb;
}evt_t;

typedef void (*subs_loop_rtn_func_t)();

void hx_event_init();
void hx_event_create(hx_event_t *event);
void hx_event_set_priority(hx_event_t event, uint8_t prio);
void hx_event_set_callback(hx_event_t event, hx_event_cbfunc_t fct);
void hx_eventloop_start();
void hx_eventloop_stop();

void hx_event_activate_ISR(hx_event_t event);
void hx_event_set_idlecb(hx_idle_cbfunc_t fct);
void hx_event_set_cycle_cnt();

#endif /* HXEVENT_H_ */
