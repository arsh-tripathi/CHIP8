#include <iostream>
#include <fstream>
// #include <random>

using namespace std;

int main(int argc, char *argv[]) {
//     random_device rd;
// 	mt19937 gen(rd());
// 	uniform_int_distribution<> distrib(0, 10);
// 	cout << distrib(gen) << endl;
	if (argc != 2) exit(1);
	ifstream in{argv[1]};
	ofstream out{"instructions.txt"};
	uint8_t value;
	if (!in.is_open()) cout << "The file couldn't be opened";
	out << hex;
	while (in >> value) {
		out << +value << endl;
	}
}
