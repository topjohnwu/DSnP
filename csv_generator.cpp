#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cassert>
using namespace std;

/****************************************************************************
This program guarantees all numbers are distributed uniformly
The random size is expanded to twice the size of RAND_MAX
Set your own values below
ROW and COLUMN determines the number of rows and columns of the output .csv file
MAX and MIN determines the maximum and minimum value appears in the .csv
NULL_SPACE determines the chance of null to appear, as the number became larger, null appears more likely
SIZE, HALF_SIZE, and OFFSET are calculated constants, please don't move them
** Note that HALF_SIZE must be smaller or equals to RAND_MAX + 1(likely = 2147483648)
OUTPUT_FILENAME determines the output file name
****************************************************************************/

static const long ROW = 10000;
static const long COLUMN = 10;
static const long MAX = 100;
static const long MIN = -99;
static const long NULL_SPACE = 50;
static const char OUTPUT_FILENAME[] = "test.csv";

//Don't move below
static const long SIZE = (MAX - MIN + NULL_SPACE + 1);
static const long HALF_SIZE = (SIZE / 2) + 1;
static const long OFFSET = HALF_SIZE - 1 + MIN;
//Don't move above

int main() {
	assert(HALF_SIZE <= long(RAND_MAX) + 1);
	srand(time(0));
	ofstream out(OUTPUT_FILENAME, ios::out);
	for(long j = 0; j < ROW; j++) {
		for(long i = 0; i < COLUMN; i++) {
			if(i) out << ',';
			long n;
			while(1) {
				n = rand() % HALF_SIZE;
				if(rand() % 2) {
					if(n) n = 0 - n;
					else continue;
				}
				n += OFFSET;
				if(n > MAX + NULL_SPACE) continue;
				else break;
			}
			if(n <= MAX) out << n;
		}
		out << '\r';
	}
	out << '\r' << '\r';
}