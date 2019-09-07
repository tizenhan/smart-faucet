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

#ifndef __RESOURCE_ULTRASONIC_JSN_SR04T_H__
#define __RESOURCE_ULTRASONIC_JSN_SR04T_H__

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Reads the value of gpio connected ultrasonic sensor(JSN_SR04T).
 * @param[in] trig_pin_num The number of the gpio pin connected to the trig of the ultrasonic sensor
 * @param[in] echo_pin_num The number of the gpio pin connected to the echo of the ultrasonic sensor
 * @param[in] cb A callback function to be invoked when the gpio interrupt is triggered
 * @param[in] data The data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @see If the gpio pin is not open, creates gpio handle before reading the value of gpio.
 */
extern int resource_read_ultrasonic_jsn_sr04t(int trig_pin_num, int echo_pin_num, resource_read_cb cb, void *data);

#if defined (__cplusplus)
}
#endif

#endif /* __RESOURCE_ULTRASONIC_JSN_SR04T_H__ */
