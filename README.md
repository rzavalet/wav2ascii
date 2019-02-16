# wav2ascii
Wave audio format file to ASC format file

The Wav format file is an audio format file developed by IBM and Microsoft (in fact, it is the most commonly way to store raw and uncompressed audio in windows). It has the following characteristics:

1) It does not use any compression, typically;

2) Stores audio in mono and stereo;

3) Permits many sample rates and resolution;

4) Takes into account the little-endianess of the Intel CPU

Wav format file is usually used with PCM[1][2]. This means that this audio format file uses no compression. What that means is that the resulting file is huge, e.g. in order to have a one-minute audio file with audio CD quality[3] we need 10 MB of disc space available, approximately.

One of its limitations is that it can only store an audio file of about 6.6 hrs, i.e. 4GB of disc space. Why? A Wav file has a field in its header which stores its length. This field is 32 bits[4] wide. That is 2^32 [bits]/2^30[bits/GB] = 2^2[GB] = 4[GB].

## To build:
$ cd src

$ gcc  parser.c -o parser -ggdb -lm

## To execute:
$ ./parser ../sample_wav/11k16bitpcm.wav out


REFERENCES

[1]http://es.wikipedia.org/wiki/Waveform_Audio_Format

[2]http://en.wikipedia.org/wiki/WAV

[3]https://ccrma.stanford.edu/courses/422/projects/WaveFormat/

[4]http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html
