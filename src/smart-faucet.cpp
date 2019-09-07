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
#include "nubisonif.hpp"

// 누비슨 클라우드에서 발급하는 인식키
// 연결장치의 제품 식별키를 입력하기
char mytoken[32] = "56S7-87E7-G69X-6585";

// Nubison IoT 클라우드 서비스 주소(테스트서버 주소)
char cloudifaddress[64] = "nubisoniot.com";

// 클라우드 접속 포트 번
int  cloudifport = 1883;

// Nubison IoT 연계 모듈 클래스 인스턴스
static NubisonIF *cloudif = NULL;

#define GPIO_JSN_SR04T_TRIG (20)
#define GPIO_JSN_SR04T_ECHO (21)

#define GPIO_RELAY_0 (19) /* Cold Water */
#define GPIO_RELAY_1 (26) /* Hot Water */

#define HIGH_TEMPERATURE (30)
#define LOW_TEMPERATURE (20)

/* Timer */
#define DISTANCE_DURATION (1.0f)
#define TEMPERATURE_DURATION (0.05f)
#define NUBISON_DURATION (11.0f)
static Ecore_Timer *distance_timer = NULL;
static Ecore_Timer *temperature_timer = NULL;
static Ecore_Timer *nubison_timer = NULL;

/* Value */
static int enable_cold = 1;
static int enable_hot = 1;
static int status_cold = 1;
static int status_hot = 1;
static double target_object = 0.0f;
static double distance = 0.0f;
static int power = 1;

/* Size */
#define BUFSIZE 32

void _read_distance_cb(double value, void *data)
{
	/* Distance : 20 ~ 600 cm */
	distance = value;
	//_D("Distance : %f", distance);

	int ret = 0;
	static int in_count = 0;
	static int out_count = 0;
	if (power == 0 || distance < 30.0f) {
		if (in_count >= 5) {
			ret = resource_write_relay(GPIO_RELAY_0, 0);
			ret_if(ret < 0);
			status_cold = 0;

			ret = resource_write_relay(GPIO_RELAY_1, 0);
			ret_if(ret < 0);
			status_hot = 0;
		}
		in_count++;
		out_count = 0;
	} else {
		if (out_count >= 5) {
			ret = resource_write_relay(GPIO_RELAY_0, enable_cold);
			ret_if(ret < 0);
			status_cold = enable_cold;

			ret = resource_write_relay(GPIO_RELAY_1, enable_hot);
			ret_if(ret < 0);
			status_hot = enable_hot;
		}
		out_count++;
		in_count = 0;
	}
}

Eina_Bool _distance_timer_cb(void *data)
{
	int ret = 0;

	ret = resource_read_ultrasonic_jsn_sr04t(GPIO_JSN_SR04T_TRIG, GPIO_JSN_SR04T_ECHO, _read_distance_cb, NULL);
	retv_if(ret < 0, ECORE_CALLBACK_CANCEL);

	return ECORE_CALLBACK_RENEW;
}

Eina_Bool _temperature_timer_cb(void *data)
{
	int ret = 0;
	double target_ambient = 0.0f;

	ret = resource_read_mcu90615(&target_object, &target_ambient);
	retv_if(ret < 0, ECORE_CALLBACK_CANCEL);

	if (target_object == 0.0f) return ECORE_CALLBACK_RENEW;
	//_D("Temperature : %f and %f", target_object, target_ambient);

	if (target_object > HIGH_TEMPERATURE) {
		enable_cold = 1;
		enable_hot = 0;
	} else if (target_object < LOW_TEMPERATURE) {
		enable_cold = 0;
		enable_hot = 1;
	} else {
		enable_cold = 1;
		enable_hot = 1;
	}

	return ECORE_CALLBACK_RENEW;
}

// Nubison Cloud과 통신하는 콜백 함수
//1.Device 의 상태를 조회하는 콜백
void NubisonCB_Query(char* rdata, char* api, char* uniqkey)
{
	_D("QueryCB : %s %s %s", rdata, api, uniqkey);

	// sendData를 String 자료형 변수로 저장하여 보내주세요.
	char tmp[BUFSIZE] = {0, };
	sprintf(tmp, "%d:%d:%d:%d:%d", status_cold, status_hot, (int) target_object, (int) distance, power);

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 관련해서 정확히 DB에 Unit 별로 들어 게 하는 것은 클라우드 서버에서 Driver로 셋팅함
	cloudif->SendtoCloud(tmp, TYPE_STRING, api, uniqkey);
}

