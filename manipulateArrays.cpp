#include "manipulateArrays.h"

extern unsigned long long numConfChecks;

void initializeArrays(int ** & nodesByTimeSlot, int ** & conflicts, int ** & tabuStatus, int * & nbcPosition, Graph & g, vector<int> &t, int k) {

	int n=g.n;

	// Allocate and initialize (k+1)x(n+1) array for nodesByTimeSlot and conflicts
	nodesByTimeSlot = new int*[k+1];
	conflicts = new int*[k+1];
	for (int i=0; i<=k; i++) {
		nodesByTimeSlot[i] = new int[n+1];
		nodesByTimeSlot[i][0] = 0;
		conflicts[i] = new int[n+1];
		for (int j=0; j<=n; j++) {
			conflicts[i][j]=0;
		}
	}

	// Allocate the tabuStatus array
	tabuStatus = new int*[n];
	for (int i=0; i<n; i++) {
		tabuStatus[i]= new int[k+1];
		for (int j=0; j<=k; j++) {
			tabuStatus[i][j] = 0;
		}
	}

	// Allocate the nbcPositions array
	nbcPosition = new int[n];

	// Initialize the nodesByTimeSlot and nbcPosition array
	for (int i=0; i<n; i++) {
		// C is cool ;-)
		nodesByTimeSlot[ t[i] ][ (nbcPosition[i] = ++nodesByTimeSlot[t[i]][0]) ] = i;
	}

	// Initialize the conflicts and neighbors array.
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			numConfChecks++;
			if (g[i][j] && i!=j) {
				conflicts[t[j]][i]++;
			}
		}
	}
}

void moveNodeToAssignForTabu(int bestNode, int bestTimeSlot, Graph & g, vector<int> &t, int ** nodesByTimeSlot, int ** conflicts, int * nbcPosition, int ** neighbors,
	int * nodesInConflict, int * confPosition, int ** tabuStatus,  long totalIterations, int tabuTenure)
{
	int oldTimeSlot = t[bestNode];
	// move bestNodes to bestTimeSlot
	t[bestNode] = bestTimeSlot;

	// If bestNode is not a conflict node anymore, remove it from the list
	numConfChecks+=2;
	if (conflicts[oldTimeSlot][bestNode] && !(conflicts[bestTimeSlot][bestNode])) {
		confPosition[nodesInConflict[nodesInConflict[0]]] = confPosition[bestNode];
		nodesInConflict[confPosition[bestNode]] = nodesInConflict[nodesInConflict[0]--];
	}
	else {  // If bestNode becomes a conflict node, add it to the list
		numConfChecks+=2;
		if (!(conflicts[oldTimeSlot][bestNode]) && conflicts[bestTimeSlot][bestNode]) {
			nodesInConflict[ (confPosition[bestNode] = ++nodesInConflict[0]) ] = bestNode;
		}
	}

	// Update the conflicts of the neighbors
	numConfChecks++;
	for (int i=1; i<=neighbors[bestNode][0]; i++) {
		int nb = neighbors[bestNode][i];
		numConfChecks+=2;
		// Decrease the number of conflicts in the old timeslot
		if ((--conflicts[oldTimeSlot][nb]) == 0 && t[nb] == oldTimeSlot) {
			// Remove nb from the list of conflicting nodes if there are 0 conflicts in
			// its own timeslot
			confPosition[nodesInConflict[nodesInConflict[0]]] = confPosition[nb];
			nodesInConflict[confPosition[nb]] = nodesInConflict[nodesInConflict[0]--];
		}
		// Increase the number of conflicts in the new timeslot
		numConfChecks++;
		if ((++conflicts[bestTimeSlot][nb]) == 1 && t[nb] == bestTimeSlot) {
			// Add nb from the list conflicting nodes if there is a new conflict in
			// its own timeslot
			nodesInConflict[ (confPosition[nb] = ++nodesInConflict[0]) ] = nb;
		}
	}
	// Set the tabu status
	tabuStatus[bestNode][oldTimeSlot] = totalIterations + tabuTenure;
}

void freeArrays(int ** & nodesByTimeSlot, int ** & conflicts, int ** & tabuStatus, int * & nbcPosition, int k, int n)
{
	for (int i=0; i<=k; i++) {
		delete[] nodesByTimeSlot[i];
		delete[] conflicts[i];
	}
	for (int i=0; i<n; i++) {
		delete[] tabuStatus[i];
	}
	delete[] nodesByTimeSlot;
	delete[] conflicts;
	delete[] tabuStatus;
	delete[] nbcPosition;
}
