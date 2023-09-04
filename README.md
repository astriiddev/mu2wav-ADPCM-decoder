# mu2wav mulaw ADPCM decoder and encoder
Converter for Linndrum, Drumulator, and similar 8-bit mu-law ADPCM audio samples

Convert mu-law binary files and to mono 16-bit .wav files at 22050hz
Default sample rate set tp 22050hz to allow for original variable sample rate tuning range.

Convert mono 8-bit, 16-bit, and 24-bit .wav files to mu-law encoded binary files.

# Usage
    mu2wav v0.8 by _astriid_
    
    Usage: mu2wav infile [options] [outfile]
    
    infile must be .bin (mu-law ADPCM) or .wav (PCM)
    input .wav files must be 8, 16, or 24 bit
    output .wav files will be 16 bit
    
    Options:
      -s    | [ output file's resampled sample rate ]
              input mu-law files are treated as having a
              22050hz sample rate. If option is not set
              output wav file sample rate default is
              22050hz and output mulaw sample rate default
              is equal to wav file input sample rate

# Installation
Can either be downloaded from release or compiled with compiler of your choice.

current release exe compiled with GCC 11.4.0 in Power Shell with settings:

    gcc -O2 -s main.c readwrite.c compand.c -lm -o mu2wav.exe

(Previous release compiled with UCRT64, further testing proved GCC compilation under
Power Shell imporved speeds up to 4 times faster)

 # Todo
- Add stereo mixing for wav
- ??? possibly add other file format support
