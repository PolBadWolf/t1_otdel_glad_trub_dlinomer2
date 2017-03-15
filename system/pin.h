/* 
* pin.h
*
* Created: 19.10.2016 11:49:01
* Author: GumerovMN
*/


#ifndef __PIN_H__
#define __PIN_H__

#include "path.h"

template <class T, class P>
class tc_pin
{
//variables
public:
	uint8_t tr;
	uint8_t trFr;
	uint8_t trSp;
protected:
private:
	volatile unsigned char* ddr;
	volatile unsigned char* port;
	volatile unsigned char* pin;
	unsigned mask;
	T iPorog;
	T iCount;
	uint8_t iFl;
	uint8_t flFr;
	uint8_t flSp;
	// ===
	P pFrMax;
	P pFr;
	P pSpMax;
	P pSp;
	uint8_t pFrFl;
	uint8_t pSpFl;
//functions
public:
	tc_pin(volatile unsigned char* ddr, volatile unsigned char* port, volatile unsigned char* pin, unsigned char bit, T iPorog, P pFrM, P pSpM)
	{
		this->ddr = ddr;
		this->port = port;
		this->pin = pin;
		this->mask = (1<<(bit & 0x07));
		this->iPorog = iPorog;
		*this->ddr &= ~mask;
		*this->port |= 1<<mask;
		__delay_ms(1);
		tr = iFl = (*this->pin & mask)?1:0;
		if (iFl)
			iCount = this->iPorog;
		else
			iCount = (T)0;
		// protect time flags
		trFr = trSp = flFr = flSp = 0;
		// protect time count
		this->pFr = this->pSp = 0;
		// protect time - set time
		pFrMax = pFrM;
		pSpMax = pSpM;
	}
	//
	void Interrupt()
	{
		if (*this->pin & mask)
		{
			if ( iCount <  iPorog) iCount++;
			if ((iCount >= iPorog) && !iFl)
			{
				tr = iFl = 1;	// integral
				flFr = 0;		// flag for protect time
				flSp = 1;
			}
		}
		else
		{
			if ( iCount >  (T)0) iCount--;
			if ((iCount == (T)0) && iFl)
			{
				tr = iFl = 0;	// integral
				flFr = 1;
				flSp = 0;		// flag for protect time
			}
		}
		// protect time
		// front
		if (pFr > 0)	pFr--;
		if (flFr)
		{
			flFr = 0;
			if (pFr == 0)	trFr = 1;
			pFr = pFrMax;
		}
		// spad
		if (pSp > 0)	pSp--;
		if (flSp)
		{
			flSp = 0;
			if (pSp == 0)	trSp = 1;
			pSp = pSpMax;
		}
	}
	uint8_t readSensor() { return this->iFl; }
protected:
private:
	tc_pin( const tc_pin &c );
	tc_pin& operator=( const tc_pin &c );

}; //pin

#endif //__PIN_H__
