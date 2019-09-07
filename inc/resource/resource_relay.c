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

#include <stdlib.h>
#include <unistd.h>
#include <peripheral_io.h>

#include "log.h"
#include "resource_internal.h"

void resource_close_relay(int pin_num)
{
	if (!resource_get_info(pin_num)->opened) return;

	_I("Relay is finishing...");
	peripheral_gpio_close(resource_get_info(pin_num)->sensor_h);
	resource_get_info(pin_num)->opened = 0;
}

int resource_write_relay(int pin_num, int write_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (!resource_get_info(pin_num)->opened) {
		ret = peripheral_gpio_open(pin_num, &resource_get_info(pin_num)->sensor_h);
		retv_if(!resource_get_info(pin_num)->sensor_h, -1);

		ret = peripheral_gpio_set_direction(resource_get_info(pin_num)->sensor_h, PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
		retv_if(ret != 0, -1);

		resource_get_info(pin_num)->opened = 1;
		resource_get_info(pin_num)->close = resource_close_relay;
	}

	ret = peripheral_gpio_write(resource_get_info(pin_num)->sensor_h, write_value);
	retv_if(ret < 0, -1);

	_I("Relay Value : %s", write_value ? "ON":"OFF");

	return 0;
}
