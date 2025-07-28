#include <iostream>
#include <mpi.h>
#include <cmath>
#include <numeric>
#include <vector>

int get_parent(int child, int num_nodes) {
    if (child == 0) {
        return -1;
    }
    return floor((child - 1) / 2);
}

int get_left_child(int parent, int num_nodes) {
    if (num_nodes - 1< parent * 2 + 1) {
        return -1;
    }
    return parent * 2 + 1;
}

int get_right_child(int parent, int num_nodes) {
    if (num_nodes - 1< parent * 2 + 2) {
        return -1;
    }
    return parent * 2 + 2;
}

int Bcast_simple(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);

int Bcast_tree(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);



int main(int argc, char **argv) {
    bool simple_broadcast = false;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int root = 0; // root

    int * data;
    double start;
    double end;
    if (rank == root) {
        const int size = 1000000;
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = i;
        }
    }

    const double SIZE = 100;
    start = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    for (int trial = 0; trial < SIZE; trial++) {
        if (!simple_broadcast)
            Bcast_tree(&data, 1, MPI_INT, root, MPI_COMM_WORLD);
        else
            Bcast_simple(&data, 1, MPI_INT, root, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();


    if (rank == root) {
        delete [] data;
        double average = (end - start)/SIZE;
        std::cout << "Average time for : " << (simple_broadcast ? "simple" : "tree") << " broadcast: "<< average << std::endl;
    }



    MPI_Finalize();
    return 0;
}


int Bcast_simple(void *buffer, int count, MPI_Datatype datatype, int root,
                 MPI_Comm comm) {
    int myrank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int tag = root;
    //root, broadcasting
    if (myrank == root) {
        for (int i = 1; i < size; i++) {
            MPI_Send(buffer, count, datatype, i, tag, comm);
        }
    }
    //other nodes, being broadcast
    else {
        MPI_Recv(buffer, count, datatype, root, tag, comm, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}

int Bcast_tree(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int myrank;
    MPI_Comm_rank(comm, &myrank);

    int size;
    MPI_Comm_size(comm, &size);


    if (myrank != root) {
        MPI_Recv(buffer, count, datatype, get_parent(myrank, size), /*get_parent(myrank, size)*/0, comm, MPI_STATUS_IGNORE);
    }

    int left_child = get_left_child(myrank, size);
    if (left_child != -1) {
        MPI_Send(buffer, count, datatype, left_child, 0/*myrank*/, comm);
    }

    int right_child = get_right_child(myrank, size);
    if (right_child != -1) {
        MPI_Send(buffer, count, datatype, right_child, 0/*myrank*/, comm);
    }

    return MPI_SUCCESS;
}
