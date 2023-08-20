/* ==================================================================================
 *
 * Converter for Linndrum, Drumulator, and similar 8-bit mu-law ADPCM audio samples
 *
 * Convert mu-law binary files and to mono 16-bit .wav files at 22050hz
 * Default sample rate set tp 22050hz to allow for original variable sample rate tuning range.
 *
 * Convert mono 16-bit .wav files to mu-law encoded binary files.
 *
 * 08/19/23 by Finley Baker (_astriiddev_)
 *
 * ==================================================================================*/



#include <stdio.h>
#include <string.h>

#include "readwrite.h"

static void 	usage();

static char 	*inName,
				*outName;

int main(int argc, char* argv[])
{
	if(argv[1] == NULL)
	{
		usage();
	}
	else
	{
		int isWav = 0;
		
		inName = argv[1];

		/* Determine if input is wav PCM or binary ADPCM*/
		char* inWavExt = strstr(inName, ".wav");
		char* inMuExt = strstr(inName, ".bin");

		if(inWavExt != NULL)
			isWav = 1;
		else if (inMuExt == NULL)
		{
			printf("Please select a valid .wav or .bin file.\n");
			return 0;
		}

		if (readFile(inName, isWav) == 0)
			printf("Failure to read file!\n");
		else
		{
			
			printf("Converting \"%s\"....\n", inName);
			
			char *outName;
	
			if(argv[2] == NULL)
			{
				/* adds output file name if only input name is entered*/
			
				outName = argv[1];
			
				char* outMuExt  = strstr(outName, ".bin");
				char* outWavExt = strstr(outName, ".wav");
					
				if (outMuExt != NULL)
					strcpy(outMuExt, ".wav");

				if (outWavExt != NULL)
					strcpy(outWavExt, ".bin");
			}
			else
				outName = argv[2];
			
			/* if output file name is entered with no extension,
			 * add extension based on input */

			char* outMuExt  = strstr(outName, ".bin");
			char* outWavExt = strstr(outName, ".wav");
			
			if(isWav == 1 && outMuExt == NULL)
				sprintf(outName, "%s%s", outName, ".bin");

			if(isWav == 0 && outWavExt == NULL)
				sprintf(outName, "%s%s", outName, ".wav");

			writeFile(outName, isWav);

			if (freeBuffers() == 0)
				printf("Possible memory corruption!\n");
		}
	}
	
	return 0;
}

static void usage()
{
	printf("Usage: mu2wav infile.bin [outfile.wav]\n");
}
