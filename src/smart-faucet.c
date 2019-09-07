#include <log.h>
#include <tizen.h>
#include <service_app.h>
#include <Ecore.h>

#include "resource.h"

#define GPIO_JSN_SR04T_TRIG (20)
#define GPIO_JSN_SR04T_ECHO (21)

/* Timer */
static Ecore_Timer *jsn_sr04t_timer = NULL;

/* Value */
static double distance = 0.0f;

void _read_jsn_sr04t_cb(double value, void *data)
{
	distance = value;
	_D("Distance : %f", distance);
}

Eina_Bool _jsn_sr04t_timer_cb(void *data)
{
	int ret = 0;

	ret = resource_read_ultrasonic_jsn_sr04t(GPIO_JSN_SR04T_TRIG, GPIO_JSN_SR04T_ECHO, _read_jsn_sr04t_cb, NULL);
	retv_if(ret < 0, ECORE_CALLBACK_CANCEL);

	return ECORE_CALLBACK_RENEW;
}

bool service_app_create(void *data)
{
    jsn_sr04t_timer = ecore_timer_add(1.0f, _jsn_sr04t_timer_cb, NULL);
    retv_if(!jsn_sr04t_timer, false);

    return true;
}

void service_app_terminate(void *data)
{
    ecore_timer_del(jsn_sr04t_timer);

    return;
}

void service_app_control(app_control_h app_control, void *data)
{
    // Todo: add your code here.
    return;
}

static void
service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	return;
}

static void
service_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
service_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char* argv[])
{
    char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);

	return service_app_main(argc, argv, &event_callback, ad);
}
