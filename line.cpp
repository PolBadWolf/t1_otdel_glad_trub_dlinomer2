/* 
* line.cpp
*
* Created: 19.10.2016 15:24:45
* Author: GumerovMN
*/


#include "line.h"
#include "system/path.h"
// #include <stdlib.h>

namespace ns_line
{
	const uint8_t sensorsN = 8;
	tc_pin<uint16_t, uint16_t>* sensors[8];		// массив датчиков
	int32_t count;				// счетчик тайминга
	int32_t times[8][2];			// массив таймингов сработок датчиков
	uint16_t distance[8] = {0, 1400, 2800, 4200, 5600, 7000, 13250, 13950};
	uint8_t mode = 2;						// режим работы
//	uint16_t timeOut = 0;					// тайм аут
	// карта датчиков
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
		uint16_t kI = (uint8_t)(((uint32_t)10 ) * ((uint32_t)FtTimerMain) / ((uint32_t)1000));
		uint16_t kF = (uint8_t)(((uint32_t)500) * ((uint32_t)FtTimerMain) / ((uint32_t)1000));
		uint16_t kS = (uint8_t)(((uint32_t)500) * ((uint32_t)FtTimerMain) / ((uint32_t)1000));
		sensors[0] = new tc_pin<uint16_t, uint16_t>(&DDRD, &PORTD, &PIND, 5, kI, kF, kS);
		sensors[1] = new tc_pin<uint16_t, uint16_t>(&DDRD, &PORTD, &PIND, 4, kI, kF, kS);
		sensors[2] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 7, kI, kF, kS);
		sensors[3] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 6, kI, kF, kS);
		sensors[4] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 5, kI, kF, kS);
		sensors[5] = new tc_pin<uint16_t, uint16_t>(&DDRB, &PORTB, &PINB, 4, kI, kF, kS);
		sensors[6] = new tc_pin<uint16_t, uint16_t>(&DDRE, &PORTE, &PINE, 7, kI, kF, kS);
		sensors[7] = new tc_pin<uint16_t, uint16_t>(&DDRE, &PORTE, &PINE, 6, kI, kF, kS);
		Start();
	}
	void Start()
	{
		// обнуление таймингов
		count = 0;
		sensN = 0;	// последний сработавший датчик
		mode = 1;	// ожидание ухода трубы
	}
	void Interrupt()
	{
		// read line
		for (uint8_t i = 0; i < 8; i++)
		{
			sensors[i]->tr = 9;
			sensors[i]->Interrupt();
		}
		
		if (mode == 1)
		{	// ожидание ухода трубы
			if ( (sensors[7]->readSensor() != 0) && (sensors[6]->readSensor() != 0) && (sensors[5]->readSensor() != 0) )
			{
				// обнуление таймингов
				count = 0;	// счетчик тайминга
				for (uint8_t i = 0; i < 8; i++)
				{
					times[i][0] = (uint32_t)0; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					times[i][1] = (uint32_t)0;
				}
				mode = 2;	// цикл сбора таймингов
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
						&& (sensors[0]->readSensor() != 0) // пусто - нет трубы на датчиках
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
			// чтение таймингов переключения датчиков
			for (uint8_t i=0; i<8; i++)
			{
				if (sensors[i]->tr == 0)
				{
					if (times[i][0] == 0)
					{
						times[i][0] = count;
						if ((i+1) < 8) times[i+1][0] = 0;
					}
				}
				if (sensors[i]->tr == 1)
				{
					times[i][1] = count;
				}
			}
			// сработка последнего датчика
			if ((sensors[7]->tr == 0) && (sensors[6]->readSensor() == 0))
			{
				mode = 0;	// расчет
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
			//fprintf_P(stdTeleOut, PSTR("%12d"), distance[i]);
			rsTele::Digit(12, distance[i]);
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
		fprintf_P(stdTeleOut, PSTR("\r\nTELE End\r\n"));
		
		
	}
	 // ---
	int8_t Render(uint16_t *dlina)
	{
		if (mode != 0)	// расчет ?
			return 0;	// расчет не требуется

		// init distance
		for (uint8_t i = 0; i < 8; i++)
		{
			distance[i] = eeprom_read_word(&ns_vg::eeDistance[i]);
		}
		uint8_t status;
		// ===================


		// проверка реверса
		if ( (times[7][0] <= times[6][0]) || (times[6][0] <= times[5][0]) )
		{	// реверс
			mode = 1;	// ожидание ухода трубы
			status = -1;
			Telemetry(status, -1, -1, -1, 0, 0);
			return status;	// реверс
		}
		// ======
		
		// поиск начального датчика
		int8_t n;
		int8_t bNe = -1;
		for (n=5; n>=0; n--)
		{
			if ( times[n][0] == 0 ) continue; // датчик не работал
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
		{	// ошибка
			mode = 1;
			status = 2;
			Telemetry(status, -1, -1, -1, 0, 0);
			return status;
		}
		// render solution
		int32_t ex[3];
		if (times[7][0] > times[n][1])					// base = d(7)-d(n)			cor -
				ex[0] = times[7][0] - times[n][1];
		else	ex[0] = 999999;
		if (times[6][0] <= times[n][1])					// base = d(6)-d(n)			cor +
				ex[1] = times[n][1] - times[6][0];
		else	ex[1] = 999999;
		if (times[6][0] > times[n][1])					// base = d(6)-d(n)			cor -
				ex[2] = times[6][0] - times[n][1];
		else	ex[2] = 999999;
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
		base = distance[bNe] - distance[n];
		dochetT = times[n][1] - times[bNe][0];
		uint16_t speedLen = distance[7] - distance[6];
		int32_t speedTime = times[7][0] - times[6][0];
		int16_t udl = ( ((int64_t)dochetT) * ((int64_t)speedLen) / ((int64_t)speedTime) );
		*dlina = base + udl;
		mode = 1;
		status = 1;
		Telemetry(status, exN, n, bNe, base, udl);
		return status;
	}
} //line
