/*  
    
    8-bit mu-law ADPCM to 16-bit PCM decoder
 
    Converts sample rom dumps from Linndrum, Drumulator, 
      or other mu-law samplers and outputs a  mono
      16-bit PCM wav file.

    Default sample rate set tp 22050hz to allow for 
      original variable sample rate tuning range.

    9/19/2022 - 4/30/2023, by Finley Baker 
    
*/

#include <iostream>
#include <fstream>
#include <string>

constexpr auto sampleRate = 22050, bitDepth = 16, numOfChannels = 1;        //sets output WAV file to mono, 16 bit at 22050hz sample rate

int16_t decodedSample, *decodedArray;
uint32_t sampleLength;
char *sample;

int fileRead(), writeFile(), sampleDecode(uint8_t uSample);

int main() {
    
    while (1)
    {
        fileRead();
    }

    return 0;
}

int fileRead()                                                                                              //read input .bin file
{
    std::string inFileName;
    std::cout << "Type \"exit\" to leave program.\n" << "Binary file to convert: ";
    std::cin >> inFileName, "\n";

    if (inFileName == "exit" || inFileName == "Exit" || inFileName == "EXIT")                               //type exit to leave decoder
    {
        exit(0);
    }

    std::ifstream linnFile;

    if (linnFile) {

        linnFile.open(inFileName.find(".bin", 0) == std::string::npos ? inFileName + ".bin" : inFileName,   //input .bin 8-bit mu-255law file
            std::ios::in | std::ios::binary);

        if (!linnFile)
        {
            std::cout << "File not fould!\n\n";
            return 1;
        }

        linnFile.seekg(0, std::ios::end);
        uint64_t length = linnFile.tellg();

        sampleLength = uint32_t(length);                                                        //sets output PCM length to same as input length

        sample = new char[length];
        linnFile.seekg(0, std::ios::beg);
        linnFile.read(&sample[0], length / sizeof(char));

        if (linnFile) {
            std::cout << "Reading: " << length << " samples.\nAll samples read successfully!\n\n";
        }
        else
            std::cout << "error: only " << linnFile.gcount() << " couldbe read";
        linnFile.close();
    }

    writeFile();

    return 0;
}

int sampleDecode(uint8_t uSample) {
    if (uSample > 0xFF) {
        std::cerr << "Invalid bit depth!\n";
        return 1;
    }

    else if ((uSample & 0x80) == 0x80) {
        uSample ^= 0x80;
        decodedSample = (int16_t)round((((pow(256, (double(uSample) / 127))) - 1) * 32767) / 255);      //decode positive side of waveform
    }
    else if ((uSample & 0x80) == 0x00) {
        decodedSample = (int16_t)round((((pow(256, (double(uSample) / 127))) - 1) * -32767) / 255);     //decode negative side of waveform
    }

    return 0;

}

template<typename T>
void writeToFile(std::ofstream& file, T value, int size) {
    file.write(reinterpret_cast<char*> (&value), size);
}

int writeFile() {                                                                                       //generate output .wav file
    bool rawBool = false;
    std::string writeRaw;
    std::string outFileName;
    std::cout << "WAV File Name: ";
    std::cin.ignore();
    std::getline(std::cin, outFileName, '\n');

    if (outFileName == "")
    {
        exit(0);
    }

    std::cout << "Export RAW PCM? ";
    std::getline(std::cin, writeRaw, '\n');

    if (writeRaw == "yes" || writeRaw == "YES" || writeRaw == "Yes" || writeRaw == "y" || writeRaw == "Y")
    {
        rawBool = true;
    }

    decodedArray = new int16_t[sampleLength];                                                   //set PCM buffer to length of audio

    struct WAV_HEADER {                                                                         //RIFF Wave format header
        uint8_t chunkID_RIFF[4] = { 'R', 'I', 'F', 'F' };
        uint32_t chunkSize_RIFF = (sampleLength * 2) + 36;
        uint8_t RIFF_Type[4] = { 'W', 'A', 'V', 'E' };
        uint8_t subchunkID_fmt_[4] = { 'f', 'm', 't', ' ' };
        uint32_t subchunkSize_fmt_ = 16;
        uint16_t compressionType = 1;
        uint16_t NumOfChan = numOfChannels;
        uint32_t SamplesPerSec = sampleRate;
        uint32_t bytesPerSec = (sampleRate * numOfChannels * bitDepth) / 8;
        uint16_t blockAlign = (numOfChannels * bitDepth) / 8;
        uint16_t bitsPerSample = bitDepth;
        uint8_t subchunkID_data[4] = { 'd', 'a', 't', 'a' };
        uint32_t subchunkSize_data = (sampleLength * 2) - 44;
    } wav_hdr;

    std::ofstream rawFile;
    std::ofstream wavFile;

    if (!rawFile) {
        std::cout << "Cannot open file!" << "\n";
        return 1;
    }
    if (!wavFile) {
        std::cout << "Cannot open file!" << "\n";
        return 1;
    }

    if (rawBool) { rawFile.open(outFileName + ".raw", std::ios::binary); }
    wavFile.open(outFileName + ".wav", std::ios::binary);

    writeToFile(wavFile, wav_hdr, 44);

    for (uint16_t i = 0; i < sampleLength; i++) {                                               //decode mu-law .bin to 16-bit PCM .wav
        sampleDecode(sample[i]);

        decodedArray[i] = decodedSample;

        if (decodedArray == nullptr)
        {
            std::cerr << "Error: memory could not be allocated";
            return 1;
        }

        if (i > sampleLength)
        {
            return 1;
        }

        else {
            if (rawBool) { writeToFile(rawFile, decodedArray[i], 2); }
            writeToFile(wavFile, decodedArray[i], 2);
        }
    }

    delete[]decodedArray;
    decodedArray = nullptr;
    delete[] sample;
    sample = nullptr;

    rawFile.close();

    if (rawBool && !rawFile.good()) {
        std::cout << "Error occurred at writing time!" << "\n";
        return 1;
    }
    wavFile.close();
    if (!wavFile.good()) {
        std::cout << "Error occurred at writing time!" << "\n";
        return 1;
    }

    std::cout << "\nReading....\n" << "Sample size: " << sampleLength << "\nConversion Successful!\n\n";

    return 0;

}
