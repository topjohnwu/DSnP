#include <iostream>
#include <fstream>
#include <string>

using namespace std;
int main(int argc, char *argv[]) {
	string file = "test1.aag", dummy;
	int size, out, level = 100;
	file = "tests.fraig/" + file;
	ifstream in(file.c_str());
	in >> dummy >> size >> out >> out >> out;
	in.close();
	ofstream o("mydo");
	o << "cirr " << file << endl
	  << "cirp" << endl
	  << "cirp -n" << endl
	  << "cirp -pi" << endl
	  << "cirp -po" << endl
	  << "cirp -fl" << endl;
	for(int i = 0; i <= size + out; ++i) {
	o << "cirg " << i << endl
	  << "cirg " << i << " -fani " << level << endl
	  << "cirg " << i << " -fano " << level << endl;
	}
	o << "cirw" << endl;
	o << "q -f" << endl;
}