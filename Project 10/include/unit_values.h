/**********************************************************************
 * Name: Unit Values
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/02/2026 (mm/dd/yyyy)
 * Date Created:  09/12/2026 (mm/dd/yyyy)
 * Description:
    * Lets me quickly and legibly add common values into my code using 
    * prefixs or coloquial names
**********************************************************************/
#ifndef UNIT_VALUES
#define UNIT_VALUES
    // SI Prefixes
        #define TERA    1000000000000
        #define GIGA       1000000000
        #define MEGA          1000000
        #define KILO             1000
        #define HECTO             100
        #define DECA               10
        #define DECI                0.1
        #define CENTI               0.01
        #define MILLI               0.001
        #define MICRO               0.000001
        #define NANO                0.000000001

    // Coloquial Names
        #define TRILLION    TERA
        #define BILLION     GIGA
        #define MILLION     MEGA
        #define THOUSAND    KILO
        #define HUNDRED     HECTO
        #define TEN         DECA
        #define TENTH       DECI
        #define HUNDREDTH   CENTI
        #define THOUSANDTH  MILLI
        #define MILLIONTH   MICRO
        #define BILLIONTH   NANO

        #define TEN_E_N1 DECI
        #define TEN_E_N2 CENTI
        #define TEN_E_N3 MILLI
        #define TEN_E_N4 0.0001
        #define TEN_E_N5 0.00001
        #define TEN_E_N6 MICRO
        #define TEN_E_N7 0.0000001
        #define TEN_E_N8 0.00000001
        #define TEN_E_N9 NANO
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
    // Notes Float
        // Octave 3
            #define C3_f      1308U
            #define C3_SH_f   1385U
            #define D3_f      1468U
            #define D3_SH_f   1555U
            #define E3_f      1648U
            #define F3_f      1746U
            #define F3_SH_f   1850U
            #define G3_F      1960U
            #define G3_SH_f   2076U
            #define A3_f      2200U
            #define A3_SH_f   2330U
            #define B3_f      2469U

        // Octave 4
            #define C4_f      2616U
            #define C4_SH_f   2771U
            #define D4_f      2936U
            #define D4_SH_f   3111U
            #define E4_f      3296U
            #define F4_f      3492U
            #define F4_SH_f   3699U
            #define G4_f      3920U
            #define G4_SH_f   4153U
            #define A4_f      4400U
            #define A4_SH_f   4661U
            #define B4_f      4938U

        // Octave 5
            #define C5_f      5232U
            #define C5_SH_f   5543U
            #define D5_f      5873U
            #define D5_SH_f   6222U
            #define E5_f      6592U
            #define F5_f      6984U
            #define F5_SH_f   7399U
            #define G5_f      7839U
            #define G5_SH_f   8306U
            #define A5_f      8800U
            #define A5_SH_f   9323U
            #define B5_f      9877U
#endif