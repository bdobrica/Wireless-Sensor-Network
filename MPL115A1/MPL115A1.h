#ifndef MPL115A1_h
#define MPL115A1_h
#include "Arduino.h"

#define MPL115A1_PRESH	0x00	// 80
#define	MPL115A1_PRESL	0x02	// 82
#define	MPL115A1_TEMPH	0x04	// 84
#define	MPL115A1_TEMPL	0x06	// 86

#define MPL115A1_A0MSB	0x08	// 88
#define MPL115A1_A0LSB	0x0A	// 8A
#define MPL115A1_B1MSB	0x0C	// 8C
#define MPL115A1_B1LSB	0x0E	// 8E
#define	MPL115A1_B2MSB	0x10	// 90
#define MPL115A1_B2LSB	0x12	// 92
#define MPL115A1_C12MSB	0x14	// 94
#define	MPL115A1_C12LSB	0x16	// 96
#define	MPL115A1_C11MSB	0x18	// 98
#define MPL115A1_C11LSB	0x1A	// 9A
#define MPL115A1_C22MSB	0x1C	// 9C
#define MPL115A1_C22LSB	0x1E	// 9E

#define MPL115A1_SDN		8
#define MPL115A1_CS			9
#define MPL115A1_SS			10
#define MPL115A1_MOSI		11
#define MPL115A1_MISO		12
#define MPL115A1_SCK		13

class MPL115A1 {
	public:
		MPL115A1 (void);
		void begin (void);
		float get (void);
		
	private:
		char read (unsigned short int address);
		void write (unsigned short int address, char data);
		char rxdata (void);
		void txdata(char data);
	};

#endif MPL115A1_h