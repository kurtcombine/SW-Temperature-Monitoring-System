#include "sound.h"

bool sound_playing = false;
int sound_counter = 0;
int sound_length = 0;
int sound_max = 0;

void config_DMA()
{
	LPC_SC->PCONP |= (1 << 29);
	LPC_GPDMA->DMACConfig = 1;
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
}

void DMA_IRQHandler(void)
{
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
	sound_playing = false;
}

void config_DAC()
{
	PIN_Configure(0, 26, 2, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);
	LPC_DAC->DACCTRL |= (1 << 2) | (1 << 3);
	LPC_DAC->DACCNTVAL = 568 * 4; //25 MHz -> 44 kHz
}

void config_sound()
{
	config_DMA();
	config_DAC();
}

void play_sound(const uint16_t sound[], int sound_len)
{
	if (sound_playing == true)
		return;
	sound_playing = true;
	
	LPC_GPDMACH0->DMACCSrcAddr = (int)sound;
	LPC_SC->PCONP |= (1 << 29);
	LPC_GPDMA->DMACConfig = 1;
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
	
	LPC_GPDMACH0->DMACCDestAddr = (int)&LPC_DAC->DACR;
	LPC_GPDMACH0->DMACCLLI = 0;
	LPC_GPDMACH0->DMACCControl = sound_len | (2 << 18) | (2 << 21) | (1 << 26) | (1UL << 31);
	NVIC_EnableIRQ(DMA_IRQn);
	LPC_GPDMACH0->DMACCConfig = 1 | (7 << 6) | (1 << 11) | (1 << 14) | (1 << 15);
}
