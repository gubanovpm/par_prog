#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>

using namespace std;

int main( int argc, char *argv[]) {
    int num_errors = 0;
    int rank, size;
    char port1[MPI_MAX_PORT_NAME];
    char port2[MPI_MAX_PORT_NAME];
    MPI_Status status;
    MPI_Comm comm1, comm2;
    int data = 0;

    char *ptr;
    int runno = strtol(argv[1], &ptr, 10);
    for (int i = 0; i < argc; ++i)
        printf("inputs %d %d %s \n", i,runno, argv[i]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (runno == 0) {
        printf("0: opening ports.\n");fflush(stdout);
        MPI_Open_port(MPI_INFO_NULL, port1);
        printf("opened port1: <%s>\n", port1);

        //Write port file
        ofstream myfile;
        myfile.open("port");
        if( !myfile )
                cout << "Opening file failed" << endl;
        myfile << port1 << endl;
        if( !myfile )
            cout << "Write failed" << endl;
        myfile.close();

        printf("Port %s written to file \n", port1); fflush(stdout);

        printf("Attempt to accept port1.\n");fflush(stdout);

        //Establish connection and send data
        MPI_Comm_accept(port1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &comm1);

        data = 7;
        printf("sending %d \n", data);fflush(stdout);
        
        MPI_Send(&data, 1, MPI_INT, 0, 0, comm1);
        MPI_Close_port(port1);
    }
    else if (runno == 1) {
        //Read port file
        size_t   chars_read = 0;  
        ifstream myfile;
        //Wait until file exists and is avaialble
        myfile.open("port");
        while(!myfile) {
            myfile.open("port");
            //cout << "Opening file failed" << myfile << endl;
            usleep(30000);
        }
        while( myfile && chars_read < 255 ) {
            myfile >> port1[ chars_read ];    
            if( myfile ) 
                    ++chars_read; 

            if( port1[ chars_read - 1 ] == '\n' ) 
                    break;
        }
        printf("Reading port %s from file \n", port1); fflush(stdout);
        remove( "port" );

        //Establish connection and recieve data
        MPI_Comm_connect(port1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &comm1);
        MPI_Recv(&data, 1, MPI_INT, 0, 0, comm1, &status);
        printf("Received %d 1\n", data); fflush(stdout);

    }

    //Barrier on intercomm before disconnecting
    MPI_Barrier(comm1);
    MPI_Comm_disconnect(&comm1);
    MPI_Finalize();
    return 0;
}

