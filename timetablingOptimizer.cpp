#include "Graph.h"
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "inputOmaGraph.h"
#include "timetablingOptimizer.h"
#include <list>
#include<vector>
#include <math.h>


//-------------------------------
/* methods of the class MATRIX */
Matrix::Matrix() {
    rows = 0;
    cols = 0;
    //entries = NULL;
}
Matrix::Matrix(int rows_, int columns_) {
    rows = rows_;
    cols = columns_;
    //entries.resize(rows, std::vector<int>(cols,0));
    entries = new int [rows*cols];
    for (int i=0; i<rows*cols; i++) {
        entries[i] = 0;
    }

}
int * Matrix::operator[](int i) {
    return entries + cols*i;
}

//----------------------------------
/* methods of the class OPTIMIZER */
Optimizer::Optimizer(Problem data_, std::vector<int> initialSol) {
// initialize the optimizer
    //data = data_;
    data.nStudents = data_.nStudents;
    data.nExams = data_.nExams;
    data.nSlots = data_.nSlots;
    data.conflicts = data_.conflicts;

    currentSol = initialSol;
    bestSol = currentSol;

    iteration = 0;
    temperature = 100000000;

    studentsForSlot = Matrix(data.nExams, data.nSlots);
    tabuList = Matrix(data.nExams, data.nSlots);

    slotOfGroup.resize(data.nSlots);
    groupOfSlot.resize(data.nSlots);
    for (int i=0; i<data.nSlots; i++){
        slotOfGroup[i] = i;
        groupOfSlot[i] = i;
    }

    // initialize studentsForSlot
    int commonStudents;
    for (int examI=0; examI<data.nExams-1; examI++) {
        for (int examJ=examI+1; examJ<data.nExams; examJ++) {
            commonStudents = data.conflicts[examI][examJ];
            if (commonStudents>0) {
                studentsForSlot[examI][currentSol[examJ]] += commonStudents;
                studentsForSlot[examJ][currentSol[examI]] += commonStudents;
            }
        }
    }
    // compute currentObjective
    currentObjective = 0;
    int slot;
    for (int exam=0; exam<data.nExams; exam++) {
        slot = currentSol[exam];
        for (int distance = 1; distance<=5; distance++) {
            if (slot+distance < data.nSlots) {
                currentObjective += studentsForSlot[exam][slot+distance] * (1<<(5-distance));
            } else {
                break;
            }
        }
    }
    bestObjective = currentObjective;
}// Optimizer()


void Optimizer::run() {
// exacute an entire local search
    int flag;
    temperature = 1e15;
    for (iteration = 0; iteration<100000000; iteration++) {
        //printf("iteration %d\n", iteration);
        flag = moveExam();
        temperature = temperature * 0.95;

        //printf("       f(x) = %d \n", currentObjective);
        //printf("best so far = %d \n", currentObjective);
        //printf("flag=%d\n\n",flag);
    }
}

int Optimizer::moveExam() {
// execute one iteration of the local sear261685488f65c705187f4dd000d2bdbd40e6bc96afe33a2a5023795305e00c0fch;
    //output: a flag indicating if the current solution has changed and why
    // -3 -> choosen exam could not be moved
    // -2 -> move forbidden by tabu list
    // -1 -> not accepted by simulated annealing
    //  1 -> accepted by simulated annealing
    //  2 -> inproving solution, not tabu
    //  3 -> best solution found so far
    // generate a random neighbor
    // pick a random exam and change its timeslot so that no conflicts are generated
    int changedExam = rand() % data.nExams;
    int oldSlot = currentSol[changedExam];

    // selects a feasible new slot
    std::vector<int> availableSlots(data.nSlots, -4);
    int nAvailable = 0;
    for (int slot=0; slot<data.nSlots; slot++) {
        if (studentsForSlot[changedExam][slot]==0 && slot!=oldSlot) {
            availableSlots[nAvailable] = slot;
            nAvailable++;
        }
    }
    int newSlot = -1;
    if (nAvailable > 0) {
        newSlot = availableSlots[rand() % nAvailable];
    }
    // print neighbor
    //printf("neighbor: \nexam = %d   old = %d   new = %d\n",changedExam, currentSol[changedExam], newSlot);
    if (newSlot<0) {
        return -3;
    }

    newObjective = evaluateNeighbor(changedExam, newSlot);

    if (newObjective < bestObjective) {
        // the solution is better than the best found so far
        acceptNeighbor(changedExam, newSlot);
        bestSol = currentSol;
        bestObjective = currentObjective;
        return 3;
    }
    else if (tabuList[changedExam][newSlot] > iteration) {
        // the move is forbidden by the tabu list
        return -2;
    }
    else {
        if (newObjective < currentObjective) {
            // the solution improves the current one
            acceptNeighbor(changedExam, newSlot);
            return 2;
        }
        else {
            // non-improving solution
            double probability = exp ( -(newObjective - currentObjective)/temperature );
            if (probability > rand()/(double)RAND_MAX) {
                acceptNeighbor(changedExam, newSlot);
                return 1;
            }
            else {
                return -1;
            }
        }
    }
}// step()

void Optimizer::acceptNeighbor(int changedExam, int newSlot) {
// move to the solution obtained by moving changedExam to newSlot
    int oldSlot = currentSol[changedExam];

    // update current solution
    currentSol[changedExam] = newSlot;
    currentObjective = newObjective;

    // update tabu list
    tabuList[changedExam][oldSlot] = iteration + 10;

    // update studentsForSlot
    int commonStudents;
    for (int exam = 0; exam<data.nExams; exam++) {
        commonStudents = data.conflicts[exam][changedExam];
        if (commonStudents>0) {
            studentsForSlot[exam][oldSlot] -= commonStudents;
            studentsForSlot[exam][newSlot] += commonStudents;
        }
    }
}// acceptNeighbor()

int Optimizer::evaluateNeighbor(int changedExam, int newSlot){
// incrementally compute the objective function of a neighbor of the current solution
    int objective = currentObjective;
    int oldSlot = currentSol[changedExam];
    for (int dist = 1; dist<=5; dist++) {
        if (oldSlot-dist >= 0){
            objective -= studentsForSlot[changedExam][oldSlot-dist] * (1<<(5-dist));
        }
        if (oldSlot+dist <= data.nSlots){
            objective -= studentsForSlot[changedExam][oldSlot+dist] * (1<<(5-dist));
        }
        if (newSlot-dist >= 0){
            objective += studentsForSlot[changedExam][newSlot-dist] * (1<<(5-dist));
        }
        if (newSlot+dist <= data.nSlots){
            objective += studentsForSlot[changedExam][newSlot+dist] * (1<<(5-dist));
        }
    }
    return objective;
}// evaluateNeighbor()















