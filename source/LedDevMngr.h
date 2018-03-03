/*
 * LedDevMngr.h
 *
 *  Created on: 3 mars 2018
 *      Author: julienbacon
 *
 *  FRDM-KL26Z-NixieSlider: Controlling IN-9 through touch slider and PWM.
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

#ifndef LEDDEVMNGR_H_
#define LEDDEVMNGR_H_

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

#include "channelCtrl.h"

typedef struct {
	pwmChIdx_t ledChannel;
	EventGroupHandle_t events;
	QueueHandle_t inputCtrlQueue;
	QueueHandle_t outputCtrlQueue;
} ledDevConfig_t;

void ledDevMngrTask(void *p);

#endif /* LEDDEVMNGR_H_ */
