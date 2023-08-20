#ifndef	 _COMPAND_H
#define  _COMPAND_H

#include <stdio.h>
#include <stdint.h>

int16_t decodeSample(const uint8_t sample);
int8_t  encodeSample(const int16_t pcmData);

#endif
