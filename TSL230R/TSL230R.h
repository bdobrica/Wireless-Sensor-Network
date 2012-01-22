#ifndef TSL230R_h
#define TSL230R_h
#include "Arduino.h"

#define TSL230R_FP	3
#define TSL230R_S0	6
#define TSL230R_S1	7
#define TSL230R_S2	4
#define TSL230R_S3	5

#define TSL230R_IOFF	0
#define TSL230R_I1E0	1
#define TSL230R_I1E1	2
#define TSL230R_I1E2	3

#define TSL230R_S1E0	0
#define TSL230R_S2E0	1
#define TSL230R_S1E1	2
#define TSL230R_S1E2	3

class TSL230R {
  public:
    TSL230R (void);
	void begin (void);
    float get (void);
  private:
	void scale (unsigned int iso, unsigned int frq);
  };

#endif