//2.Device 의 제어 하는 콜백
void NubisonCB_Invoke(char* rdata, char* api, char* uniqkey)
{
	_D("InvokeCB : %s %s %s", rdata, api, uniqkey);

	char tmp[BUFSIZE] = {0, };
	char *word = NULL;

	int number = 0;
	int value = 0;

	strncpy(tmp, rdata, BUFSIZE);

	word = strtok(tmp, ":");
	number = atoi(word);

	word = strtok(NULL, ":");
	value = atoi(word);

	_D("Led Number : %d, Value : %d", number, value);

	switch (number) {
	case 4:
		power = value;
		break;
	default:
		break;
	}

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
    // 제어가 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud(tmp, TYPE_STRING, api, uniqkey);
}

//3.Device 의 하드웨어 설정 하는 콜백
void NubisonCB_Setting(char* rdata, char* api, char* uniqkey)
{
	_D("SettingCB : %s %s %s", rdata, api, uniqkey);

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 설정 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"ok", TYPE_STRING, api, uniqkey);
}

//4.Device 의 하드웨어 상태를 체크 하는 콜백
void NubisonCB_Check(char* rdata, char* api, char* uniqkey)
{
	_D("CheckCB : %s %s %s", rdata, api, uniqkey);

	// 클라우드에서 조회 요청이 왔을때 관련된내용을 담아서 전달 함
	// 상태체크 성공적으로 되었는지확인해서 값을 전달함//
	cloudif->SendtoCloud((char*)"Success.", TYPE_STRING, (char*)"check", uniqkey);
}

//5.Device 의 클라우드와의 인증 관련 콜백
void NubisonCB_AUTHO(int authocode)
{
	_D("AUTHOCB : %d", authocode);

	if (authocode == NUBISONIOT_AUTHO_OK) {
		//1.인증에 정상적으로
		_D("NUBISONIOT_AUTHO_OK");
	} else if (authocode == NUBISONIOT_AUTHO_ERROR_TESTTIME_EXPIRE) {
		//2.테스트 계정기간이 만료
		_D("NUBISONIOT_AUTHO_ERROR_TESTTIME_EXPIRE");
	} else if (authocode == NUBISONIOT_AUTHO_ERROR_UNAUTHORIZED_USE) {
		//3.인증되지 않은 계정
		_D("NUBISONIOT_AUTHO_ERROR_UNAUTHORIZED_USE");
	}
}

Eina_Bool _nubison_timer_cb(void *data)
{
	NubisonIF *nubif = (NubisonIF *) data;
	char tmp[BUFSIZE] = {0, };

	if(nubif != NULL) {
		nubif->Loop();
	}

	sprintf(tmp, "%d:%d:%d:%d:%d", status_cold, status_hot, (int) target_object, (int) distance, power);
	nubif->NotitoCloud(tmp, TYPE_STRING, mytoken, 1);
	_D("NotitoCloud[%s]", tmp);

    return ECORE_CALLBACK_RENEW;
}

bool service_app_create(void *data)
{
    distance_timer = ecore_timer_add(DISTANCE_DURATION, _distance_timer_cb, NULL);
    retv_if(!distance_timer, false);

    temperature_timer = ecore_timer_add(TEMPERATURE_DURATION, _temperature_timer_cb, NULL);
    retv_if(!temperature_timer, false);

	//1. Nubison IoT 연계 모듈 생성
	cloudif = new NubisonIF();

	//2.콜백 함수를 설정
	cloudif->SetCBFuntion(NubisonCB_Query, NubisonCB_Invoke, NubisonCB_Setting, NubisonCB_Check, NubisonCB_AUTHO);

	//3. 클라우드 접속 정보를 설정해서 초기화
	int ret = cloudif->Init(cloudifaddress, cloudifport, mytoken);
	if(ret != 0) {
		_E("Failed to connect with Nubison IoT");
	}

	nubison_timer = ecore_timer_add(NUBISON_DURATION, _nubison_timer_cb, cloudif);
	retv_if(!nubison_timer, false);

    return true;
}

void service_app_terminate(void *data)
{
    ecore_timer_del(distance_timer);
    ecore_timer_del(temperature_timer);
    ecore_timer_del(nubison_timer);

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
