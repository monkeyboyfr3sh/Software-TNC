#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>  
#include <math.h>

using namespace std;

#define AMPLITUDE   3.3             //Amplitude of desired sine wave
#define PI          3.14159         //Rounded value of PI
#define BIT_SAMP    200
#define BITCOUNT    16                //Number of bits for the waveform

#define LOWF_SAMP   BIT_SAMP                //This is the sample count for the low frequency , as configured maps to 1200Hz
#define HIGHF_SAMP  int(BIT_SAMP/1.833)     //This is the sample count for the high frequency, as configured maps to 2200Hz

double lowFrequency[LOWF_SAMP];
double highFrequency[HIGHF_SAMP];
bool bitstream[] = {    0, 1, 1, 1, 1, 1, 1, 0,
                      //0, 1, 2, 3, 4, 5, 6, 7
                      /*1, 0, 1, 0, 1, 0, 1, 0,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0,
                        1, 0, 1, 0, 1, 0, 1, 0,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0,*/

                        0, 1, 1, 1, 1, 1, 1, 0
};

void genSine(double* sinArray, int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        sinArray[i] = (sin(i * 2 * PI / arraySize) + 1) * (AMPLITUDE / 2);
    }
}
void generateDatafile() {
    ofstream outfile;
    outfile.open("data.csv", ios::trunc | ios::out);

    genSine(&highFrequency[0], HIGHF_SAMP);
    genSine(&lowFrequency[0], LOWF_SAMP);

    for (int i = 0; i < BITCOUNT; i++) {
        //Insert high frequency. Need 2 waveforms to approximate 1 bit length
        if (bitstream[i]) {
            for (int i = 0; i < BIT_SAMP; i++)
            {
                outfile << highFrequency[(i+15)%HIGHF_SAMP] << ",";
            }
        }   
        //Insert low frequency
        else {
            for (int i = 0; i < LOWF_SAMP; i++)
            {
                outfile << lowFrequency[i % LOWF_SAMP] << ",";
            }
        }
    }
}

int main()
{   
    generateDatafile();
    return 0;
}