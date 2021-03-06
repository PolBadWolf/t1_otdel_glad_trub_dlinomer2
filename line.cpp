/* 
* line.cpp
*
* Created: 19.10.2016 15:24:45
* Author: GumerovMN
*/


#include "line.h"
#include "system/path.h"
// #include <stdlib.h>

#define msTocycl(zd)	( (uint16_t)( ((uint32_t)zd) * ((uint32_t)FtTimerMain) / ((uint32_t)1000) ) )

namespace ns_line
{
	const uint8_t sensorsN = 8;
	tc_pin<uint16_t, uint16_t>* sensors[8];		// ������ ��������
	int32_t count;				// ������� ��������
	int32_t times[8][2];			// ������ ��������� �������� ��������
	uint16_t distance[8] = {0, 1400, 2800, 4200, 5600, 7000, 13250, 13950};
	int16_t corSensor[8][2];
	uint8_t mode = 2;						// ����� ������
//	uint16_t timeOut = 0;					// ���� ���
	// ����� ��������
//	const uint8_t sensMap[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t sensN = 0;
	//
	uint16_t l_len = 0;
	uint32_t l_cena = 0;
	// ========
	//__port(PORTD).bit6
	// ======================================
	void Init()
	{
		// interal, front, spad
		sensors[0] = new tc_pin<uint16_t, uint16_t>(&DDRD, &PORTD, &PIND, 5, msTocycl(10), msTocycl(10), msTocycl(500));
		sensors[1] = new tc_pin<uint16_t, uint16_t>(&DDRD, &PORTD, &PIND, 4, msTocycl(10), msTocycl(10), msTocycl(500));
		sensors[2] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 7, msTocycl(10), msTocycl(10), msTocycl(500));
		sensors[3] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 6, msTocycl(10), msTocycl(10), msTocycl(500));
		sensors[4] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 5, msTocycl(10), msTocycl(10), msTocycl(500));
		sensors[5] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 4, msTocycl(10), msTocycl(10), msTocycl(500));
		sensors[6] = new tc_pin<uint16_t, uint16_t>(&DDRE, &PORTE, &PINE, 7, msTocycl(10), msTocycl(500), msTocycl(10));
		sensors[7] = new tc_pin<uint16_t, uint16_t>(&DDRE, &PORTE, &PINE, 6, msTocycl(10), msTocycl(500), msTocycl(10));
		Start();
	}
	void Start()
	{
		// ��������� ���������
		count = 0;
		sensN = 0;	// ��������� ����������� ������
		mode = 1;	// �������� ����� �����
	}
	void Interrupt()
	{
		// read line
		for (uint8_t i = 0; i < 8; i++)
		{
			sensors[i]->tr = 9;
			sensors[i]->trFr = 0;
			sensors[i]->trSp = 0;
			sensors[i]->Interrupt();
		}
		
		if (mode == 1)
		{	// �������� ����� �����
			if ( (sensors[7]->readSensor() != 0) && (sensors[6]->readSensor() != 0) ) //&& (sensors[5]->readSensor() != 0) )
			{
				// ��������� ���������
				count = 0;	// ������� ��������
				for (uint8_t i = 0; i < 8; i++)
				{
					times[i][0] = (uint32_t)0; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					times[i][1] = (uint32_t)0;
				}
				mode = 2;	// ���� ����� ���������
			}
		}
		if (mode == 2)
		{
			// start count
			if (count == 0)
			{
				for (uint8_t i=0; i<8; i++)
				{
					if (sensors[i]->readSensor() == 0)
					{
						count = 1;
					}
				}
				//if (sensors[0]->tr	== 0) count = 1;
			}
			else
			{
				count++;
				// !!!no tube ?
				{
					if ( true
						&& (sensors[0]->readSensor() != 0) // ����� - ��� ����� �� ��������
						&& (sensors[1]->readSensor() != 0)
						&& (sensors[2]->readSensor() != 0)
						&& (sensors[3]->readSensor() != 0)
						&& (sensors[4]->readSensor() != 0)
						&& (sensors[5]->readSensor() != 0)
						&& (sensors[6]->readSensor() != 0)
						&& (sensors[7]->readSensor() != 0)
					) 
					{
						mode = 1;
						return;
					}
				}				
			}        
			// ������ ��������� ������������ ��������
			for (uint8_t i = 0; i < 8; i++)
			{
				if (sensors[i]->trFr != 0)
				{
					times[i][0] = count;
					times[i][1] = 0;
				}
				if (sensors[i]->trSp != 0)
				{
					times[i][1] = count;
				}
			}
			// �������� ���������� �������
			if ((sensors[7]->trFr != 0) && (sensors[6]->readSensor() == 0))
			{
				mode = 0;	// ������
			}
			// ===
		}
		
	}
	// ===
	// find
	void SortAB(uint32_t* min, uint32_t* max)
	{
		uint32_t tmp;
		if (min < max) return;
		tmp = *min;
		*min = *max;
		*max = tmp;
	}
	// ================================================================================================================================================
	void Telemetry(uint8_t status, int8_t exN, int8_t bNb, int8_t bNe, uint16_t base, int16_t udl)
	{
		fprintf_P(stdTeleOut, PSTR("\r\nTELE Begin\r\n"));
		fprintf_P(stdTeleOut, PSTR("status: "));
		if (status == -1) fprintf_P(stdTeleOut, PSTR("Reversn"));
		else if (status == 1) fprintf_P(stdTeleOut, PSTR("Render lens"));
		else if (status == 2) fprintf_P(stdTeleOut, PSTR("Error Sensors"));
		else if (status == 3) fprintf_P(stdTeleOut, PSTR("Big tube ?"));
		else fprintf_P(stdTeleOut, PSTR("Unknown status"));
		fprintf_P(stdTeleOut, PSTR("\r\n"));
		fprintf_P(stdTeleOut, PSTR("exN=%4d, baseNb=%2d, baseNe=%2d, base=%5d, udl=%7d\r\n"), exN, bNb, bNe, base, udl);
		// out curent distance
		fprintf_P(stdTeleOut, PSTR("Distance:    "));
		for (uint8_t i=0; i<8; i++)
		{
			rsTele::Digit(12, distance[i]);
		}
		fprintf_P(stdTeleOut, PSTR("\r\n"));
		// correction
		fprintf_P(stdTeleOut, PSTR("Corr. in :   "));
		for (uint8_t i=0; i<8; i++)
		{
			rsTele::String_P(PSTR("      "));
			rsTele::Digit(6, corSensor[i][0]);
		}
		fprintf_P(stdTeleOut, PSTR("\r\n"));
		fprintf_P(stdTeleOut, PSTR("Corr. out :  "));
		for (uint8_t i=0; i<8; i++)
		{
			rsTele::String_P(PSTR("      "));
			rsTele::Digit(6, corSensor[i][1]);
		}
		fprintf_P(stdTeleOut, PSTR("\r\n"));
		//rsTele::String_P(PSTR("\r\n"));
		// out timengs
		fprintf_P(stdTeleOut, PSTR("Sensor in:   "));
		for (uint8_t i=0; i<8; i++)
		{
			//fprintf_P(stdTeleOut, PSTR("%12d"), times[i][0]);
			rsTele::Digit(12, times[i][0]);
		}
		fprintf_P(stdTeleOut, PSTR("\r\n"));
		fprintf_P(stdTeleOut, PSTR("Sensor out:  "));
		for (uint8_t i=0; i<8; i++)
		{
			//fprintf_P(stdTeleOut, PSTR("%12d"), times[i][1]);
			rsTele::Digit(12, times[i][1]);
		}
		fprintf_P(stdTeleOut, PSTR("\r\n"));
		fprintf_P(stdTeleOut, PSTR("TELE End\r\n"));
		
		
	}
	 // ---
	int8_t Render(uint16_t *dlina)
	{
		if (mode != 0)	// ������ ?
			return 0;	// ������ �� ���������
		// init distance & corection
		for (uint8_t i = 0; i < 8; i++)
		{
			distance[i] = eeprom_read_word(&ns_vg::eeDistance[i]);
			corSensor[i][0] = (int16_t)eeprom_read_word( (uint16_t *)&ns_vg::eeCorSensor[0][i] );
			corSensor[i][1] = (int16_t)eeprom_read_word( (uint16_t *)&ns_vg::eeCorSensor[1][i] );
		}
		uint8_t status;
		// ===================


		// �������� �������
		if ( (times[7][0] <= times[6][0]) || (times[6][0] <= times[5][0]) )
		{	// ������
			mode = 1;	// �������� ����� �����
			status = -1;
			Telemetry(status, -1, -1, -1, 0, 0);
			return status;	// ������
		}
		// ======
		
		// ����� ���������� �������
		int8_t n;
		int8_t bNe = -1;
		for (n=5; n>=0; n--)
		{
			if ( times[n][0] == 0 ) continue; // ������ �� �������
			if ( times[n][1] > 0 ) break;
		}
		// big tube ?
		if (n < 0)
		{
			mode = 1;
			status = 3;
			Telemetry(status, -1, -1, -1, 0, 0);
			return status;
		}
		if (times[n][1] == 0)
		{
			mode = 1;
			status = 3;
			Telemetry(status, -1, -1, -1, 0, 0);
			return status;
		}
		
		if (times[n][1] == 0)
		{	// ������
			mode = 1;
			status = 2;
			Telemetry(status, -1, -1, -1, 0, 0);
			return status;
		}
		// render solution
		int32_t ex[3] = {999999, 999999, 999999};
		if (times[7][0] > times[n][1])					// base = d(7)-d(n)			cor -
				ex[0] = times[7][0] - times[n][1];
		else	ex[0] = 999999;
		if (eeprom_read_byte(&ns_vg::eeBaseSQ7))
		{
			if (times[6][0] <= times[n][1])					// base = d(6)-d(n)			cor +
					ex[1] = times[n][1] - times[6][0];
			else	ex[1] = 999999;
			if (times[6][0] > times[n][1])					// base = d(6)-d(n)			cor -
					ex[2] = times[6][0] - times[n][1];
			else	ex[2] = 999999;
		}
		//
		uint16_t base;
		int32_t dochetT = 999999;
		int8_t exN = -1;
		// select solution
		for (uint8_t i=0; i<3; i++)
		{
			if (ex[i] < dochetT)
			{
				dochetT = ex[i];
				exN = i;
				if (i == 0) bNe	= 7;
				else bNe = 6;
			}
		}
		base = (distance[bNe] - corSensor[bNe][0]) - (distance[n] + corSensor[n][1]);
		dochetT = times[n][1] - times[bNe][0];
		uint16_t sampleLen = (distance[7] - corSensor[7][0]) - (distance[6] - corSensor[6][0]);
		int32_t sampleTime = times[7][0] - times[6][0];
		int16_t udl = (uint16_t)( ((int64_t)dochetT) * ((int64_t)sampleLen) / ((int64_t)sampleTime) );
		*dlina = base + udl;
		mode = 1;
		status = 1;
		Telemetry(status, exN, n, bNe, base, udl);
		ns_vg::tube_bNb = n;
		ns_vg::tube_bNe = bNe;
		ns_vg::tube_udl = udl;
		return status;
	}
} //line
