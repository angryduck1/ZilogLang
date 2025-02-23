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
			auto comment_pos = line.find("//");
			auto space = line.find('"');

			if (comment_pos != string::npos) {
				line = line.substr(0, comment_pos);
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

	int check_opr(const string &value_str, bool is_create) {
		if (value_str == "opr") {
			string operation;
			if (is_create) {
				operation = pc[counter + 5];
				counter += 3;
			}
			else {
				operation = pc[counter + 4];
				counter += 2;
			}
			string end_operation;
			unsigned i = 3;
			bool is_end = false;
			int result_number = -2147483646;
			double result_swim = -2147483646;
			string f_o = pc[++counter];
			string t_o = pc[counter += 2];

			while (is_end != true) {
				if (operation == "+") {
					if (number.count(f_o) && number.count(t_o) && result_number == -2147483646) {
						result_number = number[f_o] + number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] + swim[t_o];
						set_variable(var, result_swim);
					}
					else if (number.count(t_o) && result_number != -2147483646) {
						result_number = result_number + number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(t_o) && result_swim != -2147483646) {
						result_swim = result_swim + swim[t_o];
						set_variable(var, result_swim);
					}

					else if (number.count(f_o) && result_number == -2147483646) {
						result_number = number[f_o] + stod(t_o);
						set_variable(var, result_number);
					}
					else if (number.count(t_o) && result_number == -2147483646) {
						result_number = number[t_o] + stod(f_o);
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] + stod(t_o);
						set_variable(var, result_swim);
					}
					else if (swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[t_o] + stod(f_o);
						set_variable(var, result_swim);
					}

					else if (result_number == -2147483646 && result_swim == -2147483646) {
						try {
							double val_fo = stod(f_o);
							double val_to = stod(t_o);

							double result = val_fo + val_to;

							if (type == "number") {
								result_number = static_cast<int>(result);
								set_variable(var, result_number);
							}
							else if (type == "swim") {
								result_swim = result;
								set_variable(var, result_swim);
							}
							else {
								cerr << "Error in line " << current_line << ": Using incompatible types." << endl;
								return -1;
							}

						}
						catch (const invalid_argument& e) {
							cerr << "Error in line " << current_line << ": Invalid value for operation." << endl;
							return -1;
						}
						catch (const out_of_range& e) {
							cerr << "Error in line " << current_line << ": Value out of range." << endl;
							return -1;
						}
					}
					else if (result_number != -2147483646) {
						try {
							result_number = result_number + stod(t_o);
							set_variable(var, result_number);
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
					else if (result_swim != -2147483646) {
						try {
							result_swim = result_swim + stod(t_o);
							set_variable(var, result_swim);
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

				}
				else if (operation == "-") {
					if (number.count(f_o) && number.count(t_o) && result_number == -2147483646) {
						result_number = number[f_o] - number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] - swim[t_o];
						set_variable(var, result_swim);
					}
					else if (number.count(t_o) && result_number != -2147483646) {
						result_number = result_number - number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(t_o) && result_swim != -2147483646) {
						result_swim = result_swim - swim[t_o];
						set_variable(var, result_swim);
					}

					else if (number.count(f_o) && result_number == -2147483646) {
						result_number = number[f_o] - stod(t_o);
						set_variable(var, result_number);
					}
					else if (number.count(t_o) && result_number == -2147483646) {
						result_number = number[t_o] - stod(f_o);
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] - stod(t_o);
						set_variable(var, result_swim);
					}
					else if (swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[t_o] - stod(f_o);
						set_variable(var, result_swim);
					}

					else if (result_number == -2147483646 && result_swim == -2147483646) {
						try {
							double val_fo = stod(f_o);
							double val_to = stod(t_o);

							double result = val_fo - val_to;

							if (type == "number") {
								result_number = static_cast<int>(result);
								set_variable(var, result_number);
							}
							else if (type == "swim") {
								result_swim = result;
								set_variable(var, result_swim);
							}
							else {
								cerr << "Error in line " << current_line << ": Using incompatible types." << endl;
								return -1;
							}

						}
						catch (const invalid_argument& e) {
							cerr << "Error in line " << current_line << ": Invalid value for operation." << endl;
							return -1;
						}
						catch (const out_of_range& e) {
							cerr << "Error in line " << current_line << ": Value out of range." << endl;
							return -1;
						}
					}
					else if (result_number != -2147483646) {
						try {
							result_number = result_number - stod(t_o);
							set_variable(var, result_number);
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
					else if (result_swim != -2147483646) {
						try {
							result_swim = result_swim - stod(t_o);
							set_variable(var, result_swim);
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

				}
				else if (operation == "*") {
					if (number.count(f_o) && number.count(t_o) && result_number == -2147483646) {
						result_number = number[f_o] * number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] * swim[t_o];
						set_variable(var, result_swim);
					}
					else if (number.count(t_o) && result_number != -2147483646) {
						result_number = result_number * number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(t_o) && result_swim != -2147483646) {
						result_swim = result_swim * swim[t_o];
						set_variable(var, result_swim);
					}

					else if (number.count(f_o) && result_number == -2147483646) {
						result_number = number[f_o] * stod(t_o);
						set_variable(var, result_number);
					}
					else if (number.count(t_o) && result_number == -2147483646) {
						result_number = number[t_o] * stod(f_o);
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] * stod(t_o);
						set_variable(var, result_swim);
					}
					else if (swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[t_o] * stod(f_o);
						set_variable(var, result_swim);
					}

					else if (result_number == -2147483646 && result_swim == -2147483646) {
						try {
							double val_fo = stod(f_o);
							double val_to = stod(t_o);

							double result = val_fo * val_to;

							if (type == "number") {
								result_number = static_cast<int>(result);
								set_variable(var, result_number);
							}
							else if (type == "swim") {
								result_swim = result;
								set_variable(var, result_swim);
							}
							else {
								cerr << "Error in line " << current_line << ": Using incompatible types." << endl;
								return -1;
							}

						}
						catch (const invalid_argument& e) {
							cerr << "Error in line " << current_line << ": Invalid value for operation." << endl;
							return -1;
						}
						catch (const out_of_range& e) {
							cerr << "Error in line " << current_line << ": Value out of range." << endl;
							return -1;
						}
					}
					else if (result_number != -2147483646) {
						try {
							result_number = result_number * stod(t_o);
							set_variable(var, result_number);
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
					else if (result_swim != -2147483646) {
						try {
							result_swim = result_swim * stod(t_o);
							set_variable(var, result_swim);
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

				}
				else if (operation == "/") {
					if (number.count(f_o) && number.count(t_o) && result_number == -2147483646) {
						result_number = number[f_o] / number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] / swim[t_o];
						set_variable(var, result_swim);
					}
					else if (number.count(t_o) && result_number != -2147483646) {
						result_number = result_number / number[t_o];
						set_variable(var, result_number);
					}
					else if (swim.count(t_o) && result_swim != -2147483646) {
						result_swim = result_swim / swim[t_o];
						set_variable(var, result_swim);
					}

					else if (number.count(f_o) && result_number == -2147483646) {
						result_number = number[f_o] / stod(t_o);
						set_variable(var, result_number);
					}
					else if (number.count(t_o) && result_number == -2147483646) {
						result_number = number[t_o] / stod(f_o);
						set_variable(var, result_number);
					}
					else if (swim.count(f_o) && result_swim == -2147483646) {
						result_swim = swim[f_o] / stod(t_o);
						set_variable(var, result_swim);
					}
					else if (swim.count(t_o) && result_swim == -2147483646) {
						result_swim = swim[t_o] / stod(f_o);
						set_variable(var, result_swim);
					}

					else if (result_number == -2147483646 && result_swim == -2147483646) {
						try {
							double val_fo = stod(f_o);
							double val_to = stod(t_o);

							double result = val_fo / val_to;

							if (type == "number") {
								result_number = static_cast<int>(result);
								set_variable(var, result_number);
							}
							else if (type == "swim") {
								result_swim = result;
								set_variable(var, result_swim);
							}
							else {
								cerr << "Error in line " << current_line << ": Using incompatible types." << endl;
								return -1;
							}

						}
						catch (const invalid_argument& e) {
							cerr << "Error in line " << current_line << ": Invalid value for operation." << endl;
							return -1;
						}
						catch (const out_of_range& e) {
							cerr << "Error in line " << current_line << ": Value out of range." << endl;
							return -1;
						}
					}
					else if (result_number != -2147483646) {
						try {
							result_number = result_number / stod(t_o);
							set_variable(var, result_number);
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
					else if (result_swim != -2147483646) {
						try {
							result_swim = result_swim / stod(t_o);
							set_variable(var, result_swim);
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

				}

				operation = pc[counter + 1];
				t_o = pc[counter + 2];
				counter += 2;
				i += 2;

				if (pc[counter - 1] == "end") {
					is_end = true;
				}
			}

			if (counter + 1 >= c) {
				if (is_create) {
					typ[var] = type;
				}
				return 2;
			}
			else {
				if (is_create) {
					typ[var] = type;
				}
				return 0;
			}
		}

		return 3;
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

					if (check_opr(value_str, 1) == 0) {
						continue;
					} else if (check_opr(value_str, 1) == -1) {
						break;
					}
					else if (check_opr(value_str, 1) == 2) {
						break;
					}
					else if (check_opr(value_str, 1) == 3) {
						// Next
					}

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
					string op = token.substr(0, 2);
					bool is_active = false;
					bool is_endl = false;

					if (token == "endl") {
						cout << endl;
						is_endl = true;
					}

					if (!is_endl) {
						if (op == "++" || op == "--") {
							if (op == "++") {
								token = token.substr(2);
								if (number.count(token)) {
									number[token]++;
									set_variable(var, number[token]);
								}
								else if (swim.count(token)) {
									swim[token]++;
									set_variable(var, swim[token]);
								}
							}
							else {
								token = token.substr(2);
								if (number.count(token)) {
									number[token]--;
									set_variable(var, number[token]);
								}
								else if (swim.count(token)) {
									swim[token]--;
									set_variable(var, swim[token]);
								}
							}
						}

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

						else if (is_active == false) {
							if (number.count(token)) {
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
						}

						if (counter > c) {
							cerr << "Error in line " << current_line << ": Expected 'end' after variable names in display command." << endl;
							return -1;
						}
					}
					counter++;
				}

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


					if (check_opr(value_str, 0) == 0) {
						continue;
					}
					else if (check_opr(value_str, 0) == -1) {
						break;
					}
					else if (check_opr(value_str, 0) == 2) {
						break;
					}
					else if (check_opr(value_str, 0) == 3) {
						// Next
					}

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
			else if (next_line == "in") {
				var = pc[++counter];

				try {
					if (typ[var] == "number") {
						int result;
						cin >> result;
						number[var] = result;
					}
					else if (typ[var] == "swim") {
						double result;
						cin >> result;
						swim[var] = result;
					}
					else if (typ[var] == "raw") {
						string result;
						cin >> result;
						raw[var] = result;
					}
					else if (typ[var] == "con") {
						bool result;
						cin >> result;
						con[var] = result;
					}
				} 
				catch (const invalid_argument&) {
					cerr << "Error in line " << current_line << endl;
					return -1;
				}
				catch (const out_of_range&) {
					cerr << "Error in line " << current_line << ": Value out of range for variable '" << var << "'"
						<< endl;
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
