/*
 * sine.h
 *
 *  Created on: Nov 1, 2020
 *      Author: monke
 */

#ifndef SRC_SINE_H_
#define SRC_SINE_H_
#include "FreqIO.h"

#define FREQ_SAMP			300
extern uint32_t wave[2*FREQ_SAMP];
extern int asin_lut[4096];

void gen_asin();

#endif /* SRC_SINE_H_ */
