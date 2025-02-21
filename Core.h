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
			auto space = line.find('"');

			if (comment_pos != string::npos) {
				line = line.substr(0, comment_pos);
				code += line + "\n";
			}

			while (space != string::npos) {
				auto next_space_pos = line.find('"', space + 1);

				if (next_space_pos != string::npos) {
					for (size_t i = space + 1; i < next_space_pos; ++i) {
						if (line[i] == ' ') {
							line[i] = '\\';
						}
					}
					space = line.find('"', next_space_pos + 1);
				}
				else {
					break;
				}
			}

			code += line + "\n";
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
	map<string, string> typ;
	map<unsigned, string> pc;
	string var;
	string type;
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

			if ((next_line == "number" || next_line == "swim" || next_line == "raw" || next_line == "con")) {
				type = next_line;
				var = pc[counter + 1];

				if (counter + 2 < c && pc[counter + 2] == "=") {
					string value_str = pc[counter + 3];
					bool is_active = false;

					try {
						if (number.count(value_str) && typ[value_str] == type) {
							set_variable(var, number[value_str]);
							is_active = true;
						}
						else if (swim.count(value_str) && typ[value_str] == type) {
							set_variable(var, swim[value_str]);
							is_active = true;
						}
						else if (raw.count(value_str) && typ[value_str] == type) {
							set_variable(var, raw[value_str]);
							is_active = true;
						}
						else if (con.count(value_str) && typ[value_str] == type) {
							set_variable(var, con[value_str]);
							is_active = true;
						}

						if (type == "number" && is_active == false) {
							int value = stoi(value_str);
							set_variable(var, value);
							typ[var] = type;
						}
						else if (type == "swim" && is_active == false) {
							double value = stod(value_str);
							set_variable(var, value);
						}
						else if (type == "raw" && value_str.front() == '"' && value_str.back() == '"' && is_active == false) {
							int i = 0;
							for (auto ch : value_str) {
								if (ch == '\\') {
									value_str[i] = ' ';
								}
								i++;
							}
							set_variable(var, value_str.substr(1, value_str.length() - 2));
						}
						else if (type == "con" && is_active == false) {
							if (value_str != "true" && value_str != "false" && value_str != "1" && value_str != "0") {
								cerr << "Error in line " << current_line << ": con must be true (1) or false (0). Your value '" << value_str << "'" << endl;
								return -1;
							}
							bool value = (value_str == "1") || (value_str == "true");
							set_variable(var, value);
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
					typ[var] = type;
					continue;
				}
				else {
					cerr << "Error in line " << current_line << endl;
					return -1;
				}
			}

			else if (next_line == "display") {
				counter++;
				while (pc[counter] != "end") {
					string token = pc[counter];

					if (token.front() == '"' && token.back() == '"') {
						int i = 0;
						for (auto ch : token) {
							if (ch == '\\') {
								token[i] = ' ';
							}
							i++;
						}
						cout << token.substr(1, token.length() - 2);
					}
					else if (number.count(token)) {
						cout << number[token];
					}
					else if (swim.count(token)) {
						cout << swim[token];
					}
					else if (raw.count(token)) {
						cout << raw[token];
					}
					else if (con.count(token)) {
						cout << con[token];
					}
					else {
						try {
							cout << stod(token);
						}
						catch (const invalid_argument&) {
							cerr << "Error in line " << current_line << ": Variable '" << token << "' not found." << endl;
							return -1;
						}
					}

					if (counter > c) {
						cerr << "Error in line " << current_line << ": Expected 'end' after variable names in display command." << endl;
						return -1;
					}
					counter++;
				}
				cout << endl;

				if (counter + 1 > c) {
					return 0;
				}
				else {
					counter++;
				}
			}

			else if (number.count(next_line) || swim.count(next_line) || raw.count(next_line) || con.count(next_line)) {
				var = pc[counter];

				if (pc[counter + 1] == "=") {
					string value_str = pc[counter + 2];
					string op = value_str.substr(0, 2);
					bool is_active = false;

					try {
						if (number.count(value_str) && typ[value_str] == typ[var]) {
							set_variable(var, number[value_str]);
							is_active = true;
						}
						else if (swim.count(value_str) && typ[value_str] == typ[var]) {
							set_variable(var, swim[value_str]);
							is_active = true;
						}

						else if (raw.count(value_str) && typ[value_str] == typ[var]) {
							set_variable(var, raw[value_str]);
							is_active = true;
						}
						else if (con.count(value_str) && typ[value_str] == typ[var]) {
							set_variable(var, con[value_str]);
							is_active = true;
						}

						if (typ[var] == "number" && op != "++" && op != "--" && is_active == false) {
							int value = stoi(value_str);
							set_variable(var, value);
						}
						else if (typ[var] == "swim" && value_str.substr(0, 2) != "++" && value_str.substr(0, 2) != "--" && is_active == false) {
							double value = stod(value_str);
							set_variable(var, value);
						}
						else if (typ[var] == "raw" && value_str.front() == '"' && value_str.back() == '"' && is_active == false) {
							int i = 0;
							for (auto ch : value_str) {
								if (ch == '\\') {
									value_str[i] = ' ';
								}
								i++;
							}
							set_variable(var, value_str.substr(1, value_str.length() - 2));
						}
						else if (typ[var] == "con" && is_active == false) {
							if (value_str != "true" && value_str != "false" && value_str != "1" && value_str != "0") {
								cerr << "Error in line " << current_line << ": con must be true (1) or false (0). Your value '" << value_str << "'" << endl;
								return -1;
							}
							bool value = (value_str == "1") || (value_str == "true");
							set_variable(var, value);
						}
						else {
							if (op == "++" || op == "--") {
								if (op == "++") {
									value_str = value_str.substr(2);
									if (number.count(value_str)) {
										number[var]++;
										set_variable(var, number[var]);
									}
									else if (swim.count(value_str)) {
										swim[var]++;
										set_variable(var, swim[var]);
									}
								}
								else {
									value_str = value_str.substr(2);
									if (number.count(value_str)) {
										number[var]--;
										set_variable(var, number[var]);
									}
									else if (swim.count(value_str)) {
										swim[var]--;
										set_variable(var, swim[var]);
									}
								}
							}

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
				}

				counter += 3;
				continue;
			}
			else if (next_line.substr(0, 2) == "++" || next_line.substr(0, 2) == "--") {
				string op = next_line.substr(0, 2);
				string var_name = next_line.substr(2);

				if (number.count(var_name)) {
					if (op == "++") {
						number[var_name]++;
					}
					else {
						number[var_name]--;
					}
					set_variable(var_name, number[var_name]);
				}
				else if (swim.count(var_name)) {
					if (op == "++") {
						swim[var_name]++;
					}
					else {
						swim[var_name]--;
					}
					set_variable(var_name, swim[var_name]);
				}
				else {
					cerr << "Error in line " << current_line << ": Variable '" << var_name << "' not found." << endl;
					return -1;
				}
				counter++;
			}

			else {
				cerr << "Error in line " << current_line << endl;
				return -1;
			}
		}
		return 0;
	}
};
