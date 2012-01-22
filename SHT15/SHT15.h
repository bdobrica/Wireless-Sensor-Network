#ifndef SHT15_h
#define SHT15_h
#include "Arduino.h"

#define SHT15_DAT		A4
#define SHT15_CLK		A5

class SHT15 {
	public:
		SHT15 (void);
		void begin (void);
		float get (unsigned short int mode);
	private:
		unsigned short int getraw (unsigned short int * p, unsigned short int mode);
		unsigned short int txdata (unsigned short int data);
		unsigned short int rxdata (unsigned short int ack);
		void txbegin (void);
		unsigned short int txreset (unsigned short int mode);
	};

#endif