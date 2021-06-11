#include <iostream>
#include <fstream>
#include <cmath>
#define PI 3.141592653589793

using namespace std;

// RIFF chunk
const std::string CHUNK_ID = "RIFF";
const std::string CHUNK_SIZE = "----"; // 4 bytes
const std::string FORMAT = "WAVE";

// FMT sub-chunk
const std::string SUBCHUNK1_ID = "fmt ";
const int SUBCHUNK1_SIZE = 16;
const int AUDIO_FORMAT = 1;
const int NUM_CHANNELS = 2; // Stereo = 2, Mono = 1
const int SAMPLE_RATE = 44100;
const int BYTE_RATE = SAMPLE_RATE * NUM_CHANNELS * (SUBCHUNK1_SIZE / 8);
const int BLOCK_ALIGN = NUM_CHANNELS * (SUBCHUNK1_SIZE / 8);
const int BITS_PER_SAMPLE = 16; 

// Data sub-chunk
const std::string SUBCHUNK2_ID = "data";
const std::string SUBCHUNK2_SIZE = "----"; // 4 bytes

const int DURATION = 2;			// affects audio
const int MAX_AMP = 32760;		
const double FREQUENCY = 420.69;	// affects audio


void writeAsBytes(ofstream &file, int value, int byte_size){
	file.write(reinterpret_cast<const char *>(&value), byte_size);
}

int main(){

	ofstream wav; // The file
	wav.open("test.wav", ios::binary);

	if(wav.is_open()){
		// RIFF chunk
		wav << CHUNK_ID;
		wav << CHUNK_SIZE;
		wav << FORMAT;

		// sub-chunk1
		wav << SUBCHUNK1_ID;
		writeAsBytes(wav, SUBCHUNK1_SIZE, 4);
		writeAsBytes(wav, AUDIO_FORMAT, 2);
		writeAsBytes(wav, NUM_CHANNELS, 2);
		writeAsBytes(wav, SAMPLE_RATE, 4);
		writeAsBytes(wav, BYTE_RATE, 4);
		writeAsBytes(wav, BLOCK_ALIGN, 2);
		writeAsBytes(wav, BITS_PER_SAMPLE, 2);

		// sub-chunk2
		wav << SUBCHUNK2_ID;
		wav << SUBCHUNK2_SIZE;
		
		int start_audio = wav.tellp(); // Start bit

		for(int i = 0; i < SAMPLE_RATE * DURATION; i++){ // ~88000 times
			double amp = (double)i / SAMPLE_RATE * MAX_AMP;
			double value = sin((2 * PI * i * FREQUENCY) / SAMPLE_RATE);

			double channel1 = amp * value / 2;
			double channel2 = (MAX_AMP - amp) * value;

			writeAsBytes(wav, channel1, 2);
			writeAsBytes(wav, channel2, 2);
		}

		int end_audio = wav.tellp(); // End bit
		
		wav.seekp(start_audio - 4);
		writeAsBytes(wav, start_audio - end_audio, 4);

		wav.seekp(4, ios::beg);
		writeAsBytes(wav, end_audio - 8, 4);
	}

	wav.close();

	return 0;
}