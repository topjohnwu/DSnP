#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <set>
#define EMPTY 1024
using namespace std;

class Data {
public:
	static int col_num;
	const int operator[] (size_t i) const {return _cols[i];}
	int& operator[] (size_t i) {return _cols[i];}
	Data() {
		_cols = new int[col_num];
		for(int i = 0; i < col_num; i++) _cols[i] = EMPTY;
	}
	Data(const Data &d) {
		_cols = new int[col_num];
		for(int i = 0; i < col_num; i++) _cols[i] = d[i];
	}
	~Data() {delete [] _cols;}
private:
	int *_cols;
};

struct SortData {
	bool operator() (const Data& d1, const Data& d2) {
		for(int i = 0; i < _sortOrder.size(); i++) {
			if(d1[_sortOrder[i]] < d2[_sortOrder[i]]) return true;
			else if(d1[_sortOrder[i]] == d2[_sortOrder[i]]) continue;
			else return false;
		}
		return false;
	}
	void pushOrder(size_t i) {
		_sortOrder.push_back(i);
	}
	void clear_all() {
		_sortOrder.clear();
	}
	vector<size_t> _sortOrder;
} SortData;

int Data::col_num = 0;

void print(const vector<Data>& table) {
	for(int row = 0; row < table.size(); row++) {
		for(int col = 0; col < Data::col_num; col++) {
			if(table[row][col] == EMPTY) cout << setw(4) << right << "";
			else cout << setw(4) << right << table[row][col];
		}
		cout << endl;
	}
}

void add(vector<Data>& table) {
	table.push_back(Data());
	string add;
	for(int col = 0; col < Data::col_num; col++) {
		cin >> add;
		if(add == "-") table.back()[col] = EMPTY;
		else table.back()[col] = stoi(add);
	}
}

void sum(const vector<Data>& table) {
	int col, sum = 0;
	cin >> col;
	for(int i = 0; i < table.size(); i++) {
		if(table[i][col] != EMPTY) {
			sum += table[i][col];
		}
	}
	cout << "The summation of data in column #" << col << " is " << sum << "." << endl;
}

void ave(const vector<Data>& table) {
	int col, num= 0;
	double ave = 0;
	cin >> col;
	for(int i = 0; i < table.size(); i++) {
		if(table[i][col] != EMPTY) {
			ave += table[i][col];
			num++;
		}
	}
	ave /= num;
	cout << "The average of data in column #" << col << " is " << ave << "." << endl;
}
void max(const vector<Data>& table) {
	int col;
	vector<int> table_col;
	cin >> col;
	for(int i = 0; i < table.size(); i++) {
		if(table[i][col] != EMPTY) {
			table_col.push_back(table[i][col]);
		}
	}
	cout << "The maximum of data in column #" << col << " is " << *max_element(table_col.begin(), table_col.end()) << "." << endl;
}
void min(const vector<Data>& table) {
	int col;
	vector<int> table_col;
	cin >> col;
	for(int i = 0; i < table.size(); i++) {
		if(table[i][col] != EMPTY) {
			table_col.push_back(table[i][col]);
		}
	}
	cout << "The minimum of data in column #" << col << " is " << *min_element(table_col.begin(), table_col.end()) << "." << endl;
}
void count(const vector<Data>& table) {
	int col;
	set<int> uniq_col;
	cin >> col;
	for(int i = 0; i < table.size(); i++) {
		if(table[i][col] != EMPTY) {
			uniq_col.insert(table[i][col]);
		}
	}
	cout << "The distinct count of data in column #" << col << " is " << uniq_col.size() << "." << endl;
}
void sort(vector<Data>& table) {
	int a;
	SortData.clear_all();
	while(1) {
		cin >> a;
		SortData.pushOrder(a);
		if(cin.peek() == '\n') break;
	}
	sort(table.begin(), table.end(), SortData);
}



int main() {
	string filename;
	cout << "Please enter the file name: ";
	cin >> filename;
	ifstream file(filename.c_str(), ios::in);
	int i, j;
	cout << "Please enter the number of rows and columns: ";
	cin >> i >> j;
	Data::col_num = j;
	vector<Data> table(i);
	for(int row = 0; row < i; row++) {
		for(int col = 0; col < j;) {
			if(file.peek() == ' ') file.ignore();
			else if(file.peek() == ',') {
				file.ignore();
				col++;
			}
			else if(file.peek() == '\r' || file.peek() == '\n' || file.peek() == EOF) {
				file.ignore();
				break;
			}
			else file >> table[row][col];
		}
	}
	cout << "File \"" << filename << "\" was read in successfully." << endl;
	file.close();
	while(1) {
		string input;
		cin >> input;
		transform(input.begin(), input.end(), input.begin(), ::toupper);
		if(input == "PRINT") print(table);
		else if(input == "ADD") add(table);
		else if(input == "SUM") sum(table);
		else if(input == "AVE") ave(table);
		else if(input == "MAX") max(table);
		else if(input == "MIN") min(table);
		else if(input == "COUNT") count(table);
		else if(input == "SORT") sort(table);
		else cout << "Insert is invalid!" << endl;
	}
}