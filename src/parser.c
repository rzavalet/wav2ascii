#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "parser.h"

#define SUCCESS (0)
#define FAIL    (1)

FMT_CHUNK   fmt_chunk;
DATA_CHUNK  data_chunk;
WAVE_CHUNK  wave_chunk;
RIFF_CHUNK  riff_chunk;

int byte2intn(char *c, int size)
{
  int n = 0;
  int i = 0;

  for (i=0; i< size; i++) {
    n |= (c[i] << (8 * i));
  }

#if 0
  if (n > pow(16, size)-1) {
    n += 1;
    n = ~n;
  }
#endif

  return n;
}

int byte2int4(char c[4])
{
  return (c[0] | c[1] << 8 | c[2] << 16 | c[3] << 24);
}

int byte2int2(char c[2])
{
  return c[0] | c[1] << 8;
}

int main(int argc, char* argv[])
{
  int  rc = SUCCESS;
  int  numSamples;
  int  sampleWidth;
  int  i;
  int  pad;
  int  aux;
  int  counter = 0;
  char c4[4];      /* Buffer de 4 bytes */
  char c2[2];      /* Buffer de 2 bytes */
  char *samples = NULL;
  FILE *forigen = NULL;
  FILE *fdestino = NULL;
  
  
  if (argc != 3) {
    fprintf(stderr, 
            "Forma de Uso: \n"
            "  %s <origen> <destino>\n", 
            argv[0]);
    exit(-1);
  }

  forigen = fopen(argv[1], "r");
  if (forigen == NULL) {
    perror(argv[1]);
    rc = FAIL;
    goto done;
  }

  fdestino = fopen(argv[2], "w");
  if (fdestino == NULL) {
    perror(argv[2]);
    goto failXit;
  }
  
  /*------------------------------------------
   * Aqui inicia el parsing
   *-----------------------------------------*/

  /*-----------------------------------------
   * RIFF CHUNK
   *-----------------------------------------*/
  fread(c4, sizeof(c4), 1, forigen);
  if (c4[0] == 'R' && c4[1] == 'I' && c4[2] == 'F' && c4[3] == 'F') {
    fprintf(stdout, "Found RIFF tag\n");
  }
  else {
    fprintf(stderr, "Oops, not a RIFF file\n");
    goto failXit;
  }

  /*Hace la copia al Chunk*/
  fread(c4, sizeof(c4), 1, forigen);
  riff_chunk.cksize = byte2int4(c4);
  fprintf(stdout, "  [cksize] = %u\n", riff_chunk.cksize);

  /*-----------------------------------------
   * WAVE CHUNK
   *-----------------------------------------*/
  fread(c4, sizeof c4, 1, forigen);
  if (c4[0] == 'W' && c4[1] == 'A' && c4[2] == 'V' && c4[3] == 'E') {
    fprintf(stdout, "Found WAVE tag\n");
  }
  else {
    fprintf(stderr, "Oops, not a wave file...\n");
    goto failXit;
  }

  /*-----------------------------------------
   * fmt CHUNK
   *-----------------------------------------*/
  fread(c4, sizeof c4, 1, forigen);
  if (c4[0] == 'f' && c4[1] == 'm' && c4[2] == 't' && c4[3] == 0x20) {
    fprintf(stdout, "Found fmt_ tag\n");
  }
  else {
    fprintf(stderr, "Oops, could not find fmt_ tag\n");
    goto failXit;
  }

  fread(c4, sizeof c4, 1, forigen);
  fmt_chunk.cksize = byte2int4(c4);
  fprintf(stdout, "  [cksize] = %u\n", fmt_chunk.cksize);

  if (fmt_chunk.cksize != 16) {
    fprintf(stderr, "ERROR: This program only works with 16bits wave files\n");
    goto failXit;
  }

  fread(c2, sizeof c2, 1, forigen);
  fmt_chunk.wFormatTag = byte2int2(c2);
  fprintf(stdout, "  [wFormatTag] = %u\n", fmt_chunk.wFormatTag);

  if (fmt_chunk.wFormatTag != 1) {
    fprintf(stderr, "ERROR: This program only works with PCM wave files\n");
    goto failXit;
  }

  fread(c2, sizeof c2, 1, forigen);
  fmt_chunk.nChannels = byte2int2(c2);
  fprintf(stdout, "  [nChannels] = %u\n", fmt_chunk.nChannels);

  fread(c4, sizeof c4, 1, forigen);
  fmt_chunk.nSamplesPerSec = byte2int4(c4);
  fprintf(stdout, "  [nSamplesPerSec] = %u\n", fmt_chunk.nSamplesPerSec);

  fread(c4, sizeof c4, 1, forigen);
  fmt_chunk.nAvgBytesPerSec = byte2int4(c4);
  fprintf(stdout, "  [nAvgBytesPerSec] = %u\n", fmt_chunk.nAvgBytesPerSec);

  fread(c2, sizeof c2, 1, forigen);
  fmt_chunk.nBlockAlign = byte2int2(c2);
  fprintf(stdout, "  [nBlockAlign] = %u\n", fmt_chunk.nBlockAlign);

  fread(c2, sizeof c2, 1, forigen);
  fmt_chunk.wBitsPerSample = byte2int2(c2);
  fprintf(stdout, "  [wBitsPerSample] = %u\n", fmt_chunk.wBitsPerSample);

  /*-----------------------------------------
   * data CHUNK
   *-----------------------------------------*/
  fread(c4, sizeof c4, 1, forigen);
  if (c4[0] =='d' && c4[1] == 'a' && c4[2] == 't' && c4[3] == 'a') {
    fprintf(stdout, "Found data tag\n");
  }
  else {
    fprintf(stderr, "Oops, could not find data tag\n");
    goto failXit;
  }

  fread(c4, sizeof c4, 1, forigen );
  data_chunk.cksize = byte2int4(c4);
  fprintf(stdout, "  [cksize] = %u\n", data_chunk.cksize);

  sampleWidth = fmt_chunk.nBlockAlign / fmt_chunk.nChannels;
  fprintf(stdout, "    [sample_width] = %u\n", sampleWidth);

  if (fmt_chunk.wBitsPerSample < 8*sampleWidth) {
    fprintf(stderr, "Oops, bad BitsPerSample\n");
    goto failXit;
  }

  numSamples = data_chunk.cksize / (sampleWidth * fmt_chunk.nChannels);

  aux = 4 + 24 + (8 + sampleWidth * fmt_chunk.nChannels * numSamples);
  if (aux % 2 == 0) {
     pad = 0;
  }
  else {
     pad = 1;
  }

  if (riff_chunk.cksize != aux + pad) {
    fprintf(stderr, "Oops, bad file size\n");
    goto failXit;
  }

  /*Ahora si leemos los datos*/
  samples = (char *)malloc(sampleWidth);
  if (samples == NULL) {
    perror("Could not allocate space for samples");
    goto failXit;
  }

  while (fread(samples, sizeof(*samples), 1, forigen) > 0) {
#if 1
    for (i=0; i<sampleWidth; i++) {
      printf("%x ", (char)samples[sampleWidth - i - 1]);
    }
    printf("\n");
#endif

    /* save in same endianess */
    fwrite(samples, sizeof (*samples), 1, fdestino);
  }

  goto done;

failXit:
  rc = FAIL;

done:
  if (samples) {
    free(samples);
  }

  if (forigen) {
    fclose(forigen);
  }
  
  if (fclose) {
    fclose(fdestino);
  }

  return rc;
}

