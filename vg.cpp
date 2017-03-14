/*
 * vg.cpp
 *
 * Created: 15.12.2016 14:36:22
 *  Author: gumerovmn
 */ 

#include "vg.h"

#include "system/path.h"

namespace ns_vg
{
	uint16_t lenTube = 125;
	uint8_t  lenTubeNew = 0;
	 int8_t  lenTubeSta = 0;
	EEMEM uint16_t eeDistance[8] = {0, 1400, 2800, 4200, 5600, 7000, 13250, 13950};
	EEMEM int16_t eeCorSensor[8] = { -30, -30, -30, -30, -30, -30, 0, 0};
}
