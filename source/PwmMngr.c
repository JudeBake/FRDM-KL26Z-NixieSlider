/*
 * PwmMngr.c
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

#include <PwmMngr.h>

#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "fsl_tpm.h"
#include "peripherals.h"

#include "channelCtrl.h"

/* Update channel function */
void updateChannel(tpm_chnl_t channel, dutyCycle_t dutyCycle);

/* Task main implementation */
void pwmMngrTask(void *p) {
	printf("Initializing PWM Manager Task.\n");
	/* Control queue */
	QueueHandle_t ctrlQueue = (QueueHandle_t)p;

	/* Channel control message */
	chCtrlMsg_t ctrlMessage;

	/* TPM channel identifiers */
	tpm_chnl_t tpmIds[MAX_PWM_CH] = {kTPM_Chnl_0, kTPM_Chnl_2,
			   	   	   	   	   	     kTPM_Chnl_4, kTPM_Chnl_5};

	/* Initialize the PWM channels */
	for(uint8_t i = 0; i < MAX_PWM_CH; ++i) {
		updateChannel(tpmIds[i], DUTY_MAX / 2);
	}

	printf("Starting PWM Manager Task.\n");

	for(;;) {
		/* Wait for control message to come in */
		xQueueReceive(ctrlQueue, &ctrlMessage, portMAX_DELAY);

		/* Check PWM limits */
		if(ctrlMessage.dutyCycle < DUTY_MIN) {
			ctrlMessage.dutyCycle = DUTY_MIN;
		} else if(ctrlMessage.dutyCycle > DUTY_MAX) {
			ctrlMessage.dutyCycle = DUTY_MAX;
		}

		/* Update the channel */
		updateChannel(tpmIds[ctrlMessage.chIndex], ctrlMessage.dutyCycle);
	}
}

/* Update channel function */
void updateChannel(tpm_chnl_t channel, dutyCycle_t dutyCycle) {
	TPM_UpdatePwmDutycycle(PWM_PERIPHERAL, channel, kTPM_CenterAlignedPwm,
			dutyCycle);
}
