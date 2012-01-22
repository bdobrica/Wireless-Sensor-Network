#include "Arduino.h"
#include "SHT15.h"

SHT15::SHT15 (void) {
	}
	
void SHT15::begin (void) {
	pinMode (SHT15_DAT, OUTPUT);
	pinMode (SHT15_CLK, OUTPUT);
	txreset (1);
	}


float SHT15::get (unsigned short int mode) {
	unsigned short int error;

	unsigned short int hraw, traw;
	float h, hq, t, d, ld;

	if ((error = getraw (&traw, 0)) != 0) return (float) error;
	if ((error = getraw (&hraw, 1)) != 0) return (float) error;

	h = (float) hraw;

	t = -40.1 + 0.01 * ((float) traw);	// 14 bit, 'C
	if (mode == 0) return t;

	hq = -2.0468 + 0.0367 * h - 1.5955E-6 * h * h;
	h = (t - 25) * (0.01 + 0.00008 * h) + hq;
	h = h < 0.1 ? 0.1 : (h > 100 ? 100 : h);
	if (mode == 1) return h;

	ld = (log10(h) - 2) / 0.4343 + (17.62 * t) / (243.12 + t);
	d = 243.12 * ld / (17.62 - ld);
	return d;
	}

// mode: 0 = temperature; 1 = humidity
unsigned short int SHT15::getraw (unsigned short int * p, unsigned short int mode) {
		unsigned short int i = 0;
		txbegin ();
		* p = 0;
		mode = mode ? 0x05 : 0x03;
		if (txdata(mode)) return 1;	// write error
		
		while (i < 240) {
			delay (3);
			if (digitalRead (SHT15_DAT) == 0) {
				i = 0;
				break;
				}
			}
			i++;
			
		if (i) return 3; // timeout
		i = rxdata (1);
		*p = (i << 8) | rxdata (0);
		return 0;
		}

unsigned short int SHT15::txdata (unsigned short int data) {
	unsigned short int i = 0x80;
	unsigned short int error = 0;
	pinMode (SHT15_DAT, OUTPUT);
	while (i) {
		if (i & data)
			digitalWrite (SHT15_DAT, HIGH);
		else
			digitalWrite (SHT15_DAT, LOW);
		digitalWrite (SHT15_CLK, HIGH);
		delayMicroseconds (3);
		digitalWrite (SHT15_CLK, LOW);
		delayMicroseconds (1);
		i >>= 1;
		}
	pinMode (SHT15_DAT, INPUT);
	digitalWrite (SHT15_CLK, HIGH);
	delayMicroseconds (3);
	if (digitalRead (SHT15_DAT)) error = 1;
	delayMicroseconds (1);
	digitalWrite (SHT15_CLK, LOW);
	
	return error;
	}

unsigned short int SHT15::rxdata (unsigned short int ack) {
	unsigned short int i = 0x80;
	unsigned short int val = 0;
	
	pinMode (SHT15_DAT, INPUT);
	
	while (i) {
		digitalWrite (SHT15_CLK, HIGH);
		delayMicroseconds (1);
		if (digitalRead(SHT15_DAT)) val = val | i;
		digitalWrite (SHT15_CLK, LOW);
		delayMicroseconds (1);
		i >>= 1;
		}
		
	pinMode (SHT15_DAT, OUTPUT);
	if (ack)
		digitalWrite (SHT15_DAT, LOW);
	else
		digitalWrite (SHT15_DAT, HIGH);
	digitalWrite (SHT15_CLK, HIGH);
	delayMicroseconds (3);
	digitalWrite (SHT15_CLK, LOW);
	delayMicroseconds (1);
	pinMode (SHT15_DAT, INPUT);
	
	return val;
	}
	
void SHT15::txbegin (void) {      
	digitalWrite (SHT15_CLK, LOW);
	pinMode (SHT15_DAT, OUTPUT); 
	digitalWrite (SHT15_DAT, HIGH);
	delayMicroseconds (1);
	digitalWrite(SHT15_CLK, HIGH);
	delayMicroseconds (1);
	digitalWrite(SHT15_DAT, LOW);
	delayMicroseconds (1);
	digitalWrite(SHT15_CLK, LOW);
	delayMicroseconds (3);
	digitalWrite(SHT15_CLK, HIGH);
	delayMicroseconds (1);
	digitalWrite(SHT15_DAT, HIGH);
	delayMicroseconds (1);
	digitalWrite(SHT15_CLK, LOW);
	delayMicroseconds (1);
	pinMode(SHT15_DAT, INPUT);
	}
	
unsigned short int SHT15::txreset (unsigned short int mode) {
	unsigned short int i;
	
	digitalWrite (SHT15_CLK, LOW);
	pinMode (SHT15_DAT, OUTPUT);
	digitalWrite (SHT15_DAT, HIGH);
	
	for (i = 0; i < 9; i++) {
		digitalWrite (SHT15_CLK, HIGH);
		delayMicroseconds (3);
		digitalWrite (SHT15_CLK, LOW);
		delayMicroseconds (3);
		}
		
	txbegin ();
	if (mode) return txdata (0x1e);
	return 0;
	}