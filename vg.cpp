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
	//uint16_t eeDistance[10] EEMEM = {0, 0, 13950, 13250, 7000, 5600, 4200, 2800, 1400, 0};
	//uint16_t eeDistance[10] EEMEM = {0, 1400, 2800, 4200, 5600, 7000, 13250, 13950, 0, 0};
	uint16_t eeDistance[8] EEMEM = {0, 1400, 2800, 4200, 5600, 7000, 13250, 13950};
}
