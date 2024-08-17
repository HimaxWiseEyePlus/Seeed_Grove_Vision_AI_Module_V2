## Event Handler - CGP 13/8/24

I am trying to get my head around the event handler in scenario_app/event_handler. It seems to 
permit the ability to receive callbacks when certain interrupt events occur.

Getting datapath events
------------------------
Take the example of allon_sensor_tflm - which does receive events when the image is ready for processing.

Its code includes this (edited highlight!)
```
static void dp_app_cv_eventhdl_cb(EVT_INDEX_E event) 
	dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);

	switch(event) {
	case EVT_INDEX_XDMA_FRAME_READY:
		g_cur_jpegenc_frame++;
		g_frame_ready = 1;
		dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
		break;
	}
```
And we see that its console prints lines like this:
```
EVT event = 10
SENSORDPLIB_STATUS_XDMA_FRAME_READY 4
```

How does this happen? The allon_sensor_tflm makefile includes this line:
```
APPL_DEFINES += -DEVT_DATAPATH
```

which is equivalent to:
```
#define EVT_DATAPATH
```

The app includes this initialisation code:
```
        event_handler_init();
        cisdp_sensor_start();
    	event_handler_start();
```

and if you look in event_handler/event_handler.c for event_handler_init() you see something like this:

```
#ifdef EVT_DATAPATH

	<snip>
    // Create and register callback for EVT_INDEX_XDMA_FRAME_READY
    hx_event_create(&g_event[EVT_INDEX_XDMA_FRAME_READY]);
    if(g_event[EVT_INDEX_XDMA_FRAME_READY] == EVT_INDEX_MAX){
        dbg_printf(DBG_LESS_INFO,"Fail to allocate event for EVT_INDEX_XDMA_FRAME_READY\n");
    }
    else{
        hx_event_set_callback(g_event[EVT_INDEX_XDMA_FRAME_READY], evt_xDMA_FrameReady_cb);
        hx_event_set_priority(g_event[EVT_INDEX_XDMA_FRAME_READY], EVT_PRIORITY_HIGHEST);
    }
    <snip>
#endif
```

So the allon_sensor_tflm app is registering callbacks for the EVT_INDEX_XDMA_FRAME_READY event, 
which duely arrive! 

Note that EVT_INDEX_XDMA_FRAME_READY is defined in event_handler_evt.h and is the 11th
entry (when EVT_DATAPATH is defined) which explains the value 10 being printed for the event.



    
    