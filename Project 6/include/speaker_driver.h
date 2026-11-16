/**********************************************************************
 * Name: Speaker Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/07/2026 (mm/dd/yyyy)
 * Date Created:  10/07/2026 (mm/dd/yyyy)
 * Description:
    * Music notes and their corresponding frequencies in Hz for use
    * with a DAC output 
**********************************************************************/
#ifndef speaker_driver
#include <cstdint>
#define speaker_driver
// Notes
    // Octave 3
        #define C3      131U
        #define C3_SH   139U
        #define D3      147U
        #define D3_SH   156U
        #define E3      165U
        #define F3      175U
        #define F3_SH   185U
        #define G3      196U
        #define G3_SH   208U
        #define A3      220U
        #define A3_SH   233U
        #define B3      247U

    // Octave 4
        #define C4      262U
        #define C4_SH   277U
        #define D4      294U
        #define D4_SH   311U
        #define E4      330U
        #define F4      349U
        #define F4_SH   370U
        #define G4      392U
        #define G4_SH   415U
        #define A4      440U
        #define A4_SH   466U
        #define B4      494U

    // Octave 5
        #define C5      523U
        #define C5_SH   554U
        #define D5      587U
        #define D5_SH   622U
        #define E5      659U
        #define F5      698U
        #define F5_SH   740U
        #define G5      784U
        #define G5_SH   831U
        #define A5      880U
        #define A5_SH   932U
        #define B5      988U

// Static Vars
    const uint16_t sine_lut[100] = {
                2048, 2110, 2173, 2235, 2296, 2357, 2416, 2474, 2530, 2584,
                2636, 2685, 2732, 2777, 2818, 2857, 2892, 2924, 2953, 2978,
                2999, 3016, 3030, 3040, 3046, 3048, 3046, 3040, 3030, 3016,
                2999, 2978, 2953, 2924, 2892, 2857, 2818, 2777, 2732, 2685,
                2636, 2584, 2530, 2474, 2416, 2357, 2296, 2235, 2173, 2110,
                2048, 1985, 1922, 1860, 1799, 1738, 1679, 1621, 1565, 1511,
                1459, 1410, 1363, 1318, 1277, 1238, 1203, 1171, 1142, 1117,
                1096, 1079, 1065, 1055, 1049, 1048, 1049, 1055, 1065, 1079,
                1096, 1117, 1142, 1171, 1203, 1238, 1277, 1318, 1363, 1410,
                1459, 1511, 1565, 1621, 1679, 1738, 1799, 1860, 1922, 1985
            };

// "Functions"
    #define speaker_set_frequency(frequency) TIM6->ARR = frequency - 1;
    #define speaker_set_amplitude(amplitude) DAC->DHR12R1 = amplitude;
#endif