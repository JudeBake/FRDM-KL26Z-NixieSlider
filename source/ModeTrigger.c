/*
 * ModeTrigger.c
 *
 *  Created on: 3 mars 2018
 *      Author: julienbacon
 *
 *  <one line to give the program's name and a brief idea of what it does.>
 *  Copyright (C) 3 mars 2018  Julien Bacon "julien.bacon.1@gmail.com"
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <ModeTrigger.h>
#include <stdio.h>

#include "fsl_gpio.h"
#include "peripherals.h"

#include "events.h"

#define MODE_CHANGE_IRQ			MODE_IRQHANDLER
#define MODE_CHANGE_IRQ_FLAG	(1U << 3U)

static EventGroupHandle_t events = NULL;

void MODE_CHANGE_IRQ(void) {
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;

	printf("Mode change IRQ.\n");

	/* Clear GPIO interrupt flag */
	GPIO_ClearPinsInterruptFlags(MODE_GPIO, MODE_CHANGE_IRQ_FLAG);

	/* Send mode change event if event group have been instantiated */
	if (events) {
		xEventGroupSetBitsFromISR(events, MODE_EVENT,
				&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void initModeTrigger(EventGroupHandle_t iEvents) {
	events = iEvents;
}
