#include "functions.hpp"

vector<string> separateValues(string input, char label) {
	stringstream inputstream(input);
	string value;
	vector<string> values;
	while (getline(inputstream, value, label)) {
		values.push_back(value);
	}
	return values;
}

string zeroPadding(string input, int number) {
	int input_size = input.size();
	int zero_size = number - input_size;
	string output = "";
	int i = 0;
	while (i < zero_size) {
		output += "0";
		i++;
	}
	output += input;
	return output;
}

string FileToString(string file_name) {
	string file_data;
	ifstream file(file_name);
	string line;
	while (getline(file, line)) {
		file_data = file_data + line + "\n";
	}
	return file_data;
}

void StringToFile(string file_name, string file_data) {
	ofstream file;
	file.open(file_name);
	file << file_data;
	file.close();
}
