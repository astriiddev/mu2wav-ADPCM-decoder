#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "readwrite.h"
#include "editor.h"

extern uint32_t* getSampleRate();
extern void		 setSampleRate(uint32_t);

extern uint32_t* getMuLength();
extern uint8_t*  getMuSamp(uint32_t);
extern uint8_t** getMuBuffer();
extern void		 newMuBuffer(uint32_t);
extern void		 setMuSamp(uint32_t, uint8_t);

extern uint32_t* getWavLength();
extern int16_t*  getWavSamp(uint32_t);
extern int16_t** getWavBuffer();
extern void		 newWavBuffer(uint32_t);
extern void		 setWavSamp(uint32_t, int16_t);

static uint32_t* editBuffer	= NULL;

static int isWav = 0;
static uint32_t editLength = 0;

void sampleEdit(const int isInputWav)
{
	/* copy audio data into edit buffer for editing */

	isWav = isInputWav;

	if (editBuffer != NULL)
	{
		free(editBuffer);
		editBuffer = NULL;
	}
	
	if (isWav)
		editLength = *getWavLength();
	else
		editLength = *getMuLength();

	if(editLength > 0)
	{
		editBuffer = malloc(editLength * sizeof *editBuffer);

		for(int i = 0; i < editLength; i++)
		{
		
			if(isWav && wavBuffer != NULL)
				editBuffer[i] = 0xFFFF & (int)*getWavSamp(i);	
		
			else if (!isWav && muBuffer != NULL)
				editBuffer[i] = 0xFF & (int)*getMuSamp(i);
			else
			{
				printf("Could not edit files!\n");
				free(editBuffer);
				editBuffer = NULL;
				return;
			}
		}
	}
	
}

void fileEditor()
{
	/* copy edited audio back into buffer */

	if (editBuffer == NULL)
		return;

	if(isWav)
		newWavBuffer(editLength);
	else
		newMuBuffer(editLength);

	if(editBuffer != NULL)
	{
		for (int i = 0; i < editLength; i++)
		{
			if (isWav && getWavBuffer != NULL)
				setWavSamp(i, 0xFFFF & (int16_t)editBuffer[i]);
			else if (!isWav && getMuBuffer() != NULL)
				setMuSamp(i, 0xFF & (uint8_t)editBuffer[i]);
			else
				break;
		}
	}

	if ((isWav && getWavBuffer() == NULL) || (!isWav && getMuBuffer() == NULL))
		printf("Unable to write edited audio!\n");

	free(editBuffer);
	editBuffer = NULL;
}

void resampleAudio(const uint32_t newSampleRate)
{
	/* nearest neightbor resampling */
	static uint32_t* resampleBuffer	= NULL;

	if(!isWav)
		setSampleRate(newSampleRate);

	if (resampleBuffer != NULL)
	{
		free(resampleBuffer);
		resampleBuffer = NULL;
	}

	double resampleRatio = 0.0;
	
	if(isWav)
		resampleRatio = (double)newSampleRate / *getSampleRate();
	else
		resampleRatio = (double)newSampleRate / 22050;

	const double holdRatio = 1.00 / resampleRatio;
	
	double sampleAndHold = 0.0;

	if(editBuffer != NULL && editLength > 0)
	{
		int resampledLength = (int)floor((double)editLength * resampleRatio);
		
		if(resampledLength <= 0)
			return;
		
		resampleBuffer = malloc(resampledLength * sizeof *resampleBuffer);

		for (int i = 0; i < resampledLength; i++)
		{
			const int pos = (int)floor(sampleAndHold);
			
			if(pos >= editLength)
				break;

			resampleBuffer[i] = editBuffer[pos];

			sampleAndHold += holdRatio;
		}

		free(editBuffer);
		editBuffer = NULL;

		editLength = resampledLength;
		editBuffer = malloc(editLength * sizeof *editBuffer);
		memcpy(editBuffer, resampleBuffer, editLength * sizeof *editBuffer);
		
		free(resampleBuffer);
		resampleBuffer = NULL;
	}
}