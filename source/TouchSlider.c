/*
 * TouchSlider.c
 *
 *  Created on: 4 mars 2018
 *      Author: julienbacon
 *
 *  FRDM-KL26Z-NixieSlider: Controlling IN-9 through touch slider and PWM.
 *  Copyright (C) 4 mars 2018  Julien Bacon "julien.bacon.1@gmail.com"
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

#include <TouchSlider.h>

#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "fsl_lptmr.h"
#include "fsl_tsi_v4.h"

#include "timers.h"

#include "deviceCtrl.h"
#include "channelCtrl.h"

/* LPTMR configuration definition */
#define LPTMR_SOURCE_CLOCK		CLOCK_GetFreq(kCLOCK_LpoClk)
#define LPTMR_USEC_COUNT		200000U

/* TSI utils definitions */
#define TOUCH_DELTA_VALUE		100U
#define TOUCH_CENTER_VALUE		2000U
#define TSI_IR_HANDLER			TSI0_IRQHandler

/* Touch slider scanning time constants */
#define SAMPLING_RATE			50U
#define DEBOUNCE_PERIOD			250U

/* Touch values */
typedef struct {
	uint16_t e1Value;
	uint16_t e2Value;
} touchValues_t;

/* Touch state */
//typedef enum {
//
//} touchState_t;

/* Device Queues and information */
static QueueHandle_t deviceQueues[MAX_PWM_CH];
static uint8_t deviceNb = 0;

/* Debouncing timer and state */
static TimerHandle_t debounceTimer = NULL;
bool isDebouncing = false;

/* Sampling timer */
static TimerHandle_t samplingTimer = NULL;

/* Touch slider values */
static touchValues_t noTouchVal = {0U, 0U};
static touchValues_t currentTouchVal = {0U, 0U};
static touchValues_t previousTouchVal = {0U, 0U};

/* Initialization functions */
void initLPTMR(void);
void initTSI(void);
void calibrateTSI(void);

/* Scanning functions */
void startScanningE1(void);
void startScanningE2(void);
bool processScan(uint16_t scanResult, uint16_t touchThreshold,
				 uint16_t *scanningValue);
BaseType_t calculateMvtType(touchValues_t touchVal);

/* Debouncing callback */
void debouncingCallback(TimerHandle_t xTimer);

/* Touch slider sampling callback */
void samplingCallback(TimerHandle_t xTimer);

/* Touch slider interrupt handler */
void TSI_IR_HANDLER(void) {
	BaseType_t needToYield = pdFALSE;

	/* Clear flags */
	TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
	TSI_ClearStatusFlags(TSI0, kTSI_OutOfRangeFlag);

	/* Get/Calculate the scan processing values */
	uint16_t scanResult = TSI_GetCounter(TSI0);
	uint16_t touchThresholdE1 = noTouchVal.e1Value + TOUCH_DELTA_VALUE;
	uint16_t touchThresholdE2 = noTouchVal.e2Value + TOUCH_DELTA_VALUE;

	/* Get the scanned channel */
	uint8_t scannedChannel = TSI_GetMeasuredChannelNumber(TSI0);

	switch (scannedChannel) {
	case BOARD_TSI_ELECTRODE_1:
		/* Process the scan and start scanning E2 if touched */
		if(processScan(scanResult, touchThresholdE1,
				&(currentTouchVal.e1Value))) {
			/* Start debouncing and scanning electrode 2 */
			xTimerStart(debounceTimer, 0);
			isDebouncing = true;
			startScanningE2();
		}
		break;
	case BOARD_TSI_ELECTRODE_2:
		/* Process the scan */
		processScan(scanResult, touchThresholdE2, &(currentTouchVal.e2Value));
		/* Check for the event type */
		needToYield = calculateMvtType(currentTouchVal);
		break;
	}
	portYIELD_FROM_ISR(needToYield);
}

