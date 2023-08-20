#include <stdlib.h>
#include <string.h>

#include "readwrite.h"
#include "compand.h"


static char 	*fileBuffer,
				*muBuffer;
static int16_t	*wavBuffer;

static uint32_t fileLength,
				wavLength;

static void writeHeader(FILE* wavFile);
static int  wavParse();

int freeBuffers()
{
	/* deallocating memory buffers */

	if(fileBuffer != NULL)
	{
		free(fileBuffer);
		fileBuffer = NULL;
	}

	if(muBuffer != NULL)
	{
		free(muBuffer);
		muBuffer = NULL;
	}

	if(wavBuffer != NULL)
	{
		free(wavBuffer);
		wavBuffer = NULL;
	}

	if ((fileBuffer = muBuffer) != NULL || wavBuffer != NULL)
	{
		printf("Could not free memory!\n");
		return 0;
	}

	return 1;
}

int readFile(const char* filePath, int isWav)
{	
	if(freeBuffers() == 0)
		return 0;
	
	/* read input file data */

	FILE* inFile = fopen(filePath, "rb");

	if(inFile == NULL)
	{
		printf("\"%s\" not found!\n", filePath);
		return 0;
	}

	fseek(inFile, 0, SEEK_END);
	fileLength = ftell(inFile);
	rewind(inFile);

	fileBuffer = (char*)malloc(fileLength * sizeof(char));
	fread(fileBuffer, 1, fileLength, inFile);
	fclose(inFile);
	
	if(fileBuffer == NULL)
	{
		printf("Could not read file!\n");
		return 0;
	}

	if(isWav)
	{
		/* parse PCM data from wav file */

		if(wavParse() == 0)
		{
			free(fileBuffer);
			fileBuffer = NULL;
			printf("Could not read WAV!\n");
			return 0;
		}
	}
	else
	{
		/* mu-law files have no header and are 8-bit, no parsing needed */

		muBuffer = (char*)malloc(fileLength);
		memcpy(muBuffer, fileBuffer, fileLength);

		if (muBuffer == NULL)
		{
			free(fileBuffer);
			fileBuffer = NULL;
			printf("Mu buffer could not be copied!\n");
			return 0;
		}
	}

	free(fileBuffer);
	fileBuffer = NULL;

	return 1;
}

static int fileExists()
{
	printf("File already exists! Overwrite file? (y/n) ");

	char proceed = getchar();
	char ignore  = 0;

	/* ignores all other input other than y/Y and n/N
	 * -- necessary for recursion as to ignore '\n' -- */

	if (proceed == '\n')
		ignore = proceed;

	while (ignore != '\n')
		ignore = getchar();
	
	/* cheap and cheerful unicode uppercase :) */

	proceed &= 0xDF;
		
	if (proceed == 'Y')
		return 1;
	else if (proceed == 'N') 
		return 0;
	/* repeat question for input other than y/Y or n/N */
	else
		return fileExists();
}

void writeFile(const char* fileName, int isMu)
{
	/* tries to read file to check if it exists */

	FILE* existingFile = fopen(fileName, "r");

	/* if exists, ask for overwrite permission */

	if(existingFile != NULL)
	{
		if(fileExists() == 0)
		{
			fclose(existingFile);
			printf("File not saved!\n");
			return;
		}
		
		fclose(existingFile);
	}
	
	FILE* outFile = fopen(fileName, "wb");

	if(outFile == NULL)
	{
		printf("Failure to write file!\n");
		fclose(outFile);
		return;
	}
	
	if(isMu == 0)
	{
		if(muBuffer != NULL)
		{
			writeHeader(outFile);
			
			/* 8-bit mulaw decoding to 16-bit WAV file */

			for (int i = 0; i < fileLength; i++)
			{
				int16_t tempDecode = decodeSample(muBuffer[i]);

				fwrite(&tempDecode, 2, 1, outFile);
			}
	
			free(muBuffer);
			muBuffer = NULL;

		}
		else
		{
			printf("Failure to decode MU to PCM!\n");
			fclose(outFile);
			return;
		}
	}
	else
	{
		if(wavBuffer != NULL)
		{
			/* 16-bit PCM to 8-bit mu-law encoding */

			for (int i = 0; i < wavLength; i++)
			{
				int8_t tempEncode = encodeSample(wavBuffer[i]);

				fwrite(&tempEncode, 1, 1, outFile);
			}

			free(wavBuffer);
			wavBuffer = NULL;
		}
		else
		{
			printf("Failure to encode PCM to MU!\n");
			fclose(outFile);
			return;
		}
	}

	printf("\"%s\" converted successfully!\n", fileName); 
	
	fclose(outFile);
}

