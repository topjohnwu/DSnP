#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
int main(int argc, char *argv[]) {
	ofstream o("mydo");
	for(int i = 12; i <= 12; ++i) {
		if(i == 13) continue;
		string file, dummy, location;
		stringstream ss;
		int size, out, level = 100;
		if(i < 10) ss << "sim0" << i << ".aag";
		else ss << "sim" << i << ".aag";
		file = ss.str();
		location = "tests.fraig/" + file;
		ifstream in(location.c_str());
		in >> dummy >> size >> out >> out >> out;
		in.close();
		o << "cirr -r " << location << endl
		  << "cirp" << endl
	  	  << "cirp -n" << endl
	  	  << "cirp -pi" << endl
	  	  << "cirp -po" << endl
		  << "cirp -fl" << endl;
		for(int j = 0; j <= size + out; ++j) {
			o << "cirg " << j << endl
			  << "cirg " << j << " -fani " << level << endl
			  << "cirg " << j << " -fano " << level << endl;
		}
		o << "cirw" << endl;
	}
	o << "q -f" << endl;
}