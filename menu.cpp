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
	uint32_t  timeOut = 0;
	#define TIMEOUT_STND	90000
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
		uint32_t to;
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
		#define COR_COR_SENS_MEN	7
		#define COR_COR_SENS		8
		#define OFF_SQ7_BASE		9
		//
		//			переменные
		//
		// текущяя позитция при вводе пароля
		uint8_t curPosPass;
		// текущяя позитция при выборе меню
		uint8_t curPosMenu;
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
		CRITICAL_SECTION { timeOut = 0; }
		key4->autoRepeat = false;
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P( &pos, PSTR("L=") );
		if ( ns_vg::lenTubeSta==1 )
		{
			scr::Digit(&pos, 6, ns_vg::lenTube);
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
		curPosMenu = 0;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		SetMode(MENU_CHECK_PASS);
		// SetMode(MENU_SELECT);
	}
	// ==============================
	// MENU_CHECK_PASS
	void Menu_CheckPass(unsigned char k)
	{
		key4->autoRepeat = false;
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P(&pos, PSTR("Меню-пароль:") );
		// позитция ввода символа
		curPosPass = 0;
		//  очистить массив ввода символов
		for (uint8_t i = 0; i < 5; i++)
			inPass[i] = 0;
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Menu_CheckPassView(unsigned char k)
	{
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < 5;i++)
		{
			if (i < curPosPass)
			{
				scr::PutChar(&pos, '*');
				continue;
			}
			if (i == curPosPass)
			{
				scr::PutChar(&pos, '0' + inPass[i]);
				continue;
			}
			scr::PutChar(&pos, ' ');
		}
	}
	void Menu_CheckPassBack(unsigned char k)
	{
		if (curPosPass > 0)
		{
			curPosPass--;
			Menu_CheckPassView(k);
			CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
		}
		else SetMode(MAIN);
	}
	void Menu_CheckPassMinus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass] > 0) inPass[curPosPass]--;
		Menu_CheckPassView(k);
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Menu_CheckPassPlus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass] < 9) inPass[curPosPass]++;
		Menu_CheckPassView(k);
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Menu_CheckPassVv(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass < 4)
		{
			curPosPass++;
			Menu_CheckPassView(k);
			CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
		}
		else
		{
			CRITICAL_SECTION { timeOut = 0; }
			// проверка пароля
			bool fl1 = true;
			bool fl2 = true;
			for (uint8_t i=0; i<5; i++)
				{
					if (inPass[i] != (char)eeprom_read_byte((uint8_t *)&eePass[i])) fl1 = false;
					if (inPass[i] != (char)pgm_read_byte((uint8_t *)&ffPass[i])) fl2 = false;
				}
			if (fl1 || fl2)
			{
				curPosMenu = 0;
				SetMode(MENU_SELECT);
			}
			else
			{
				scr::Clear();
				uint8_t pos = scr::SetPosition(0, 0);
				scr::String_P(&pos, PSTR("Ошибка ввода") );
				pos = scr::SetPosition(0, 1);
				scr::String_P(&pos, PSTR("пароля") );
				CRITICAL_SECTION	{timeOut = 5000;}
				SetMode(TIMEOUT_TO_MAIN);
			}
			
		}
	}
	void Menu_CheckPassTo(unsigned char k)
	{
		CRITICAL_SECTION { timeOut = 0; }
		SetMode(MAIN);
	}
	// ==============================
	const char menuSelectTab_corLen[]  PROGMEM = "Коррекция длины";
	const char menuSelectTab_sq1[]     PROGMEM = "Дист. SQ1<->SQ8";
	const char menuSelectTab_sq2[]     PROGMEM = "Дист. SQ2<->SQ8";
	const char menuSelectTab_sq3[]     PROGMEM = "Дист. SQ3<->SQ8";
	const char menuSelectTab_sq4[]     PROGMEM = "Дист. SQ4<->SQ8";
	const char menuSelectTab_sq5[]     PROGMEM = "Дист. SQ5<->SQ8";
	const char menuSelectTab_sq6[]     PROGMEM = "Дист. SQ6<->SQ8";
	const char menuSelectTab_sq7[]     PROGMEM = "Дист. SQ7<->SQ8";
	const char menuSelectTab_offSq7[]  PROGMEM = "отк.SQ7 base   ";
	const char menuSelectTab_setPass[] PROGMEM = "Уст. пароль    ";
	const char *menuSelectTab[] = {
		menuSelectTab_corLen,
		menuSelectTab_sq1,
		menuSelectTab_sq2,
		menuSelectTab_sq3,
		menuSelectTab_sq4,
		menuSelectTab_sq5,
		menuSelectTab_sq6,
		menuSelectTab_sq7,
		menuSelectTab_offSq7,
		menuSelectTab_setPass
	};
	void Menu_Select(unsigned char k)
	{
		CRITICAL_SECTION { timeOut = 0; }
		key4->autoRepeat = false;
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P(&pos, PSTR("Меню-выбор:") );
		if (curPosMenu >= (sizeof(menuSelectTab) / sizeof(const char*)) )
		{
			curPosMenu = 0;
		}
		Menu_SelectView(k);
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Menu_SelectBack(unsigned char k)
	{
		CRITICAL_SECTION { timeOut = 0; }
		SetMode(MAIN);
	}
	void Menu_SelectView(unsigned char k)
	{
		// очистка строки
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < scr::Stolbcov(); i++) scr::PutChar(&pos, ' ');
		pos = scr::SetPosition(0, 1);
		scr::String_P(&pos, menuSelectTab[curPosMenu] );
	}
	void Menu_SelectMinus(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosMenu > 0)
		{
			curPosMenu--;
			Menu_SelectView(k);
		}
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Menu_SelectPlus(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosMenu < (sizeof(menuSelectTab)/sizeof(char *))-1 )
		{
			curPosMenu++;
			Menu_SelectView(k);
		}
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Menu_SelectVV(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
		if (curPosMenu ==  0) {  return; }
		if (curPosMenu ==  1) { curSensIdx = 7; curSensor = 1; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  2) { curSensIdx = 6; curSensor = 2; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  3) { curSensIdx = 5; curSensor = 3; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  4) { curSensIdx = 4; curSensor = 4; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  5) { curSensIdx = 3; curSensor = 5; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  6) { curSensIdx = 2; curSensor = 6; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  7) { curSensIdx = 1; curSensor = 7; SetMode(EDIT_SENSOR); return; }
		if (curPosMenu ==  8) { SetMode(OFF_SQ7_BASE); return; }
		if (curPosMenu ==  9) { SetMode(SET_PASS); return; }
	}
	void Menu_SelectMulti(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
		if (k == 6)
		{
			curPosMenu = 0;
			SetMode(COR_COR_SENS_MEN);
			return;
		}
	}
	void Menu_SelectTo(unsigned char k)
	{
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MAIN);
	}
	// ==============================
	void Edit_Sensor(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
		tmpDistance = eeprom_read_word(&ns_vg::eeDistance[curSensIdx]);
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P(&pos, PSTR("Настройка") );
		Edit_SensorView(k);
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Edit_SensorView(unsigned char k)
	{
		uint8_t pos = scr::SetPosition(0, 1);
		scr::String_P(&pos, PSTR("SQ") );
		scr::Digit(&pos, 1, curSensor);
		scr::String_P(&pos, PSTR(": дист.=") );
		scr::Digit(&pos, 5, tmpDistance);
	}
	void Edit_SensorBack(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
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
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Edit_SensorPlus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (tmpDistance < 15000)
		{
			tmpDistance++;
			Edit_SensorView(k);
		}
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Edit_SensorVV(unsigned char k)
	{
		key4->autoRepeat = false;
		eeprom_write_word(&ns_vg::eeDistance[curSensIdx], tmpDistance);
		uint8_t pos = scr::SetPosition(0, 1);
		scr::String_P(&pos, PSTR("** сохранено **") );
		CRITICAL_SECTION{ timeOut = 4000; }
		SetMode(TIMEOUT_TO_MNSEL);
	}
	void Edit_SensorTo(unsigned char k)
	{
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MAIN);
	}
	// ==============================
	void Timeout_To_Main(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MAIN);
	}
	void TimeoutToMainE(unsigned char k)
	{
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MAIN);
	}
	// ==============================
	// установка нового пароля
	void Set_Pass(unsigned char k)
	{
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P(&pos, PSTR(">новый пароль:") );
		for (uint8_t i=0; i<5; i++) inPass[i] = 0;
		curPosPass = 0;
		Set_PassView(k);
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Set_PassView(unsigned char k)
	{
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < 5;i++)
		{
			if (i < curPosPass)
			{
				scr::PutChar(&pos, '*');
				continue;
			}
			if (i == curPosPass)
			{
				scr::PutChar(&pos, '0' + inPass[i]);
				continue;
			}
			scr::PutChar(&pos, ' ');
		}
	}
	void Set_PassBack(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
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
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Set_PassPlus(unsigned char k)
	{
		key4->autoRepeat = true;
		if (inPass[curPosPass]<9)
		{
			inPass[curPosPass]++;
			Set_PassView(k);
		}
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Set_PassVV(unsigned char k)
	{
		key4->autoRepeat = false;
		if (curPosPass < 4)
		{
			curPosPass++;
			inPass[curPosPass] = 0;
			Menu_CheckPassView(k);
			CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
		}
		else
		{
			CRITICAL_SECTION { timeOut = 0; }
			for (uint8_t i=0; i<5; i++)
				eeprom_write_byte((uint8_t *)&eePass[i], inPass[i]);
			scr::Clear();
			uint8_t pos = scr::SetPosition(0, 0);
			scr::String_P(&pos, PSTR("Новый пароль") );
			pos = scr::SetPosition(0, 1);
			scr::String_P(&pos, PSTR("установлен") );
			CRITICAL_SECTION { timeOut = 10000; }
			SetMode(TIMEOUT_TO_MAIN);
		}
	}
	void Set_PassTo(unsigned char k)
	{
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MAIN);
	}
	// ==============================
	void TimeoutToMnSelBc(unsigned char k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MENU_SELECT);
	}
	void TimeoutToMnSel(unsigned char k)
	{
		CRITICAL_SECTION	{timeOut = 0;}
		SetMode(MENU_SELECT);
	}
	// ==============================
	const char menuSelectCor_str1[] PROGMEM = ">Коррекция SQ1.0";
	const char menuSelectCor_str2[] PROGMEM = ">Коррекция SQ2.0";
	const char menuSelectCor_str3[] PROGMEM = ">Коррекция SQ3.0";
	const char menuSelectCor_str4[] PROGMEM = ">Коррекция SQ4.0";
	const char menuSelectCor_str5[] PROGMEM = ">Коррекция SQ5.0";
	const char menuSelectCor_str6[] PROGMEM = ">Коррекция SQ6.0";
	const char menuSelectCor_str7[] PROGMEM = ">Коррекция SQ7.0";
	const char menuSelectCor_str8[] PROGMEM = ">Коррекция SQ8.0";
	const char menuSelectCor_str9[] PROGMEM = ">Коррекция SQ1.1";
	const char menuSelectCor_str10[] PROGMEM = ">Коррекция SQ2.1";
	const char menuSelectCor_str11[] PROGMEM = ">Коррекция SQ3.1";
	const char menuSelectCor_str12[] PROGMEM = ">Коррекция SQ4.1";
	const char menuSelectCor_str13[] PROGMEM = ">Коррекция SQ5.1";
	const char menuSelectCor_str14[] PROGMEM = ">Коррекция SQ6.1";
	const char menuSelectCor_str15[] PROGMEM = ">Коррекция SQ7.1";
	const char menuSelectCor_str16[] PROGMEM = ">Коррекция SQ8.1";
	const char *menuSelectCor[] = {
		menuSelectCor_str1,
		menuSelectCor_str2,
		menuSelectCor_str3,
		menuSelectCor_str4,
		menuSelectCor_str5,
		menuSelectCor_str6,
		menuSelectCor_str7,
		menuSelectCor_str8,
		menuSelectCor_str9,
		menuSelectCor_str10,
		menuSelectCor_str11,
		menuSelectCor_str12,
		menuSelectCor_str13,
		menuSelectCor_str14,
		menuSelectCor_str15,
		menuSelectCor_str16
	};
	int16_t cor_CorSensMenTmp;
	uint8_t cor_CorSensMenN;
	uint8_t cor_CorSensMenNx;
	//
	void Cor_CorSensMenView()
	{
		// очистка строки
		uint8_t pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < scr::Stolbcov(); i++) scr::PutChar(&pos, ' ');
		pos = scr::SetPosition(0, 1);
		scr::String_P(&pos, menuSelectCor[curPosMenu] );
	}
	void Cor_CorSensMen(uint8_t k)
	{
		CRITICAL_SECTION	{timeOut = 0;}
		key4->autoRepeat = false;
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P(&pos, PSTR("Меню-выбор:") );
		if (curPosMenu >= (sizeof(menuSelectCor) / sizeof(const char*)) )
		{
			curPosMenu = 0;
		}
		Cor_CorSensMenView();
	}
	void Cor_CorSensMenBc(uint8_t k)
	{
		key4->autoRepeat = false;
		SetMode(MAIN);
	}
	void Cor_CorSensMenMn(uint8_t k)
	{
		key4->autoRepeat = true;
		if (curPosMenu > 0) {
			curPosMenu--;
		}
		Cor_CorSensMenView();
	}
	void Cor_CorSensMenPl(uint8_t k)
	{
		key4->autoRepeat = true;
		if (curPosMenu < (sizeof(menuSelectCor)/sizeof(char *))-1 ) {
			curPosMenu++;
		}
		Cor_CorSensMenView();
	}
	void Cor_CorSensMenVv(uint8_t k)
	{
		key4->autoRepeat = false;
		cor_CorSensMenN = 9;
		cor_CorSensMenNx = 9;
		if (curPosMenu ==  0) {cor_CorSensMenN = 0; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  1) {cor_CorSensMenN = 1; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  2) {cor_CorSensMenN = 2; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  3) {cor_CorSensMenN = 3; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  4) {cor_CorSensMenN = 4; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  5) {cor_CorSensMenN = 5; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  6) {cor_CorSensMenN = 6; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  7) {cor_CorSensMenN = 7; cor_CorSensMenNx = 0;}
		if (curPosMenu ==  8) {cor_CorSensMenN = 0; cor_CorSensMenNx = 1;}
		if (curPosMenu ==  9) {cor_CorSensMenN = 1; cor_CorSensMenNx = 1;}
		if (curPosMenu == 10) {cor_CorSensMenN = 2; cor_CorSensMenNx = 1;}
		if (curPosMenu == 11) {cor_CorSensMenN = 3; cor_CorSensMenNx = 1;}
		if (curPosMenu == 12) {cor_CorSensMenN = 4; cor_CorSensMenNx = 1;}
		if (curPosMenu == 13) {cor_CorSensMenN = 5; cor_CorSensMenNx = 1;}
		if (curPosMenu == 14) {cor_CorSensMenN = 6; cor_CorSensMenNx = 1;}
		if (curPosMenu == 15) {cor_CorSensMenN = 7; cor_CorSensMenNx = 1;}
		SetMode(COR_COR_SENS);
	}
	// ==============================
	void Cor_CorSensView()
	{
		uint8_t pos = scr::SetPosition(8, 1);
		scr::DigitZ(&pos, 4, cor_CorSensMenTmp);
	}
	void Cor_CorSens(uint8_t k)
	{
		key4->autoRepeat = false;
		scr::Clear();
		scr::String_P(scr::SetPosition(2, 0), PSTR("Ред. коррекции") );
		uint8_t pos = scr::SetPosition(0, 1);
		scr::String_P(&pos, PSTR("SQ") );
		scr::DigitZ(&pos, 1, (cor_CorSensMenN + 1) );
		scr::String_P(&pos, PSTR(".") );
		scr::DigitZ(&pos, 1, (cor_CorSensMenNx) );
		scr::String_P(&pos, PSTR(" = ") );
		cor_CorSensMenTmp = (int16_t)eeprom_read_word((uint16_t *)&ns_vg::eeCorSensor[cor_CorSensMenNx][cor_CorSensMenN]);
		Cor_CorSensView();
	}
	void Cor_CorSensBc(uint8_t k)
	{
		key4->autoRepeat = false;
		SetMode(COR_COR_SENS_MEN);
	}
	void Cor_CorSensMn(uint8_t k)
	{
		key4->autoRepeat = true;
		if (cor_CorSensMenTmp > -50) cor_CorSensMenTmp--;
		Cor_CorSensView();
	}
	void Cor_CorSensPl(uint8_t k)
	{
		key4->autoRepeat = true;
		if (cor_CorSensMenTmp < +50) cor_CorSensMenTmp++;
		Cor_CorSensView();
	}
	void Cor_CorSensMt(uint8_t k)
	{
		key4->autoRepeat = false;
		if (k != 6) return;
		cor_CorSensMenTmp = 0;
		Cor_CorSensView();
	}
	void Cor_CorSensVv(uint8_t k)
	{
		key4->autoRepeat = false;
		eeprom_update_word((uint16_t *)&ns_vg::eeCorSensor[cor_CorSensMenNx][cor_CorSensMenN], (uint16_t)cor_CorSensMenTmp );
		SetMode(COR_COR_SENS_MEN);
	}
	// ==============================
	uint8_t offBaseSQ7_tmp;
	void Off_Sq7_BaseView()
	{
		uint8_t pos = scr::SetPosition(11, 1);
		if (offBaseSQ7_tmp)
		{
			scr::String_P(&pos, PSTR("on "));
		}
		else
		{
			scr::String_P(&pos, PSTR("off"));
		}
	}
	void Off_Sq7_Base(uint8_t k)
	{
		CRITICAL_SECTION { timeOut = 0; }
		key4->autoRepeat = false;
		offBaseSQ7_tmp = eeprom_read_byte(&ns_vg::eeBaseSQ7);
		scr::Clear();
		uint8_t pos = scr::SetPosition(0, 0);
		scr::String_P(&pos, PSTR("select base line"));
		pos = scr::SetPosition(0, 1);
		scr::String_P(&pos, PSTR("sensor SQ7 "));
		Off_Sq7_BaseView();
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Off_Sq7_BaseMn(uint8_t k)
	{
		key4->autoRepeat = false;
		offBaseSQ7_tmp = 0;
		Off_Sq7_BaseView();
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Off_Sq7_BasePl(uint8_t k)
	{
		key4->autoRepeat = false;
		offBaseSQ7_tmp = 1;
		Off_Sq7_BaseView();
		CRITICAL_SECTION { timeOut = TIMEOUT_STND; }
	}
	void Off_Sq7_BaseTo(uint8_t k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
		SetMode(MAIN);
	}
	void Off_Sq7_BaseBc(uint8_t k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
		SetMode(MAIN);
	}
	void Off_Sq7_BaseEn(uint8_t k)
	{
		key4->autoRepeat = false;
		CRITICAL_SECTION { timeOut = 0; }
		eeprom_update_byte(&ns_vg::eeBaseSQ7, offBaseSQ7_tmp);
		scr::String_P(scr::SetPosition(0, 1), PSTR("***сохранено*** "));
		CRITICAL_SECTION { timeOut = 5000; }
		SetMode(TIMEOUT_TO_MNSEL);
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
	// ============================================
	// 0 : no key (view)
	// 1-4 : main key
	// 5 : add key
	// 6 : timeout
	const TyFnMn MassFnMn[][8] PROGMEM =
	{//					view				key1				key2				key3				key4				key5			 setmode			 timeout          
		{	       Main_View,	       Main_menu,				Dupm,				Dupm,				Dupm,		 Main_ViewMl,       Main_SetMode,				Dupm },	// 0 main
		{ Menu_CheckPassView, Menu_CheckPassBack,Menu_CheckPassMinus, Menu_CheckPassPlus,	Menu_CheckPassVv,				Dupm,     Menu_CheckPass,	Menu_CheckPassTo },	// 1 menu check pass
		{				Dupm,	 Menu_SelectBack,	Menu_SelectMinus,	 Menu_SelectPlus,	   Menu_SelectVV,	Menu_SelectMulti,		 Menu_Select,	   Menu_SelectTo },	// 2 menu select
		{				Dupm,	 Edit_SensorBack,	Edit_SensorMinus,	 Edit_SensorPlus,	   Edit_SensorVV,				Dupm,		 Edit_Sensor,	   Edit_SensorTo }, // 3 edit sensor
		{				Dupm,		Main_SetMode,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,	  TimeoutToMainE }, // 4 timeout to main
		{				Dupm,		Set_PassBack,	   Set_PassMinus,		Set_PassPlus,		  Set_PassVV,				Dupm,			Set_Pass,		  Set_PassTo }, // 5 set password
		{				Dupm,	TimeoutToMnSelBc,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,	  TimeoutToMnSel }, // 6 TIMEOUT_TO_MNSEL
		{				Dupm,	Cor_CorSensMenBc,	Cor_CorSensMenMn,	Cor_CorSensMenPl,	Cor_CorSensMenVv,				Dupm,	  Cor_CorSensMen,				Dupm },	// 7 Cor_CorSensMen
		{				Dupm,	   Cor_CorSensBc,	   Cor_CorSensMn,	   Cor_CorSensPl,	   Cor_CorSensVv,	   Cor_CorSensMt,		 Cor_CorSens,				Dupm },	// 8 Cor_CorSens
		{				Dupm,	  Off_Sq7_BaseBc,	  Off_Sq7_BaseMn,	  Off_Sq7_BasePl,	  Off_Sq7_BaseEn,				Dupm,		Off_Sq7_Base,	  Off_Sq7_BaseTo }, // 9 OFF_SQ6_BASE
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm }
	};

}
