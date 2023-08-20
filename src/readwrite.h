#ifndef  _READWRITE_H
#define  _READWRITE_H

#include <stdio.h>
#include <stdint.h>

/* byte to 16-bit word little endian conversion */
#define littleEndian16(byte1, byte2) ((uint8_t)byte2 << 8 | (uint8_t)byte1 << 0)

/* byte to 32-bit word little endian conversion */
#define littleEndian32(byte1, byte2, byte3, byte4) \
  ((uint8_t)byte4 << 24 | (uint8_t)byte3 << 16 | (uint8_t)byte2 << 8 | (uint8_t)byte1 << 0)

void writeFile(const char* fileName, int isMu );
int  readFile (const char* filePath, int isWav);
int  freeBuffers();

#endif
