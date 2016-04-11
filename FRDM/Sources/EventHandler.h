/*
 * EventHandler.h
 *
 *  Created on: 17.03.2016
 *      Author: mineichen
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

typedef enum {
	TIMER1_OVERFLOW,
	BUTTON_RED_PRESSED,
	BUTTON_BLUE_PRESSED,
	BUTTON_YELLOW_PRESSED,
	BUTTON_GREEN_PRESSED,
	BUTTON_JOYSTICK_PRESSED,
	JOYSTICK_MOVE,
	LED_HEARTBEET,
	EVENTS_NOF_TYPES
} EventType;

#include "SharedEventHandler.h"

#endif /* SOURCES_COMMON_EVENTHANDLER_H_ */

