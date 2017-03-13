/* 
* line.h
*
* Created: 19.10.2016 15:24:45
* Author: GumerovMN
*/


#ifndef __LINE_H__
#define __LINE_H__

//#include <stdint-gcc.h>
#include "system/path.h"

namespace ns_line
{
	void Init();
	void Interrupt();
	void Start();
	int8_t Render(uint16_t *dlina);
	extern uint8_t mode;
	extern const uint8_t sensorsN;
	
	extern tc_pin<uint16_t, uint16_t>* sensors[];		// массив датчиков
//	extern uint8_t sensMapN;
}; //line

#endif //__LINE_H__
