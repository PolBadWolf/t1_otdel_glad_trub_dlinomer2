/*
 * GccApplication2.cpp
 *
 * Created: 27.08.2016 19:39:37
 * Author : User
 */ 

//#include <avr/pgmspace.h>

#include "system/path.h"

//unsigned long  EEMEM ee_var1 = 3;
//tc_pin<unsigned char> *l1 = new tc_pin<unsigned char>( &DDRE, &PORTE, &PINE, 4, 10 );

namespace ns_line
{
	extern tc_pin<unsigned char>* sensors[8];
}

int main(void)
{
			//char ssMass[256];
	// �������������
	ns_device::Init();
	stdout = lcdStdOut;
	//
	ns_module::Init();
	sei();
	//=========
	// ����� ���
	//(DDRC).bit5 = 1;
	//(PORTC).bit5 = 0;
	// ������ � 232
	fprintf_P(rs232StdOut, PSTR("�������� RS232\r\n"));
	/*
	// ������ �� ����� �� ���������
	printf("������");
	// ���������� ������
	scr::pos = ns_lcd::SetPosition(0,1);
	// ������ �� ����� �� ���������
	printf("�");
	// �������� ��������
	scr::flicker = true;
	printf("��");
	// ��������� ��������
	scr::flicker = false;
	printf(" ! ");
	// ������ ����� � "����� ������"
	scr::String_P(ns_lcd::SetPosition(16,0), PSTR("���"));
	// ���������� ������
	scr::pos = ns_lcd::SetPosition(16,1);
	// ������ � ������� �����
	fprintf_P(myStdOut1, PSTR("�����"));
	*/
	//ns_lcd::Interrupt();
    while (1) 
    {
		/*
		char sim;
		if (ns_rs232::ReadByte( &sim ) )
			fprintf_P(stdout, PSTR("%c"), sim);
		*/
		uint16_t dlina;
		int8_t stat = -10;
		stat = ns_line::Render(&dlina);
		
		if (stat == -1)
		{
			// ������ �����
			ns_vg::lenTubeSta = stat;
		}
		if (stat == 1)
		{
			// ����� �����
		//	ns_vg::izmTube->Push(stat, dlina);
			ns_vg::lenTube = dlina;
			ns_vg::lenTubeSta = stat;
			ns_vg::lenTubeNew = 1;
			fprintf_P(rs232StdOut, PSTR("����� �����=%5d��\r\n"), dlina);
		}
		if (stat == 2)
		{
			// ������ ������
			ns_vg::lenTubeSta = stat;
			ns_vg::lenTubeNew = 1;
		}
		//
		ns_module::Main();
		
		/*{
			scr::DigitZ(scr::SetPosition(10, 0), 2, ns_line::mode);
			uint8_t pos = scr::SetPosition(14, 0);
			scr::DigitZ(&pos, 1, ns_line::sensors[0]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[1]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[2]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[3]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[4]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[5]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[6]->tr);
			scr::DigitZ(&pos, 1, ns_line::sensors[7]->tr);
			pos = scr::SetPosition(14, 1);
			scr::DigitZ(&pos, 1, ns_line::sensors[0]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[1]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[2]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[3]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[4]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[5]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[6]->readSensor());
			scr::DigitZ(&pos, 1, ns_line::sensors[7]->readSensor());
		}*/
	//ns_eeprom::read(&sd, sizeof(sd), &x );
		/*
		*ll[0]->port = 8;
		*ll[1]->port = 9;
		PORTA = 1;
		DDRA = 2;
		*la[0] = 3;

		*lb[0].port = 5;
		*lb[1].port = 6;
		// (*((volatile BYTE_BITFIELD*)(&DDRA)))
		// (*((volatile ll->BYTE_BITFIELD*)(ll[0]->port)))
		// *ll[0]->bit->bit3 = 0;
		// unsigned char xx = ((volatile BYTE_BITFIELD*)(&PORTG));
		(PORTA).bit2 =1;
		(PORTG).bit3 = 0;
		(*((volatile BYTE_BITFIELD*)(&PINF))).bit5 = 1;
		(*((BYTE_BITFIELD*)(&PINF))).bit4 = 1;
		(*ll[0]).bit6 = 1;
		ns_key4::cicle();
		*/
    }
}

