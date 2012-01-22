#include "Arduino.h"
#include "TSL230R.h"

TSL230R::TSL230R (void) {
	}
	
void TSL230R::begin (void) {
	pinMode (TSL230R_FP, INPUT);
	pinMode (TSL230R_S0, OUTPUT);
	pinMode (TSL230R_S1, OUTPUT);
	pinMode (TSL230R_S2, OUTPUT);
	pinMode (TSL230R_S3, OUTPUT);
	
	scale (TSL230R_I1E0, TSL230R_S1E1);
	}
	
void TSL230R::scale (unsigned int iso, unsigned int frq) {
	digitalWrite (TSL230R_S0, (iso & 1));
	digitalWrite (TSL230R_S1, (iso & 2) >> 1);
	digitalWrite (TSL230R_S2, (frq & 1));
	digitalWrite (TSL230R_S3, (frq & 2) >> 1);
	}
	
float TSL230R::get () {
	unsigned c = 0;
	unsigned long p = 0, q = 0;
	for ( ; c < 64; c++) {
		q = pulseIn (TSL230R_FP, HIGH, 50000);
		p += q ? q : 50000;
		}
	return 500000.0 / ((float) (p >> 6));
	}