#ifndef HUNGARIAN_H_INCLUDED
#define HUNGARIAN_H_INCLUDED

#include <iostream>
#include <limits.h>
#include <string>
#include <stdlib.h>
#include <vector>

using namespace std;

struct cell{
	int weight;
	bool lined, visible;
	//initialisation
	cell() {
		lined = false;
		visible = true;
		weight = -1;
	}
};

void doHungarian(int n, vector<vector<int> > &matrix, vector<int> &matching);

#endif // HUNGARIAN_H_INCLUDED
