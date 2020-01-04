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

#include <peripheral_io.h>

#include "log.h"
#include "resource_internal.h"

#define UART_BUS 0 /* RPI3 - Tizen */

#define GPIO_UART 15

#define INPUT_SIZE 3
#define OUTPUT_SIZE 9

static peripheral_uart_h g_uart_h = NULL;

void resource_close_mcu90615(int pin_num)
{
	_I("MCU90615 is finishing...[%d]", pin_num);
	resource_get_info(GPIO_UART)->opened = 0;
	if (!g_uart_h) return;

	peripheral_uart_close(g_uart_h);
	g_uart_h = NULL;
}

int resource_read_mcu90615(double *target_object, double *target_ambient)
{
	int ret = PERIPHERAL_ERROR_NONE;
	uint8_t data[INPUT_SIZE] = {0xA5, 0x45, 0xEA};
	uint8_t rdata[OUTPUT_SIZE] = {0, };

	if (!g_uart_h) {
		ret = peripheral_uart_open(UART_BUS, &g_uart_h);
		retv_if(ret != PERIPHERAL_ERROR_NONE, -1);

		resource_get_info(GPIO_UART)->opened = 1;
		resource_get_info(GPIO_UART)->close = resource_close_mcu90615;

		ret = peripheral_uart_set_baud_rate(g_uart_h, PERIPHERAL_UART_BAUD_RATE_115200);
		goto_if(ret != PERIPHERAL_ERROR_NONE, error);

		ret = peripheral_uart_set_byte_size(g_uart_h, PERIPHERAL_UART_BYTE_SIZE_8BIT);
		goto_if(ret != PERIPHERAL_ERROR_NONE, error);

		ret = peripheral_uart_set_parity(g_uart_h, PERIPHERAL_UART_PARITY_NONE);
		goto_if(ret != PERIPHERAL_ERROR_NONE, error);

		ret = peripheral_uart_set_stop_bits(g_uart_h, PERIPHERAL_UART_STOP_BITS_1BIT);
		goto_if(ret != PERIPHERAL_ERROR_NONE, error);

		ret = peripheral_uart_set_flow_control(g_uart_h, PERIPHERAL_UART_SOFTWARE_FLOW_CONTROL_NONE, PERIPHERAL_UART_HARDWARE_FLOW_CONTROL_NONE);
		goto_if(ret != PERIPHERAL_ERROR_NONE, error);

		ret = peripheral_uart_write(g_uart_h, data, INPUT_SIZE);
		goto_if(ret < 0, error); /* the number of bytes write on success, otherwise a negative error value */
	}

	ret = peripheral_uart_read(g_uart_h, rdata, OUTPUT_SIZE);
	goto_if(ret < 0, error); /* the number of bytes write on success, otherwise a negative error value */

#ifdef DEBUG
	_D("Read : %d", ret);
#endif

#ifdef DEBUG
	for (int i = 0; i < OUTPUT_SIZE; i++) {
		_D("0x%X", rdata[i]);
	}
#endif

	*target_object = (double) (rdata[4] << 8 | rdata[5]) / 100.0f;
	*target_ambient = (double) (rdata[6] << 8 | rdata[7]) / 100.0f;

#ifdef DEBUG
	if (*target_object != 0.0f)
		_D("Target Object : %f, Target Ambient : %f", target_object, target_ambient);
#endif

	return 0;

error:
	resource_close_mcu90615(GPIO_UART);

	return -1;
}