static int wavParse()
{

	int	d = 0;

	/* find 'data' subchunk; usually at $0x2C,
	 * but safer to check for exact location */

	for (int i = 0; i < fileLength; i++)
	{
		static int dataChunkID = 0;

		if (fileBuffer[i + 0] != 'd')
			continue;

		if (fileBuffer[i + 1] != 'a')
			continue;
		
		if (fileBuffer[i + 2] != 't')
			continue;
			
		if (fileBuffer[i + 3] != 'a')
			continue;
		else		
			dataChunkID = 1;
				
		if (dataChunkID)
		{
			/* actual PCM data 8 bytes after data subchunk ID address;
			 * data subchunk ID is 4 bytes, following 4 bytes are PCM data size */
			d = i + 8;
			break;
		}
	}

	if(wavBuffer != NULL)
	{
		free(wavBuffer);
		wavBuffer = NULL;
	}
	
	/* checks if WAV is 16 bit and mono */

	int16_t numChannels = littleEndian16(fileBuffer[22], fileBuffer[23]);

	int16_t bitDepth	= littleEndian16(fileBuffer[34], fileBuffer[35]);

	if (numChannels != 1)
	{
		printf("WAV is not mono! Cannot convert file!\n");
		return 0;
	}

	if(bitDepth != 16)
	{
		printf("WAV is not 16-bit! Cannot convert file!\n");
		return 0;
	}


	if (d != 0)
	{
		/* PCM data length in bytes, determined by 'data' subchunk */
		wavLength = littleEndian32(fileBuffer[d - 4], fileBuffer[d - 3], 
								   fileBuffer[d - 2], fileBuffer[d - 1]);
		
		/* 16-bit WAV is 2 bytes per sample, 
		 * so actual sample length is half of length read from 'data' subchunk */
		wavLength /= 2;

		wavBuffer = (int16_t*)malloc(wavLength * sizeof(int16_t));

		int i = 0;
		if(wavBuffer != NULL)
		{
			for(; d < fileLength - 1; d += 2)
			{
				if (d + 1 >= fileLength)
					break;

				if(i < wavLength)
					wavBuffer[i] = littleEndian16(fileBuffer[d + 0], 
												  fileBuffer[d + 1]);
				else
					break;
			
				i++;
			}
		}
		
		if(wavBuffer == NULL)
			return 0;
		else
			return 1;
	}
	else
	{
		printf("data chunk not found!\n");
		return 0;
	}
}

static void writeHeader(FILE* wavFile)
{
	const static int 	sampleRate 		= 22050,
						numOfChannels 	= 1,
						bitDepth		= 16;

	/* RIFF Wave format header for writing WAV files */
	uint8_t chunkID_RIFF[4] = { 'R', 'I', 'F', 'F' };
	fwrite(&chunkID_RIFF, 4, 1, wavFile);
	
	uint32_t chunkSize_RIFF = ((fileLength * 2) + 36);
	fwrite(&chunkSize_RIFF, 4, 1, wavFile);

	uint8_t RIFF_Type[4] = { 'W', 'A', 'V', 'E' };
	fwrite(RIFF_Type, 4, 1, wavFile);

	uint8_t subchunkID_fmt_[4] = { 'f', 'm', 't', ' ' };
	fwrite(subchunkID_fmt_, 4, 1, wavFile);

	uint32_t subchunkSize_fmt_ = bitDepth;
	fwrite(&subchunkSize_fmt_, 4, 1, wavFile);

	uint16_t compressionType = 1;
	fwrite(&compressionType, 2, 1, wavFile);

	uint16_t NumOfChan = numOfChannels;
	fwrite(&NumOfChan, 2, 1, wavFile);

	uint32_t SamplesPerSec = sampleRate;
	fwrite(&SamplesPerSec, 4, 1, wavFile);

	uint32_t bytesPerSec = (sampleRate * numOfChannels * bitDepth) / 8;
	fwrite(&bytesPerSec, 4, 1, wavFile);

	uint16_t blockAlign = (numOfChannels * bitDepth) / 8;
	fwrite(&blockAlign, 2, 1, wavFile);

	uint16_t bitsPerSample = bitDepth;
	fwrite(&bitsPerSample, 2, 1, wavFile);

	uint8_t subchunkID_data[4] = { 'd', 'a', 't', 'a' };
	fwrite(subchunkID_data, 4, 1, wavFile);

	uint32_t subchunkSize_data = (fileLength * 2);
	fwrite(&subchunkSize_data, 4, 1, wavFile);
}
