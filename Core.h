#pragma once
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <type_traits>

using namespace std;

class Interpritator {
public:
	Interpritator() {
		ifstream inFile("main.zil");

		string line;
		string code;

		while (getline(inFile, line)) {
			auto comment_pos = line.find(";");

			if (comment_pos != string::npos) {
				line = line.substr(0, comment_pos);
				code += line + "\n";
			}
			else {
				code += line + "\n";
			}
		}

		inFile.close();

		istringstream stream(code);
		string commands;

		while (stream >> commands) {
			pc[c] = commands;
			++c;
		}

		if (compiling() != 0) {
			cerr << "Compilation failed!" << endl;
		}

	}

	void print(const double& var) {
		cout << var << endl;
	}
private:
	map<string, int> number;
	map<string, double> swim;
	map<string, string> raw;
	map<string, bool> con;
	map<unsigned, string> pc;
	string type;
	string var;
	unsigned c = 0;
	unsigned counter = 0;
	unsigned current_line = 0;


	template<typename T>
	void set_variable(const string var, T value) {
		if constexpr (is_same_v<T, int>) {
			number[var] = value;
		}
		else if constexpr (is_same_v<T, double>) {
			swim[var] = value;
		}
		else if constexpr (is_same_v<T, string>) {
			raw[var] = value;
		}
		else if constexpr (is_same_v<T, bool>) {
			con[var] = value;
		}
		else {
			cerr << "Unknown error in line " << current_line << endl;
		}
	}

	int compiling() {
		while (counter < c) {
			++current_line;
			string next_line = pc[counter];

			if ((next_line == "number" || next_line == "swim" || next_line == "raw" || next_line == "bool")) {
				type = next_line;
				var = pc[counter + 1];

				if (counter + 2 < c && pc[counter + 2] == "=") {
					string value_str = pc[counter + 3];

					try {
						if (type == "number") {
							int value = stoi(value_str);
							set_variable(var, value);
						}
						else if (type == "swim") {
							double value = stod(value_str);
							set_variable(var, value);
						}
						else if (type == "raw") {
							set_variable(var, value_str);
						}
						else if (type == "bool") {
							bool value = (value_str == "true" || value_str == "1");
							set_variable(var, value);
						}
						else {
							cerr << "Error in line " << current_line << ": Unknown type '" << type << "'" << endl;
							return -1;
						}

					}
					catch (const invalid_argument&) {
						cerr << "Error in line " << current_line << ": Invalid value '" << value_str << "' for type '"
							<< type << "'" << endl;
						return -1;
					}
					catch (const out_of_range&) {
						cerr << "Error in line " << current_line << ": Value out of range for variable '" << var << "'"
							<< endl;
						return -1;
					}

					counter += 4;
					continue;
				}
				else {
					cerr << "Error in line " << current_line << ": Expected '=' after variable declaration." << endl;
					return -1;
				}
			}


			if (next_line == "display") {
				string var_name = pc[counter + 1];
				if (swim.count(var_name)) {
					print(swim[var_name]);
				}
				else {
					cerr << "Error in line " << current_line << ": Variable '" << var_name << "' not found." << endl;
					return -1;
				}
				counter += 2;
				continue;
			}

			counter++;

		}

		return 0;
	}
};