/* Initialize touch slider */
void initTouchSlider(QueueHandle_t iDeviceQueues[], uint8_t iDeviceNb) {
	printf("Initializing touch slider.\n");
	/* Save device queues and information */
	deviceNb = iDeviceNb;
	for(uint8_t i = 0; i < deviceNb; ++i) {
		deviceQueues[i] = iDeviceQueues[i];
	}

	/* Initialize hardware */
	initLPTMR();
	initTSI();

	/* Calibrate TSI */
	calibrateTSI();

	/* Creating the debouncing timer */
	TickType_t debouncingPeriod = pdMS_TO_TICKS(DEBOUNCE_PERIOD);
	debounceTimer = xTimerCreate("debouncingTimer", debouncingPeriod, pdFALSE,
			0, debouncingCallback);

	/* Create and start the sampling timer */
	TickType_t samplingRate = pdMS_TO_TICKS(SAMPLING_RATE);
	samplingTimer = xTimerCreate("samplingTimer", samplingRate, pdTRUE,
			0, samplingCallback);
	xTimerStart(samplingTimer, 0);

	printf("Touch slider initialized.\n");
}

/* Initialization functions */
void initLPTMR(void) {
	/* Configuration structure */
	lptmr_config_t lptmrConfig;
	memset((void *)&lptmrConfig, 0, sizeof(lptmrConfig));

	/* Get default configuration */
	LPTMR_GetDefaultConfig(&lptmrConfig);

	/* Initialize hardware */
	LPTMR_Init(LPTMR0, &lptmrConfig);
	LPTMR_SetTimerPeriod(LPTMR0,
			USEC_TO_COUNT(LPTMR_USEC_COUNT, LPTMR_SOURCE_CLOCK));
}

void initTSI(void) {
	/* Configuration structure */
	tsi_config_t tsiConfig;

	/* Get default configuration */
	TSI_GetNormalModeDefaultConfig(&tsiConfig);

	/* Initialize hardware */
	TSI_Init(TSI0, &tsiConfig);
	NVIC_EnableIRQ(TSI0_IRQn);
	TSI_EnableModule(TSI0, true);

	/* Enable software trigger */
	TSI_EnableHardwareTriggerScan(TSI0, false);

	/* Enable interrupt */
	TSI_EnableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
	TSI_EnableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
}

void calibrateTSI(void) {
	tsi_calibration_data_t	calibrationVal;
	memset((void *)&calibrationVal, 0, sizeof(calibrationVal));
	TSI_Calibrate(TSI0, &calibrationVal);

	noTouchVal.e1Value =
			calibrationVal.calibratedData[BOARD_TSI_ELECTRODE_1];
	noTouchVal.e2Value =
			calibrationVal.calibratedData[BOARD_TSI_ELECTRODE_2];
}

/* Scanning functions */
void startScanningE1(void) {
	/* Start scanning */
	TSI_SetMeasuredChannelNumber(TSI0, BOARD_TSI_ELECTRODE_1);
	TSI_StartSoftwareTrigger(TSI0);
}

void startScanningE2(void) {
	/* Start scanning */
	TSI_SetMeasuredChannelNumber(TSI0, BOARD_TSI_ELECTRODE_2);
	TSI_StartSoftwareTrigger(TSI0);
}

bool processScan(uint16_t scanResult, uint16_t touchThreshold,
				 uint16_t *scanningValue) {
	bool sliderIsTouched = false;

	if (scanResult > touchThreshold) {
		*scanningValue = scanResult;
		sliderIsTouched = true;
	} else {
		*scanningValue = TOUCH_CENTER_VALUE;
	}

	return sliderIsTouched;
}

BaseType_t calculateMvtType(touchValues_t touchVal) {
	BaseType_t needToYield = pdFALSE;
	devCtrlMsg_t deviceCtrlMsg;
	/* Check for UP and Down touch scheme */
	if (touchVal.e1Value < touchVal.e2Value) {
		deviceCtrlMsg.commande = DEVICE_UP;
	} else if (touchVal.e1Value > touchVal.e2Value) {
		deviceCtrlMsg.commande = DEVICE_DOWN;
	}

	/* Send control message to all device */
	for(uint8_t i = 0; i < deviceNb; ++i) {
		xQueueSendToBackFromISR(deviceQueues[i], &deviceCtrlMsg,
				&needToYield);
	}

	return needToYield;
}

/* Debouncing callback */
void debouncingCallback(TimerHandle_t xTimer) {
	isDebouncing = false;
}

/* Touch slider sampling callback */
void samplingCallback(TimerHandle_t xTimer) {
	if(!isDebouncing) {
		startScanningE1();
	}
}
