# mu2wav mulaw ADPCM decoder and encoder
Converter for Linndrum, Drumulator, and similar 8-bit mu-law ADPCM audio samples

Convert mu-law binary files and to mono 16-bit .wav files at 22050hz
Default sample rate set tp 22050hz to allow for original variable sample rate tuning range.

Convert mono 16-bit .wav files to mu-law encoded binary files.

# Installation
Can either be downloaded from release or compiled with compiler of your choice.

release exe compiled with GCC 11.4.0 in MinGW UCRT64 with settings:

    gcc -o2 -s main.c readwrite.c compand.c -lm -o mu2wav.exe

UCRT64 and -o2 -s flags recommended for best performance with smallest file size

 # Todo
- Add support for various wav bit depths
- Add stereo mixing for wav
- Add resampling for wav
