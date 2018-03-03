/*
 * channelCtrl.h
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

#ifndef CHANNELCTRL_H_
#define CHANNELCTRL_H_

#define MAX_PWM_CH			4
#define MAX_NIXIE_CH		1
#define MAX_LED_CH			(MAX_PWM_CH - MAX_NIXIE_CH)

typedef enum {
	NIXIE_CH,
	RED_CH,
	GRN_CH,
	BLU_CH
} pwmChIdx_t;

enum dutyCycleLimits {
	DUTY_MIN = 0U,
	DUTY_MAX = 100U
};

typedef uint8_t dutyCycle_t;

typedef struct {
	pwmChIdx_t chIndex;
	dutyCycle_t dutyCycle;
} chCtrlMsg_t;

#endif /* CHANNELCTRL_H_ */
