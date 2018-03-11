/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    FRDM-KL26Z-NixieSlider.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL26Z4.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "task.h"

#include "channelCtrl.h"
#include "deviceCtrl.h"
#include "LedDevMngr.h"
#include "ModeTrigger.h"
#include "PwmMngr.h"
#include "touchSlider.h"

#define ALL_LED_CONFIG_CREATED			7U
#define DEVICE_SELECT_PIN				5U

/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /* Check the selected device */
    uint8_t selectedDevice = (uint8_t)GPIO_ReadPinInput(MODE_GPIO,
    		DEVICE_SELECT_PIN);

    /* Configurations */
    ledDevConfig_t *ledConfigs[MAX_LED_CH] = {NULL, NULL, NULL};
    uint8_t ledDevConfigsCreated = 0U;

    /* Create control queues and events group */
    QueueHandle_t pwmCtrlQueue = xQueueCreate(10, sizeof(chCtrlMsg_t));
    QueueHandle_t devCtrlQueues[MAX_PWM_CH] = {NULL};
    uint8_t deviceNb;
    EventGroupHandle_t events = xEventGroupCreate();

    /* Initialize device configurations */
    if(selectedDevice) {
    	deviceNb = MAX_LED_CH;
    	pwmChIdx_t ledChIdx[MAX_LED_CH] = {RED_CH, GRN_CH, BLU_CH};
    	for(uint8_t i = 0; i < deviceNb; ++i) {
    		devCtrlQueues[i] = xQueueCreate(1, sizeof(devCtrlMsg_t));

    		if(devCtrlQueues[i]) {
    			ledConfigs[i] = pvPortMalloc(sizeof(ledDevConfig_t));
    			if(ledConfigs[i]) {
    				ledConfigs[i]->ledChannel = ledChIdx[i];
    				ledConfigs[i]->events = events;
    				ledConfigs[i]->inputCtrlQueue = devCtrlQueues[i];
    				ledConfigs[i]->outputCtrlQueue = pwmCtrlQueue;
    				ledDevConfigsCreated |= (1U << i);
    			}
    		}
    	}
    }

    /* Check the result of queues and events group creation */
    if(pwmCtrlQueue && events &&
    		(ledDevConfigsCreated == ALL_LED_CONFIG_CREATED)) {
    	/* Initialize the mode trigger */
    	initModeTrigger(events);

    	/* Initialize the touch slider */
    	initTouchSlider(devCtrlQueues, deviceNb);

    	/* Create tasks */
    	if(selectedDevice) {
    		char *ledMngrTaskNames[MAX_LED_CH] = {"RedDevMngrTask",
    											  "GrnDevMngrTask",
												  "BluDevMngrTask"};
    		for(uint8_t i = 0; i < MAX_LED_CH; ++i) {
    			xTaskCreate(ledDevMngrTask, ledMngrTaskNames[i], 175,
    					(void *)ledConfigs[i], 2, NULL);
    		}
    	}

    	xTaskCreate(pwmMngrTask, "PwmMngrTask", 150, (void *)pwmCtrlQueue, 1,
    			NULL);


    	/* Start scheduler */
    	vTaskStartScheduler();
    } else {
    	printf("Error creating control queues and events group.\n");
    }

    return 0 ;
}
