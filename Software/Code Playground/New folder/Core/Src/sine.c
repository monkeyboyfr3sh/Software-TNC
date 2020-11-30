/*
 * sine.c
 *
 *  Created on: Nov 1, 2020
 *      Author: monke
 */
#include "sine.h"

 uint32_t wave[2*FREQ_SAMP] = {
		2048,2091,2134,2177,2219,2262,2305,2347,
		2390,2432,2474,2516,2557,2599,2640,2681,
		2722,2762,2802,2842,2881,2920,2959,2997,
		3035,3072,3109,3145,3181,3217,3252,3286,
		3320,3353,3386,3418,3450,3481,3511,3541,
		3570,3598,3626,3653,3679,3705,3730,3754,
		3777,3800,3822,3843,3863,3882,3901,3919,
		3936,3952,3968,3982,3996,4009,4021,4032,
		4042,4051,4060,4067,4074,4080,4085,4089,
		4092,4094,4095,4095,4095,4094,4092,4089,
		4085,4080,4074,4067,4060,4051,4042,4032,
		4021,4009,3996,3982,3968,3952,3936,3919,
		3901,3882,3863,3843,3822,3800,3777,3754,
		3730,3705,3679,3653,3626,3598,3570,3541,
		3511,3481,3450,3418,3386,3353,3320,3286,
		3252,3217,3181,3145,3109,3072,3035,2997,
		2959,2920,2881,2842,2802,2762,2722,2681,
		2640,2599,2557,2516,2474,2432,2390,2347,
		2305,2262,2219,2177,2134,2091,2048,2005,
		1962,1919,1877,1834,1791,1749,1706,1664,
		1622,1580,1539,1497,1456,1415,1374,1334,
		1294,1254,1215,1176,1137,1099,1061,1024,
		987,951,915,879,844,810,776,743,
		710,678,646,615,585,555,526,498,
		470,443,417,391,366,342,319,296,
		274,253,233,214,195,177,160,144,
		128,114,100,87,75,64,54,45,
		36,29,22,16,11,7,4,2,
		0,0,0,2,4,7,11,16,
		22,29,36,45,54,64,75,87,
		100,114,128,144,160,177,195,214,
		233,253,274,296,319,342,366,391,
		417,443,470,498,526,555,585,615,
		646,678,710,743,776,810,844,879,
		915,951,987,1024,1061,1099,1137,1176,
		1215,1254,1294,1334,1374,1415,1456,1497,
		1539,1580,1622,1664,1706,1749,1791,1834,
		1877,1919,1962,2005,2048,

		2048,2091,2134,2177,2219,2262,2305,2347,
		2390,2432,2474,2516,2557,2599,2640,2681,
		2722,2762,2802,2842,2881,2920,2959,2997,
		3035,3072,3109,3145,3181,3217,3252,3286,
		3320,3353,3386,3418,3450,3481,3511,3541,
		3570,3598,3626,3653,3679,3705,3730,3754,
		3777,3800,3822,3843,3863,3882,3901,3919,
		3936,3952,3968,3982,3996,4009,4021,4032,
		4042,4051,4060,4067,4074,4080,4085,4089,
		4092,4094,4095,4095,4095,4094,4092,4089,
		4085,4080,4074,4067,4060,4051,4042,4032,
		4021,4009,3996,3982,3968,3952,3936,3919,
		3901,3882,3863,3843,3822,3800,3777,3754,
		3730,3705,3679,3653,3626,3598,3570,3541,
		3511,3481,3450,3418,3386,3353,3320,3286,
		3252,3217,3181,3145,3109,3072,3035,2997,
		2959,2920,2881,2842,2802,2762,2722,2681,
		2640,2599,2557,2516,2474,2432,2390,2347,
		2305,2262,2219,2177,2134,2091,2048,2005,
		1962,1919,1877,1834,1791,1749,1706,1664,
		1622,1580,1539,1497,1456,1415,1374,1334,
		1294,1254,1215,1176,1137,1099,1061,1024,
		987,951,915,879,844,810,776,743,
		710,678,646,615,585,555,526,498,
		470,443,417,391,366,342,319,296,
		274,253,233,214,195,177,160,144,
		128,114,100,87,75,64,54,45,
		36,29,22,16,11,7,4,2,
		0,0,0,2,4,7,11,16,
		22,29,36,45,54,64,75,87,
		100,114,128,144,160,177,195,214,
		233,253,274,296,319,342,366,391,
		417,443,470,498,526,555,585,615,
		646,678,710,743,776,810,844,879,
		915,951,987,1024,1061,1099,1137,1176,
		1215,1254,1294,1334,1374,1415,1456,1497,
		1539,1580,1622,1664,1706,1749,1791,1834,
		1877,1919,1962,2005,2048
};