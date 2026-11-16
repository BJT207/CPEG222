/**********************************************************************
 * Name: Unit Values
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Date: 9/12/2026 (mm/dd/yyyy)
 * Description:
    * I got tired of typing out long numbers so this lets me quickly 
    * and legibly add common large values into my code using either 
    * the SI prefix or the coloquial name
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
#endif