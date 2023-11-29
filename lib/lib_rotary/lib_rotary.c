/*
 *  File name:  lib_rotary.c
 *  Date first: 12/29/2017
 *  Date last:  06/24/2018
 *
 *  Description: STM8 Library for ALPS rotary encoder
 *
 *  Author:     Richard Hodges
 *
 *  Copyright (C) 2017, 2018 Richard Hodges. All rights reserved.
 *  Permission is hereby granted for any use.
 *
 ******************************************************************************
 *
 *  Pinout:
 *
 *  PA1: encoder A
 *  PA2: encoder B
 */

#include "stm8s.h"
#include "stm8s_gpio.h"
#include "lib_rotary.h"

#define PIN_MASK (GPIO_PIN_3 | GPIO_PIN_4)

#define PORT (GPIOB)
#define PIN_1 (GPIO_PIN_4)
#define PIN_2 (GPIO_PIN_3)

static char val_cur;
static char val_max;
static char direction;
static char pins_last;
static char roll_up, roll_down;

/******************************************************************************
 *
 *  Setup
 *  in: number of steps
 */

void alps_init(char steps, char option)
{

    GPIO_Init(PORT, (GPIO_Pin_TypeDef)PIN_1, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(PORT, (GPIO_Pin_TypeDef)PIN_2, GPIO_MODE_IN_PU_NO_IT);

    val_cur = 0;
    val_max = steps;
    direction = 0; 
    pins_last = GPIO_ReadInputData(PORT) & PIN_MASK;

    /* assume ALPS_LIMITS option */
    roll_up = steps - 1;	/* new value when val_cur hits maximum */
    roll_down = 1;		/* value (adjusted ) when val_cur goes < 0 */
    if (option == ALPS_ROLLOVER) {
	roll_up = 0;
	roll_down = steps;
    }
}

/******************************************************************************
 *
 *  Get current position (relative from setup)
 */

char alps_value(void)
{
    return val_cur;
}

/******************************************************************************
 *
 *  Poll switch (recommended: 1 to 4 milliseconds
 */

void alps_poll(void)
{
    char	pins_cur;
    char	diff;

    pins_cur = GPIO_ReadInputData(PORT) & PIN_MASK;
    if ((pins_cur == 0) ||	/* are both pins zero? */
	(pins_cur == PIN_MASK)) { /* both set? */
	    if (pins_cur == pins_last)
		return;
	    pins_last = pins_cur;
	    if (direction) {
		val_cur++;
		if (val_cur == val_max)
		    val_cur = roll_up;
		return;
	    }
	    if (val_cur == 0)
		val_cur = roll_down;
	    val_cur--;
	    return;
	}
	diff = pins_cur ^ pins_last;
	direction = 0;		/* counter-clockwise */
	if (diff == PIN_2)		/* pin B changed */
	    direction = 1;	/* clockwise */
}

void alps_set_value(char value)
{
	val_cur = value;
}

/******************************************************************************

The ALPS rotary encoder switch has 30 detent positions, each one spaced 12
degrees apart. The three (encoder) pins are A (left), Common, B (right).
Pins A and B should be pulled up to Vcc; 10K resistors are recommended to
keep current under 1mA. Here, the input pins have internal pull-ups of
about 40K.

Both A and B are stable at detent position, and both A and B will have
the same output. When the encoder is rotated clockwise, B will change
first, then A. For counterclockwise, A will change first, then B.

Instead of using Interrupt On Change, poll at a rate often enough to
catch the fastest transitions. One full revolution in half a second
might be a good starting point, which gives 60 pin changes per second,
or one every 16 milliseconds. Oversampling by 4 would give one poll
every 4 milliseconds, which is probably already a timer interrupt.

0.      Read pins A and B, save starting status.

Polling procedure:

1.      Are pins A and B the same?
        NO: Which changed from saved status? Set direction flag. Exit.
        YES: Continue to 2.

2.      Are A and B equal to last saved status?
        YES: Encoder at same detent position. Ignore. Exit.
        NO:  Save new status. Eval direction flag and indicate rotation.
*/
