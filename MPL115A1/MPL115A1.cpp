#include "Arduino.h"
#include "MPL115A1.h"
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

MPL115A1::MPL115A1 (void) {	
	}
	
void MPL115A1::begin (void) {
	byte discart;
	pinMode (MPL115A1_SDN, OUTPUT);
	digitalWrite (MPL115A1_SDN, HIGH);

	pinMode (MPL115A1_CS, OUTPUT);
	digitalWrite (MPL115A1_CS, HIGH);
	
	pinMode (MPL115A1_SCK, OUTPUT);
	digitalWrite (MPL115A1_SCK, LOW);
	
	pinMode (MPL115A1_MOSI, OUTPUT);
	digitalWrite (MPL115A1_MOSI, LOW);
	
	pinMode (MPL115A1_MISO, INPUT);

	pinMode (MPL115A1_SS, OUTPUT);
	digitalWrite (MPL115A1_SS, HIGH);

	SPCR = (1<<SPE)|(1<<MSTR);
	discart = SPSR;
	discart = SPDR;
	delay (10);
	}
	
float MPL115A1::get (void) {
	signed char sia0MSB, sia0LSB;
	signed char sib1MSB, sib1LSB;
	signed char sib2MSB, sib2LSB;
	signed char sic12MSB, sic12LSB;
	signed char sic11MSB, sic11LSB;
	signed char sic22MSB, sic22LSB;
	signed int sia0, sib1, sib2, sic12, sic11, sic22, siPcomp;
	float decPcomp;
	signed long lt1, lt2, lt3, si_c11x1, si_a11, si_c12x2;
	signed long si_a1, si_c22x2, si_a2, si_a1x1, si_y1, si_a2x2;
	unsigned int uiPadc, uiTadc;
	unsigned char uiPH, uiPL, uiTH, uiTL;

	write(0x24, 0x00);	// Start Both Conversions
	// write(0x20, 0x00);	// Start Pressure Conversion
	// write(0x22, 0x00);	// Start temperature conversion
	delay(10);	// Typical wait time is 3ms
	
	// Read pressure
	uiPH = read(MPL115A1_PRESH);
	uiPL = read(MPL115A1_PRESL);
	uiTH = read(MPL115A1_TEMPH);
	uiTL = read(MPL115A1_TEMPL);
	
	uiPadc = (unsigned int) uiPH << 8;
	uiPadc += (unsigned int) uiPL & 0x00FF;
	uiTadc = (unsigned int) uiTH << 8;
	uiTadc += (unsigned int) uiTL & 0x00FF;
	
	// Placing Coefficients into 16-bit Variables
	// a0
	sia0MSB = read(MPL115A1_A0MSB);
	sia0LSB = read(MPL115A1_A0LSB);
	sia0 = (signed int) sia0MSB << 8;
	sia0 += (signed int) sia0LSB & 0x00FF;
	
	// b1
	sib1MSB = read(MPL115A1_B1MSB);
	sib1LSB = read(MPL115A1_B1LSB);
	sib1 = (signed int) sib1MSB << 8;
	sib1 += (signed int) sib1LSB & 0x00FF;
	
	// b2
	sib2MSB = read(MPL115A1_B2MSB);
	sib2LSB = read(MPL115A1_B2LSB);
	sib2 = (signed int) sib2MSB << 8;
	sib2 += (signed int) sib2LSB & 0x00FF;
	
	// c12
	sic12MSB = read(MPL115A1_C12MSB);
	sic12LSB = read(MPL115A1_C12LSB);
	sic12 = (signed int) sic12MSB << 8;
	sic12 += (signed int) sic12LSB & 0x00FF;
	
	// c11
	sic11MSB = read(MPL115A1_C11MSB);
	sic11LSB = read(MPL115A1_C11LSB);
	sic11 = (signed int) sic11MSB << 8;
	sic11 += (signed int) sic11LSB & 0x00FF;
	
	// c22
	sic22MSB = read(MPL115A1_C22MSB);
	sic22LSB = read(MPL115A1_C22LSB);
	sic22 = (signed int) sic22MSB << 8;
	sic22 += (signed int) sic22LSB & 0x00FF;
	
	// Coefficient 9 equation compensation
	uiPadc = uiPadc >> 6;
	uiTadc = uiTadc >> 6;
	
	// Step 1 c11x1 = c11 * Padc
	lt1 = (signed long) sic11;
	lt2 = (signed long) uiPadc;
	lt3 = lt1*lt2;
	si_c11x1 = (signed long) lt3;
	
	// Step 2 a11 = b1 + c11x1
	lt1 = ((signed long)sib1)<<14;
	lt2 = (signed long) si_c11x1;
	lt3 = lt1 + lt2;
	si_a11 = (signed long)(lt3>>14);
	
	// Step 3 c12x2 = c12 * Tadc
	lt1 = (signed long) sic12;
	lt2 = (signed long) uiTadc;
	lt3 = lt1*lt2;
	si_c12x2 = (signed long)lt3;
	
	// Step 4 a1 = a11 + c12x2
	lt1 = ((signed long)si_a11<<11);
	lt2 = (signed long)si_c12x2;
	lt3 = lt1 + lt2;
	si_a1 = (signed long) lt3>>11;
	
	// Step 5 c22x2 = c22*Tadc
	lt1 = (signed long)sic22;
	lt2 = (signed long)uiTadc;
	lt3 = lt1 * lt2;
	si_c22x2 = (signed long)(lt3);
	
	// Step 6 a2 = b2 + c22x2
	lt1 = ((signed long)sib2<<15);
	lt2 = ((signed long)si_c22x2>1);
	lt3 = lt1+lt2;
	si_a2 = ((signed long)lt3>>16);
	
	// Step 7 a1x1 = a1 * Padc
	lt1 = (signed long)si_a1;
	lt2 = (signed long)uiPadc;
	lt3 = lt1*lt2;
	si_a1x1 = (signed long)(lt3);
	
	// Step 8 y1 = a0 + a1x1
	lt1 = ((signed long)sia0<<10);
	lt2 = (signed long)si_a1x1;
	lt3 = lt1+lt2;
	si_y1 = ((signed long)lt3>>10);
	
	// Step 9 a2x2 = a2 * Tadc
	lt1 = (signed long)si_a2;
	lt2 = (signed long)uiTadc;
	lt3 = lt1*lt2;
	si_a2x2 = (signed long)(lt3);
	
	// Step 10 pComp = y1 + a2x2
	lt1 = ((signed long)si_y1<<10);
	lt2 = (signed long)si_a2x2;
	lt3 = lt1+lt2;
	
	// Fixed point result with rounding
	//siPcomp = ((signed int)lt3>>13);
	siPcomp = lt3/8192;
	
	// decPcomp is defined as a floating point number
	// Conversion to decimal value from 1023 ADC count value
	// ADC counts are 0 to 1023, pressure is 50 to 115kPa respectively
	decPcomp = ((65.0/1023.0)*siPcomp)+50;
	
	return decPcomp;
	}
	
char MPL115A1::read (unsigned short int address) {
	char byte;

	address |= 0x80;

	digitalWrite (MPL115A1_CS, LOW);
	txdata(address);
	byte = rxdata();
	digitalWrite (MPL115A1_CS, HIGH);

	return byte;
	}

void MPL115A1::write (unsigned short int address, char data) {
	address &= 0x7F;
	//Serial.print ("write @");
	//Serial.print (address, HEX);
	//Serial.print (" :");
	//Serial.println (data, HEX);
	digitalWrite (MPL115A1_CS, LOW);
	delay(1);
	txdata(address);
	delay(1);
	txdata(data);
	delay(1);
	digitalWrite (MPL115A1_CS, HIGH);
	}
	
char MPL115A1::rxdata (void) {
	//Serial.print ("rxdata: ");
	SPDR = 0x55;
	while (!(SPSR & (1<<SPIF)));
	//Serial.println (SPDR, HEX);
	return SPDR;
	}
	
void MPL115A1::txdata (char data) {
	SPDR = data;
	//Serial.print ("txdata: ");
	//Serial.println (data, HEX);
	while (!(SPSR & (1<<SPIF)));
	}