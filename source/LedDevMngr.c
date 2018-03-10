/*
 * LedDevMngr.c
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

#include <LedDevMngr.h>

#include <stdio.h>

#include "task.h"

#include "deviceCtrl.h"
#include "events.h"

/* LED device step */
#define LED_BRT_STEP			5

/* Task Period */
#define TASK_PERIOD				250U

/* LED modes */
typedef enum {
	RGB,
	RED,
	GRN,
	BLU
} ledMode_t;

/* LED Status */
typedef enum {
	INACTIVE,
	ACTIVE,
} ledActivity_t;

typedef struct {
	ledMode_t ledMode;
	ledActivity_t ledIsActive;
	dutyCycle_t activeDutyCycle;
	dutyCycle_t previousDutyCycle;
	ledDevConfig_t *config;
} ledDevice_t;

/* LED device name strings */
static char *ledDevNames[MAX_LED_CH] = {"RED", "GREEN", "BLUE"};

/* Mode changing function */
void changeMode(ledDevice_t *device);

/* Device PWM processing function */
void processDevPwm(devCtrlMsg_t inputCmd, ledDevice_t *device);

/* Device updating functions */
void turnOffDevice(ledDevice_t *device);
void restoreDevice(ledDevice_t *device);
void updateDevice(ledDevice_t *device);

/* Task main implementation */
void ledDevMngrTask(void *p) {
	/* LED device descriptor */
	ledDevice_t device = {RGB, ACTIVE, (DUTY_MAX / 2), (DUTY_MAX / 2),
								(ledDevConfig_t *)p};

	printf("Initializing %s LED device.\n",
			ledDevNames[device.config->ledChannel - MAX_NIXIE_CH]);

	/* Input message */
	devCtrlMsg_t devCommand;

	/* Convert time period */
	TickType_t taskPeriod = pdMS_TO_TICKS(TASK_PERIOD);

	/* Event */
	EventBits_t event;
	EventBits_t event2WaitFor = MODE_EVENT;

	printf("Starting %s LED manager task.\n",
			ledDevNames[device.config->ledChannel - MAX_NIXIE_CH]);

	for(;;) {
		event = xEventGroupWaitBits(device.config->events, event2WaitFor,
				pdTRUE, pdFALSE, taskPeriod);

		if((event & MODE_EVENT)) {
			switch(device.ledMode) {
			case RGB:
				device.ledMode = RED;
				break;
			case RED:
				device.ledMode = GRN;
				break;
			case GRN:
				device.ledMode = BLU;
				break;
			case BLU:
				device.ledMode = RGB;
				break;
			}
			changeMode(&device);
		}

		if(uxQueueMessagesWaiting(device.config->inputCtrlQueue)) {
			xQueueReceive(device.config->inputCtrlQueue, &devCommand, 0);

			/* Process input command and send output command */
			processDevPwm(devCommand, &device);
			updateDevice(&device);
		}
	}
}

/* Change mode function */
void changeMode(ledDevice_t *device) {
	switch(device->ledMode) {
	case RGB:
		device->ledMode = RGB;
		if(device->config->ledChannel != BLU_CH) {
			restoreDevice(device);
		}
		if(device->activeDutyCycle == DUTY_MIN) {
			device->ledIsActive = INACTIVE;
		} else {
			device->ledIsActive = ACTIVE;
		}
		break;
	case RED:
		device->ledMode = RED;
		if((device->config->ledChannel == GRN_CH) ||
				(device->config->ledChannel == BLU_CH)) {
			turnOffDevice(device);
		}
		break;
	case GRN:
		device->ledMode = GRN;
		if((device->config->ledChannel == GRN_CH)) {
			restoreDevice(device);
		} else if((device->config->ledChannel == RED_CH)) {
			turnOffDevice(device);
		}
		break;
	case BLU:
		device->ledMode = BLU;
		if((device->config->ledChannel == BLU_CH)) {
			restoreDevice(device);
		} else if((device->config->ledChannel == GRN_CH)) {
			turnOffDevice(device);
		}
		break;
	}
}

/* Device PWM processing function */
void processDevPwm(devCtrlMsg_t inputCmd, ledDevice_t *device) {
	/* Calculate the new PWM */
	dutyCycle_t newDutyCycle;
	switch(inputCmd.commande) {
	case DEVICE_DOWN:
		newDutyCycle = device->activeDutyCycle - LED_BRT_STEP;
		break;
	case DEVICE_UP:
		newDutyCycle = device->activeDutyCycle + LED_BRT_STEP;
		break;
	}

	/* Limit the duty cycle */
	if(newDutyCycle < DUTY_MIN) {
		newDutyCycle = DUTY_MIN;
	} else if(newDutyCycle > DUTY_MAX) {
		newDutyCycle = DUTY_MAX;
	}

	/* Check device mode */
	switch(device->ledMode) {
	case RGB:
		if(device->ledIsActive) {
			device->activeDutyCycle = newDutyCycle;
		}
		break;
	case RED:
		if(device->config->ledChannel == RED_CH) {
			device->activeDutyCycle = newDutyCycle;
		}
		break;
	case GRN:
		if(device->config->ledChannel == GRN_CH) {
			device->activeDutyCycle = newDutyCycle;
		}
		break;
	case BLU:
		if(device->config->ledChannel == BLU_CH) {
			device->activeDutyCycle = newDutyCycle;
		}
		break;
	}
}

/* Device updating functions */
void turnOffDevice(ledDevice_t *device) {
	chCtrlMsg_t chCtrlMsg;
	device->previousDutyCycle = device->activeDutyCycle;
	device->activeDutyCycle = DUTY_MIN;
	chCtrlMsg.chIndex = device->config->ledChannel;
	chCtrlMsg.dutyCycle = device->activeDutyCycle;
	printf("Updating LED %s to %d duty cycle.\n",
			ledDevNames[device->config->ledChannel - MAX_NIXIE_CH],
			device->activeDutyCycle);
	xQueueSendToBack(device->config->outputCtrlQueue, &chCtrlMsg,
			portMAX_DELAY);
}

void restoreDevice(ledDevice_t *device) {
	chCtrlMsg_t chCtrlMsg;
	device->activeDutyCycle = device->previousDutyCycle;
	chCtrlMsg.chIndex = device->config->ledChannel;
	chCtrlMsg.dutyCycle = device->activeDutyCycle;
	printf("Updating LED %s to %d duty cycle.\n",
			ledDevNames[device->config->ledChannel - MAX_NIXIE_CH],
			device->activeDutyCycle);
	xQueueSendToBack(device->config->outputCtrlQueue, &chCtrlMsg,
			portMAX_DELAY);
}

void updateDevice(ledDevice_t *device) {
	chCtrlMsg_t chCtrlMsg;
	device->previousDutyCycle = device->activeDutyCycle;
	chCtrlMsg.chIndex = device->config->ledChannel;
	chCtrlMsg.dutyCycle = device->activeDutyCycle;
	printf("Updating LED %s to %d duty cycle.\n",
			ledDevNames[device->config->ledChannel - MAX_NIXIE_CH],
			device->activeDutyCycle);
	xQueueSendToBack(device->config->outputCtrlQueue, &chCtrlMsg,
			portMAX_DELAY);
}
