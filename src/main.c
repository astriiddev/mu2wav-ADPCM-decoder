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

#include "readwrite.h"
#include "editor.h"

static void 	usage();

static char 	*inName,
		*outName;

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		usage();
	}
	else
	{
		
		inName = argv[1];

		int end = argc - 1;
		
		int inputIsWav = 0;

		/* Determine if input is wav PCM or binary ADPCM*/
		char* inWavExt = strstr(inName, ".wav");
		char* inMuExt = strstr(inName, ".bin");

		if(inWavExt != NULL)
			inputIsWav = 1;
		else if (inMuExt == NULL)
		{
			printf("Please select a valid .wav or .bin file.\n");
			return 0;
		}

		if (readFile(inName, inputIsWav) == 0)
			printf("Failure to read file!\n");
		else
		{
			if (argc >= 4 && argv[2][0] == '-')
			{
				sampleEdit(inputIsWav);

				if (argv[2][1] == 's')
					resampleAudio(atoi(argv[3]));

				fileEditor();
			}

			printf("Converting \"%s\"....\n", inName);
			
			char *outName;
	
			if(argc <= 2 || argv[end][0] == '-' || argv[end - 1][0] == '-')
			{
				/* adds output file name if only input name is entered */
			
				outName = argv[1];
			
				char* outMuExt  = strstr(outName, ".bin");
				char* outWavExt = strstr(outName, ".wav");
					
				if (outMuExt != NULL)
					strcpy(outMuExt, ".wav");

				if (outWavExt != NULL)
					strcpy(outWavExt, ".bin");
			}
			else
				outName = argv[end];

			/* if output file name is entered with no extension,
			 * add extension based on input */

			char* outMuExt  = strstr(outName, ".bin");
			char* outWavExt = strstr(outName, ".wav");
			
			if(inputIsWav == 1 && outMuExt == NULL)
				sprintf(outName, "%s%s", outName, ".bin");

			if(inputIsWav == 0 && outWavExt == NULL)
				sprintf(outName, "%s%s", outName, ".wav");

			writeFile(outName, inputIsWav);
			
			if (freeBuffers() == 0)
				printf("Possible memory corruption!\n");
		}
	}
	
	return 0;
}

static void usage()
{
	printf("\nmu2wav v0.8 by _astriid_\n\n"
		"Usage: mu2wav infile [options] [outfile]\n\n"
		"infile must be .bin (mu-law ADPCM) or .wav (PCM)\n"
		"input .wav files must be 8, 16, or 24 bit\n"
		"output .wav files will be 16 bit\n\n"
		"Options:\n"
		"  -s\t| [ output file's resampled sample rate ]\n"
		"    \t  input mu-law files are treated as having a\n"
		"    \t  22050hz sample rate. If option is not set\n"
		"    \t  output wav file sample rate default is\n"
		"    \t  22050hz and output mulaw sample rate default\n"
		"    \t  is equal to wav file input sample rate\n\n");
} 
