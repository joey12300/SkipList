#include "skiplist.h"
#include <iostream>
using namespace std;
int main() {
	SkipList<int> sl;
	for(int i = 1; i < 10; ++i)
		sl.insert(i);
	cout << sl << endl;

	for (int i = 1; i < 20; i += 2) {
		if (sl.contain(i)) {
			cout << "found" << endl;
		}
		else {
			cout << "not found" << endl;
		}
	}

	for (int i = 1; i < 9; ++i) {
		sl.remove(i);
		cout << "After delete " << i << endl;
		cout << sl << endl;
	}
		
	return 0;
}