#include <iostream>
#include <random>

using namespace std;

int main(void) {
    random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, 10);
	cout << distrib(gen) << endl;
}
