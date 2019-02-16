#ifndef _PARSER_
#define _PARSER_

#define WAVE_FORMAT_PCM   0x0001

#define ID     unsigned int
#define SIZE   unsigned int


typedef struct {
	unsigned int  FMT_ID;		/*Debe decir WAVE*/
	unsigned int  cksize;
	unsigned int	wFormatTag;		/*WAVE_FORMAT_PCM*/
	unsigned int	nChannels;
	unsigned int	nSamplesPerSec;
	unsigned int	nAvgBytesPerSec;
	unsigned int	nBlockAlign;
	unsigned int	wBitsPerSample;
	/*int	cbSize;*/
} FMT_CHUNK;

typedef struct {
	ID   FACT_ID;			/*Debe decir fact*/
	SIZE cksize;
	int  dwSampleLength;
} FACT_CHUNK;

typedef struct {
	ID    DATA_ID;			/*Debe decir data*/
	SIZE  cksize;
	int   *data;
	char  pad;
} DATA_CHUNK;

typedef struct {
	ID	        WAVE_ID;		/*Debe decir WAVE*/
	FMT_CHUNK  *fmt_chunk;
	/*FACT_CHUNK *fact_chunk;*/
	DATA_CHUNK *data_chunk;
} WAVE_CHUNK;
	
typedef struct {
	ID           RIFF_ID;        /*Debe decir RIFF*/
	SIZE         cksize;
	WAVE_CHUNK   *wave_chunk;
} RIFF_CHUNK;

#endif
