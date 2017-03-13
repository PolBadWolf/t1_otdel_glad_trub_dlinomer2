/* 
* Device.cpp
*
* Created: 04.02.2017 11:51:20
* Author: User
*/


#include "path.h"
#include "Device.h"

FILE*	lcdStdOut;
FILE*	rs232StdOut;
tc_key4*	key4 = NULL;

namespace ns_device
{
	void Init()
	{
		ns_LcdKeyTimer::Init();
		ns_timerMain::Init();
		lcdStdOut = ns_lcd::Init(24);
		key4 = new tc_key4;
		ns_clock::Init();
		rs232StdOut = ns_rs232::Init(baud9600, DISABLE, bit8, size16, size128);
		// =========
	}
	void Interrupt()
	{
		// lcd
		ns_lcd::Interrupt();
		// key
		key4->Interrupt();
		// clock rt
		ns_clock::Interrupt();
	}
}