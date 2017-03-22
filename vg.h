/*
 * vg.h
 *
 * Created: 15.12.2016 14:36:39
 *  Author: gumerovmn
 */ 


#ifndef VG_H_
#define VG_H_

#include "system/path.h"
namespace ns_vg
{
	extern uint16_t lenTube;
	extern uint8_t  lenTubeNew;
	extern  int8_t  lenTubeSta;
	extern EEMEM uint16_t eeDistance[];
	extern EEMEM int16_t eeCorSensor[2][8];
	extern EEMEM uint8_t eeBaseSQ7;
	extern int8_t  tube_bNb;
	extern int8_t  tube_bNe;
	extern int16_t tube_udl;
}


#endif /* VG_H_ */