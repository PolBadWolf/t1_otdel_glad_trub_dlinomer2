/* 
* Device.h
*
* Created: 04.02.2017 11:51:20
* Author: User
*/


#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "LcdKey_Timer.h"
#include "timer2.h"
#include "Lcd.h"
#include "key4.h"
#include "pin.h"
#include "Clock.h"
#include "rs232.h"


namespace ns_device
{
	void Init();
	void Interrupt();
}

extern FILE* lcdStdOut;
extern FILE* rs232StdOut;
extern tc_key4*	key4;

#define scr ns_lcd
#define rsTele ns_rs232
#define stdTeleOut rs232StdOut

#endif //__DEVICE_H__
