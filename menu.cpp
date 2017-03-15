/* 
* menu.cpp
*
* Created: 17.10.2016 15:41:31
* Author: GumerovMN
*/
#include <avr/pgmspace.h>
#include "menu.h"
#include "system/path.h"

namespace ns_menu
{
	unsigned char mode = 0;
	unsigned int  timeOut = 0;
	void Dupm(unsigned char key);
	// 0 : no key (view)
	// 1-4 : main key
	// 5 : add key
	// 6 : set mode
	#define SETMODE 6
	// 7 : timeout
	#define TIMEOUT 7
	// *************************************************
	typedef void(*TyFnMn)(unsigned char);
	extern const TyFnMn MassFnMn[][8];
	#define FnMenu(mode, stat, k) ((TyFnMn)pgm_read_ptr(& MassFnMn[mode][stat]))(k)
	//
	void SetMode(unsigned char modeS)
	{
		mode = modeS;
		FnMenu(modeS, SETMODE, 0);
	}
	void Init()
	{
		SetMode(0);
	}
	void Main()
	{
		unsigned char key;
		unsigned char keyStat = key4->Read(&key);
		unsigned int to;
		/*scr::Digit(scr::SetPosition(11,0), 2, keyStat);
		if (keyStat>0)
		scr::Digit(scr::SetPosition(14,0), 2, key);*/
		CRITICAL_SECTION
		{
			to = timeOut;
		}
		if (to == 1)
		{
			CRITICAL_SECTION
			{
				timeOut = 0;
			}
			FnMenu(mode, TIMEOUT, key);
		}
		else
		{
			FnMenu(mode, keyStat, key);
		}
	}
	void Interrupt()
	{
		CRITICAL_SECTION
		{
			if (timeOut > 1) timeOut--;
		}
	}
	void Dupm(unsigned char par) {}
	// локальные прототипы
	void Menu_CheckPass(unsigned char k);
	void Menu_CheckPassView(unsigned char k);
	void Menu_CheckPassBack(unsigned char k);
	void Menu_CheckPassMinus(unsigned char k);
	void Menu_CheckPassPlus(unsigned char k);
	void Menu_CheckPassVv(unsigned char k);
	void Menu_Select(unsigned char);
	void Menu_SelectView(unsigned char k);
	void Menu_SelectBack(unsigned char k);
	void Menu_SelectMinus(unsigned char k);
	void Menu_SelectVV(unsigned char k);
	void Edit_Sensor(unsigned char k);
	void Edit_SensorView(unsigned char k);
	void Set_PassView(unsigned char k);
	// ============================================
		// определение строки
		#define MAIN				0
		#define MENU_CHECK_PASS		1
		#define MENU_SELECT			2
		#define EDIT_SENSOR			3
		#define TIMEOUT_TO_MAIN		4
		#define SET_PASS			5
		#define TIMEOUT_TO_MNSEL	6
		#define COR_COR_SENS		7
		//
		//			переменные
		//
		// текущяя позитция при вводе пароля
		uint8_t curPosPass;
		// вводимый пароль
		char inPass[5];
		char eePass[5] EEMEM         = {0, 0, 0, 0, 0 };
		const char ffPass[5] PROGMEM = {2, 2, 3, 6, 0 };
		// текущий датчик
		uint8_t curSensor;
		uint8_t curSensIdx;
		// временная для редактирование дистанции для датчиков
		uint16_t tmpDistance;
		//
	void Main_SetMode(unsigned char mod)
	{
		//mode = MAIN;
		timeOut = 0;
		key4->autoRepeat = false;
		scr::Clear();
		scr::pos = scr::SetPosition(0, 0);
		scr::String_P( &scr::pos, PSTR("L=") );
		if ( ns_vg::lenTubeSta==1 )
		{
			printf_P( PSTR("%5d "), ns_vg::lenTube);
		}
	}
	void Main_ViewMl(uint8_t k)
	{
		if (k == 6)
		{
			uint8_t pos = scr::SetPosition(8, 1);
			scr::Digit(&pos, 1, ns_vg::tube_bNb);
			scr::Digit(&pos, 1, ns_vg::tube_bNe);
			scr::Digit(&pos, 4, ns_vg::tube_udl);
		}
	}
	void Main_View(unsigned char k)
	{
		if (ns_vg::lenTubeNew)
		{
			ns_vg::lenTubeNew = 0;
			int8_t sta = ns_vg::lenTubeSta;
			scr::Clear();
			uint8_t pos = scr::SetPosition(0, 0);
			scr::String_P( &pos, PSTR("L=") );
			if ( sta==-1 )
			{
				scr::String_P(&pos, PSTR("Реверс"));
			}
			if ( sta==1 )
			{
				scr::Digit(&pos, 6, ns_vg::lenTube);
			}
			if ( sta==2 )
			{
				scr::String_P(&pos, PSTR("Ошибка"));
			}
		}
		// =========
		for (uint8_t i=0; i<8; i++)
		{
			if ( ns_line::sensors[i]->readSensor() )
			{
				scr::PutChar(scr::SetPosition(i, 1), 0);
			}
			else
			{
				scr::PutChar(scr::SetPosition(i, 1), 1);
			}
		}
	}
	void Main_menu(unsigned char k)
	{
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		SetMode(MENU_CHECK_PASS);
		// SetMode(MENU_SELECT);
	}
	void Main_mins(unsigned char k)
	{
		//xx_count--;
		Main_View(k);
	}
	void Main_plus(unsigned char k)
	{
		//xx_count++;
		Main_View(k);
	}
	void Main_rOff(unsigned char k) { key4->autoRepeat = false;	}
	void Main_rOn (unsigned char k) { key4->autoRepeat = true;	}
	// ==============================
	// MENU_CHECK_PASS
	void Menu_CheckPass(unsigned char k)
	{
		//mode = MENU_CHECK_PASS;
		key4->autoRepeat = false;
		scr::Clear();
		scr::pos = scr::SetPosition(0, 0);
		printf_P(PSTR("Меню-пароль:"));
		curPosPass = 0;
		for (uint8_t i = 0; i < 5; i++)
			inPass[i] = 0;
	}
	void Menu_CheckPassView(unsigned char k)
	{
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < 5;i++)
		{
			if (i < curPosPass)
			{
				scr::flicker = false;
				scr::PutChar(&pos, '*');
				continue;
			}
			if (i == curPosPass)
			{
				scr::flicker = true;
				scr::PutChar(&pos, '0' + inPass[i]);
				scr::flicker = false;
				continue;
			}
			scr::flicker = false;
			scr::PutChar(&pos, ' ');
		}
	}
	void Menu_CheckPassBack(unsigned char k)
	{
		if (curPosPass > 0)
		{
			curPosPass--;
			Menu_CheckPassView(k);
		}
		else SetMode(MAIN);// Main_SetMode(k);
	}
	void Menu_CheckPassMinus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass] > 0) inPass[curPosPass]--;
		Menu_CheckPassView(k);
	}
	void Menu_CheckPassPlus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass] < 9) inPass[curPosPass]++;
		Menu_CheckPassView(k);
	}
	void Menu_CheckPassVv(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass < 4)
		{
			//char t = inPass[curPosPass];
			curPosPass++;
			//inPass[curPosPass] = t;
			Menu_CheckPassView(k);
		}
		else
		{
			// проверка пароля
			bool fl1 = true;
			bool fl2 = true;
			for (uint8_t i=0; i<5; i++)
				{
					if (inPass[i] != (char)eeprom_read_byte((uint8_t *)&eePass[i])) fl1 = false;
					if (inPass[i] != (char)pgm_read_byte((uint8_t *)&ffPass[i])) fl2 = false;
				}
			if (fl1 || fl2)
				SetMode(MENU_SELECT);
			else
			{
				scr::Clear();
				printf_P(PSTR("Ошибка ввода"));
				scr::pos = scr::SetPosition(0, 1);
				printf_P(PSTR("пароля"));
				CRITICAL_SECTION	{timeOut = 5000;}
				SetMode(TIMEOUT_TO_MAIN);
			}
			
		}
	}
	// ==============================
	const char menuSelectTab_str1[] PROGMEM = "Коррекция длины";
	const char menuSelectTab_str2[] PROGMEM = "Дист. SQ1<->SQ8";
	const char menuSelectTab_str3[] PROGMEM = "Дист. SQ2<->SQ8";
	const char menuSelectTab_str4[] PROGMEM = "Дист. SQ3<->SQ8";
	const char menuSelectTab_str5[] PROGMEM = "Дист. SQ4<->SQ8";
	const char menuSelectTab_str6[] PROGMEM = "Дист. SQ5<->SQ8";
	const char menuSelectTab_str7[] PROGMEM = "Дист. SQ6<->SQ8";
	const char menuSelectTab_str8[] PROGMEM = "Дист. SQ7<->SQ8";
	const char menuSelectTab_str9[] PROGMEM = "Уст. пароль    ";
	const char *menuSelectTab[] = {
		menuSelectTab_str1,
		menuSelectTab_str2,
		menuSelectTab_str3,
		menuSelectTab_str4,
		menuSelectTab_str5,
		menuSelectTab_str6,
		menuSelectTab_str7,
		menuSelectTab_str8,
		menuSelectTab_str9
	};
	void Menu_Select(unsigned char k)
	{
		//mode = MENU_SELECT;
		key4->autoRepeat = false;
		scr::Clear();
		scr::pos = scr::SetPosition(0, 0);
		printf_P(PSTR("Меню-выбор:"));
		curPosPass = 0;
		Menu_SelectView(k);
	}
	void Menu_SelectBack(unsigned char k)
	{
		SetMode(MAIN);
	}
	void Menu_SelectView(unsigned char k)
	{
		// очистка строки
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < scr::Stolbcov(); i++) scr::PutChar(&pos, ' ');
		scr::pos = scr::SetPosition(0, 1);
		//printf_P(PSTR("%2d)"), (curPosPass+1));
		printf_P(menuSelectTab[curPosPass]);
	}
	void Menu_SelectMinus(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass > 0)
		{
			curPosPass--;
			Menu_SelectView(k);
		}
	}
	void Menu_SelectPlus(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass < (sizeof(menuSelectTab)/sizeof(char *))-1 )
		{
			curPosPass++;
			Menu_SelectView(k);
		}
	}
	void Menu_SelectVV(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass ==  0) { curSensIdx = 7; curSensor = 1; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  1) { curSensIdx = 6; curSensor = 2; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  2) { curSensIdx = 5; curSensor = 3; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  3) { curSensIdx = 4; curSensor = 4; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  4) { curSensIdx = 3; curSensor = 5; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  5) { curSensIdx = 2; curSensor = 6; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  6) { curSensIdx = 1; curSensor = 7; SetMode(EDIT_SENSOR); return; }
		if (curPosPass ==  7) {  return; }
		if (curPosPass ==  8) { SetMode(SET_PASS); return; }
	}
	// ==============================
	void Edit_Sensor(unsigned char k)
	{
		//mode = EDIT_SENSOR;
		key4->autoRepeat = false;
		tmpDistance = eeprom_read_word(&ns_vg::eeDistance[curSensIdx]);
		scr::Clear();
		scr::pos = scr::SetPosition(0, 0);
		printf_P(PSTR("Настройка"));
		Edit_SensorView(k);
	}
	void Edit_SensorView(unsigned char k)
	{
		scr::pos = scr::SetPosition(0, 1);
		printf_P(PSTR("Д%1d: дист.=%5d"), curSensor, tmpDistance);
	}
	void Edit_SensorBack(unsigned char k)
	{
		key4->autoRepeat = false;
		SetMode(MENU_SELECT);
	}
	void Edit_SensorMinus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (tmpDistance > 0)
		{
			tmpDistance--;
			Edit_SensorView(k);
		}
	}
	void Edit_SensorPlus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (tmpDistance < 15000)
		{
			tmpDistance++;
			Edit_SensorView(k);
		}
	}
	void Edit_SensorMulti(unsigned char k)
	{
		key4->autoRepeat = true;
		if (k == 3)
		{
			if (tmpDistance >= 10)
			{
				tmpDistance -= 10;
				Edit_SensorView(k);
			}
			else if (tmpDistance > 0)
			{
				tmpDistance = 0;
				Edit_SensorView(k);
			}
		}
		if (k == 5)
		{
			if (tmpDistance <= 14990)
			{
				tmpDistance += 10;
				Edit_SensorView(k);
			}
			else if (tmpDistance < 15000)
			{
				tmpDistance = 15000;
				Edit_SensorView(k);
			}
		}
		//scr::Digit(scr::SetPosition(12,0), 2, k);
	}
	void Edit_SensorVV(unsigned char k)
	{
		key4->autoRepeat = false;
		eeprom_write_word(&ns_vg::eeDistance[curSensIdx], tmpDistance);
		//SetMode(MENU_SELECT);
		scr::pos = scr::SetPosition(0, 1);
		printf_P(PSTR("** сохранено **"));
		CRITICAL_SECTION{ timeOut = 4000; }
		SetMode(TIMEOUT_TO_MNSEL);
	}
	// ==============================
	void Timeout_To_Main(unsigned char k)
	{
		key4->autoRepeat = false;
		//mode = TIMEOUT_TO_MAIN;
	}
	void TimeoutToMainE(unsigned char k)
	{
		SetMode(MAIN);
	}
	// ==============================
	// установка нового пароля
	void Set_Pass(unsigned char k)
	{
		scr::Clear();
		printf_P(PSTR("новый пароль:"));
		for (uint8_t i=0; i<5; i++) inPass[i] = 0;
		curPosPass = 0;
		Set_PassView(k);
		key4->autoRepeat = false;
	}
	void Set_PassView(unsigned char k)
	{
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < 5;i++)
		{
			if (i < curPosPass)
			{
				scr::flicker = false;
				scr::PutChar(&pos, '*');
				continue;
			}
			if (i == curPosPass)
			{
				scr::flicker = true;
				scr::PutChar(&pos, '0' + inPass[i]);
				scr::flicker = false;
				continue;
			}
			scr::flicker = false;
			scr::PutChar(&pos, ' ');
		}
	}
	void Set_PassBack(unsigned char k)
	{
		key4->autoRepeat = false;
		SetMode(MENU_SELECT);
	}
	void Set_PassMinus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass]>0)
		{
			inPass[curPosPass]--;
			Set_PassView(k);
		}
	}
	void Set_PassPlus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass]<9)
		{
			inPass[curPosPass]++;
			Set_PassView(k);
		}
	}
	void Set_PassVV(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass < 4)
		{
			char t = inPass[curPosPass];
			curPosPass++;
			inPass[curPosPass] = t;
			Menu_CheckPassView(k);
		}
		else
		{
			for (uint8_t i=0; i<5; i++)
				eeprom_write_byte((uint8_t *)&eePass[i], inPass[i]);
			scr::Clear();
			printf_P(PSTR("Новый пароль"));
			scr::pos = scr::SetPosition(0, 1);
			printf_P(PSTR("установлен"));
			CRITICAL_SECTION { timeOut = 10000; }
			SetMode(TIMEOUT_TO_MAIN);
		}
	}
	// ==============================
	void TimeoutToMnSel(unsigned char k)
	{
		SetMode(MENU_SELECT);
	}
	// ==============================
	void Cor_CorSens_Vi(uint8_t k)
	{
		if (ns_vg::lenTubeNew)
		{
			ns_vg::lenTubeNew = 0;
			int8_t sta = ns_vg::lenTubeSta;
			scr::Clear();
			uint8_t pos = scr::SetPosition(0, 0);
			scr::String_P( &pos, PSTR("L=") );
			if ( sta==-1 )
			{
				scr::String_P(&pos, PSTR("Реверс"));
			}
			if ( sta==1 )
			{
				scr::Digit(&pos, 6, ns_vg::lenTube);
			}
			if ( sta==2 )
			{
				scr::String_P(&pos, PSTR("Ошибка"));
			}
		}
	}
	void Cor_CorSens(uint8_t k)
	{
		CRITICAL_SECTION { timeOut = 0; }
		key4->autoRepeat = false;
	}
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ==============================
	// ============================================
	// 0 : no key (view)
	// 1-4 : main key
	// 5 : add key
	// 6 : timeout
	const TyFnMn MassFnMn[][8] PROGMEM =
	{//					view				key1				key2				key3				key4				key5			 setmode			 timeout          
		{	       Main_View,	       Main_menu,				Dupm,				Dupm,				Dupm,		 Main_ViewMl,       Main_SetMode,				Dupm },	// 0 main
		{ Menu_CheckPassView, Menu_CheckPassBack,Menu_CheckPassMinus, Menu_CheckPassPlus,	Menu_CheckPassVv,				Dupm,     Menu_CheckPass,				Dupm },	// 1 menu check pass
		{				Dupm,	 Menu_SelectBack,	Menu_SelectMinus,	 Menu_SelectPlus,	   Menu_SelectVV,				Dupm,		 Menu_Select,				Dupm },	// 2 menu select
		{				Dupm,	 Edit_SensorBack,	Edit_SensorMinus,	 Edit_SensorPlus,	   Edit_SensorVV,	Edit_SensorMulti,		 Edit_Sensor,				Dupm }, // 3 edit sensor
		{				Dupm,		Main_SetMode,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,	  TimeoutToMainE }, // 4 timeout to main
		{				Dupm,		Set_PassBack,	   Set_PassMinus,		Set_PassPlus,		  Set_PassVV,				Dupm,			Set_Pass,				Dupm }, // 5 set password
		{				Dupm,		 Menu_Select,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,	  TimeoutToMnSel }, // 6 TIMEOUT_TO_MNSEL
		{	  Cor_CorSens_Vi,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,		 Cor_CorSens,				Dupm },	// 7 Cor_CorSens
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm }
	};

}
