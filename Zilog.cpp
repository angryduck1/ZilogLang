#include <iostream>
#include <locale>
#include "Core.h"

using namespace std;

int main(int argc, char* argv[]) {
	string path;
	if (argc > 1) {
		path = argv[1];
	}
	else {
		path = "main.zil";
	}
	Interpritator zl(path);
}