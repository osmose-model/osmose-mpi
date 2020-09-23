/*
 * Rudimentary applications to launch batch of Osmose simulations with MPI.
 * The program takes as argument the path of a text file. The text file contains
 * the list of Java commands for launching Osmose simulations.
 * 
 * Example:
 * mpirun -np 4 osmosempi indiseas-simulation.txt
 * with indiseas-simulation.txt:
 * java -jar osmose.jar simulation1.csv
 * java -jar osmose.jar simulation2.csv
 * java -jar osmose.jar simulation3.csv
 * etc.
 * 
 * File:   main.cpp
 * Author: P. Verley (philippe.verley@ird.fr
 */

#include <cstdlib>
#include <mpi.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

/* Print details in the console */
const bool DEBUG=false;

/* Functions headers */
void readFile(const char *filename, vector<string> &files, int rank);
void checkFileExist(const char *filename, int rank);

/*
 * 
 */
int main(int argc, char** argv)
    {

    int rank = -1, size = 0;

    if (argc!=2)
        {
        stringstream msg;
        msg<<"You must provide the path of the text file containing the Java commands for running Osmose simulations."<<endl;
        cerr<<msg.str();
        return 1;
        }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (DEBUG)
        {
        stringstream msg;
        msg<<"Rank "<<rank<<" | Osmose launchers read from "<<argv[1]<<endl;
        cout<<msg.str();
        }
    vector<string> files=vector<string>();
    readFile(argv[1], files, rank);

    if (files.size()<size)
        {
        stringstream msg;
        msg<<"Rank "<<rank<<" | Number of configuration files in "<<argv[1]<<" must be >= the number of MPI procs. nFile "<<files.size()<<" nProc "<<size<<endl;
        cerr<<msg.str();
        return 1;
        }

    for (int iSimu=0; iSimu < files.size(); iSimu++)
        {
        if ((iSimu%size)==rank)
            {
//            stringstream msg;
//            msg<<"Rank "<<rank<<" | Running Osmose simulation from line "<<iSimu<<" ..."<<endl;
//            cout<<msg.str();
            int err=system(files.at(iSimu).c_str());
            if (err!=0)
                {
                stringstream msg;
                msg<<"Rank "<<rank<<" | Failed to run Osmose simulation from line "<<iSimu<<endl<<files.at(iSimu).c_str()<<endl;
                cerr<<msg.str();
                }
            else
                {
                stringstream msg;
                msg<<"Rank "<<rank<<" | Completed Osmose simulation from line "<<iSimu<<endl;
                cout<<msg.str();
                }
            }
        }

    MPI_Finalize();

    return 0;
    }

void readFile(const char *filename, vector<string> &files, int rank)
    {

    checkFileExist(filename, rank);
    stringstream msg;

    ifstream file(filename, ios::in);
    if (file)
        {
        string line;
        while (getline(file, line))
            {
            if (line.empty()||line[0]=='#') continue;
            files.push_back(line);
            if (DEBUG)
                {
                msg<<"Rank "<<rank<<" | readFile() Adding command "<<line<<endl;
                cout<<msg.str();
                }
            }
        file.close();
        }
    else
        {
        msg<<"Rank "<<rank<<" | readFile() Failed to read file "<<filename<<endl;
        cerr<<msg.str();
        }
    }

void checkFileExist(const char *filename, int rank)
    {
    FILE* f;
    stringstream msg;
    if (!(f=fopen(filename, "r")))
        {
        msg<<"Rank "<<rank<<" | checkFileExist() Failed to open file: "<<filename<<endl;
        cerr<<msg.str();
        exit(1);
        }
    fclose(f);
    }

