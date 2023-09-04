#ifndef  _READWRITE_H
#define  _READWRITE_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

extern uint8_t* muBuffer;
extern int16_t* wavBuffer;

extern uint32_t muLength;
extern uint32_t wavLength,
	 			sampleRate;

/* byte to 16-bit word little endian conversion */
#define littleEndian16(byte1, byte2) ((uint8_t)byte2 << 8 | (uint8_t)byte1 << 0)

/* byte to 32-bit word little endian conversion */
#define littleEndian32(byte1, byte2, byte3, byte4) \
  ((uint8_t)byte4 << 24 | (uint8_t)byte3 << 16 | (uint8_t)byte2 << 8 | (uint8_t)byte1 << 0)

void writeFile(const char* fileName, int isMu );
int  readFile (const char* filePath, int isWav);

int  freeBuffers();

inline uint32_t* getSampleRate() {return &sampleRate; }
inline void		 setSampleRate(uint32_t newRate) { sampleRate = newRate; }

inline uint32_t* getMuLength() { return &muLength; };
inline uint8_t*  getMuSamp(uint32_t pos) { return &muBuffer[pos]; }
inline uint8_t** getMuBuffer() { return &muBuffer; }

inline void		 newMuBuffer(uint32_t length)
{
    if (muBuffer != NULL)
    {
	free(muBuffer);
	muBuffer = NULL;
    }
    muLength = length;
    muBuffer = malloc(muLength * sizeof *muBuffer);
}

inline void		 setMuSamp(uint32_t pos, uint8_t samp)
{
    if(muBuffer == NULL)
		return;

    muBuffer[pos] = samp;
}


inline uint32_t* getWavLength() { return &wavLength; };
inline int16_t*  getWavSamp(uint32_t pos) { return &wavBuffer[pos]; }
inline int16_t** getWavBuffer() { return &wavBuffer; }

inline void 	 newWavBuffer(uint32_t length)
{
    if (wavBuffer != NULL)
    {
		free(wavBuffer);
		wavBuffer = NULL;
    }
    wavLength = length;
    wavBuffer = malloc(wavLength * sizeof *wavBuffer);
}

inline void		 setWavSamp(uint32_t pos, int16_t samp)
{
    if(wavBuffer == NULL)
		return;

    wavBuffer[pos] = samp;
}

#endif
