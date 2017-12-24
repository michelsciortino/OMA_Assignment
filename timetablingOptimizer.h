//
// Created by nathan on 12/20/17.
//

#ifndef TIMETABLINGOPTIMIZER_H_INCLUDED
#define TIMETABLINGOPTIMIZER_H_INCLUDED

#include "inputOmaGraph.h"
#include<vector>

// classes defined: Matrix, Optimizer

// data stucture representing an n x m matrix
class Matrix {

public:
    Matrix();
    Matrix(int rows_, int columns_);
    //~Matrix();

    int rows; // number of rows
    int cols; // number of columns

    //std::vector<std::vector<int>> entries; // array containing in place [m*i+j] element [i][j] of the matrix
    int * entries; // array containing in place [m*i+j] element [i][j] of the matrix

    //std::vector<int> operator[](int index); // so that we can use notation matrix[i][j] instead of matrix.entries[cols*i+j]
    int* operator[](int index); // so that we can use notation matrix[i][j] instead of matrix.entries[cols*i+j]
};// Matrix


// class responsible of optimizing a timetabling problem, starting from a given feasible solution
class Optimizer {

public:
    Optimizer();
    Optimizer(Problem data_, std::vector<int> initialSol);
    //~Optimizer();

    int moveExam(); // execute one iteration of the local search
    void run(); // execute an entire local search
    int evaluate(std::vector<int> sol);
    int evaluateNeighbor(int changedExam, int newSlot);
    void acceptNeighbor(int changedExam, int newSlot);

    // Class containing the data of the timetabling problem to optimize
    Problem data;

    std::vector<int> currentSol; // current solution
    std::vector<int> bestSol; // best solution found so far
    std::vector<int> groupOfSlot;
    std::vector<int> slotOfGroup;

    int currentObjective; // current value of the objective function
    int bestObjective; // best value found so far
    int newObjective; // value of the objective function on next iteration's solution

    int iteration;
    double temperature; // current "temperature" of the simulated annealing

    Matrix studentsForSlot; // matrix containig in place [i][t] the sum of conflicts[i][j] over all j such that currentSol[j]=t
    Matrix tabuList;
};// Optimizer

#endif // TIMETABLINGOPTIMIZER_H_INCLUDED
