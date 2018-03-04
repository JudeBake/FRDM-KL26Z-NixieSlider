/*
 * deviceCtrl.h
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

#ifndef DEVICECTRL_H_
#define DEVICECTRL_H_

typedef enum {
	DEVICE_DOWN,
	DEVICE_UP
} devCmd_t;

typedef int16_t displacement_t;

typedef struct {
	devCmd_t	commande;
	displacement_t displacement;
} devCtrlMsg_t;

#endif /* DEVICECTRL_H_ */