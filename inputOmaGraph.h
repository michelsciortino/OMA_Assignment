#ifndef INPUTOMAGRAPH_H_INCLUDED
#define INPUTOMAGRAPH_H_INCLUDED
#include "Graph.h"
// class containing the data of a timetabling problem
class Problem {

public:
    int nStudents;
    int nExams;
    int nSlots;
    // matrix containing in place i,j the number of students
    // enrolled to both i and j
    int ** conflicts;

    void loadFile(char * instance, Graph &g);
};


#endif // INPUTOMAGRAPH_H_INCLUDED
