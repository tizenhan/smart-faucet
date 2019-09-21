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

#ifndef __RESOURCE_ULTRASONIC_JSN_SR04T_INTERNAL_H__
#define __RESOURCE_ULTRASONIC_JSN_SR04T_INTERNAL_H__

/**
 * @brief Releases the gpio handle and changes the gpio pin state to the close(0).
 * @param[in] trig_pin_num The number of the gpio pin connected to the trig of the ultrasonic sensor
 */
extern void resource_close_ultrasonic_jsn_sr04t_trig(int trig_pin_num);

/**
 * @brief Releases the gpio handle and changes the gpio pin state to the close(0).
 * @param[in] echo_pin_num The number of the gpio pin connected to the echo of the ultrasonic sensor
 */
extern void resource_close_ultrasonic_jsn_sr04t_echo(int echo_pin_num);

#endif /* __RESOURCE_ULTRASONIC_JSN_SR04T_INTERNAL_H__ */
