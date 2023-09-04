#include <math.h>
#include <stdlib.h>

#include "compand.h"

/* ==================================================================================
 *
 * mu-law ADPCM encoding and decoding as described in
 *
 * https://en.wikipedia.org/wiki/%CE%9C-law_algorithm
 *
 * and
 *
 * https://electricdruid.net/wp-content/uploads/2018/06/AM6070-uLaw-DAC.pdf
 *
 * and configured for 8-bit ADPCM to 16-bit PCM decoding/encoding
 *
 * ==================================================================================*/

int16_t decodeSample(const uint8_t sample)
{
	double tempDecode = 0.0;

	/* ==================================================================================
	 *
	 * 8-bit mu-law has 127 values for both positive and negative integers
	 * 
	 * from 0x01 to 0x7F for negative numbers (from nearest of zero to max, respectively)
	 * 
	 * from 0x81 to 0xFF for positive numbers (from nearest of zero to max, respectively)
	 * 
	 * with 0x00 and 0x80 both representing 0x00 equally 
	 *
	 * ==================================================================================*/

	if (sample & 0x80) 
		tempDecode = 1.0;
	else
		tempDecode = -1.0;

	tempDecode *= pow(256, (double)(sample & 0x7F) / 127) - 1;
	tempDecode /= 255;
	
	return (int16_t)round(tempDecode * 32767);
}

int8_t encodeSample(const int16_t pcmData)
{
	double encoding = 255.0 * (double)abs(pcmData);
	encoding /= 32767;
	encoding = log(1.0 + encoding);
	
	/* simplification of 0x7F / ln(1 + 0xFF) to save on FPU math
	 *
	 * aka magic number :) */
	encoding *= 22.91;

	uint8_t encodedSample = (uint8_t)round(encoding);

	if (pcmData > 0)
		encodedSample ^= 0x80;

	return encodedSample;
}
