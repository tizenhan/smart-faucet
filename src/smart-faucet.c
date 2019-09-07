/*
 * Copyright (c) 2019 SINO TECH Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <log.h>
#include <tizen.h>
#include <service_app.h>
#include <Ecore.h>

#include "resource.h"

#define GPIO_JSN_SR04T_TRIG (20)
#define GPIO_JSN_SR04T_ECHO (21)

#define GPIO_RELAY_0 (19)
#define GPIO_RELAY_1 (26)

/* Timer */
static Ecore_Timer *jsn_sr04t_timer = NULL;

/* Value */
static double distance = 0.0f;

void _read_jsn_sr04t_cb(double value, void *data)
{
	/* Distance : 20 ~ 600 cm */
	distance = value;
	_D("Distance : %f", distance);

	int ret = 0;
	static int in_count = 0;
	static int out_count = 0;
	if (distance < 30.0f) {
		if (in_count >= 5) {
			ret = resource_write_relay(GPIO_RELAY_0, 0);
			ret_if(ret < 0);

			ret = resource_write_relay(GPIO_RELAY_1, 0);
			ret_if(ret < 0);
		}
		in_count++;
		out_count = 0;
	} else {
		if (out_count >= 5) {
			ret = resource_write_relay(GPIO_RELAY_0, 1);
			ret_if(ret < 0);

			ret = resource_write_relay(GPIO_RELAY_1, 1);
			ret_if(ret < 0);
		}
		out_count++;
		in_count = 0;
	}
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
