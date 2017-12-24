#ifndef GraphIncluded
#define GraphIncluded

class Graph {

public:

    //constructors
	Graph();
	Graph(int n);
	~Graph();

	//methods
	void resize(int n);
	int * operator[](int index);
	void makeAdjList(int **neighbors);


    //variables
	int *matrix;
	int n;        // number of nodes
	int nbEdges;  // number of edges
};
#endif
