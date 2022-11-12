#include <mpi.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <limits>
#include <algorithm>


int main (int argc, char **argv){

    MPI_Init(&argc, &argv); 

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int num_elements; // number of elements to be sorted
    int local_data_size; // each process receive data number
    std::vector<int> copy_sortData_array;
    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int elements[num_elements]; // store elements
    int sorted_elements[num_elements]; // store sorted elements

    if (rank == 0) { // read inputs from file (master process)
        std::ifstream input(argv[2]);
        int element;
        int i = 0;
        while (input >> element) {
            elements[i] = element;
            i++;
        }
        std::cout << "actual number of elements:" << i << std::endl;
    }

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;
    if (rank == 0){ 
        t1 = std::chrono::high_resolution_clock::now(); // record time
    }

    // step 1: get basic information of sort array and preprocess the sort data
    // only process 0 access the original data
    int receive_data;
    if (0 == rank) {
        std::copy(&elements[0],&elements[num_elements],std::back_inserter(copy_sortData_array));
        int copy_array_size = num_elements;  // set the copy array element number same as original for further calculation
        int remainder_of_element = copy_array_size % world_size;

        if(remainder_of_element != 0){
            int lack_element = world_size - remainder_of_element;
            copy_array_size += lack_element;

            for(int i = 0; i < lack_element; i++){
                copy_sortData_array.push_back(std::numeric_limits<int>::max());
            }
        }
        local_data_size = copy_array_size / world_size;
    } 
    // boardcast the receive data size
    MPI_Bcast(&local_data_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //create local buffeer to ready for receive data
    std::vector<int> local_data_buffer(local_data_size);

    // scatter data to each process
    MPI_Scatter(copy_sortData_array.data(), local_data_size, MPI_INT, local_data_buffer.data(), local_data_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // step 2 : sort array!!
    bool total_sorted = false;
    bool local_sorted= false;
    
    while(!total_sorted){
        //odd process
        //even number element send to odd element
        local_sorted = true;
        
        for(int i = 1; i < local_data_size; i+=2){
            if(local_data_buffer[i] < local_data_buffer[i-1]){
                int temp = local_data_buffer[i];
                local_data_buffer[i] = local_data_buffer[i-1];
                local_data_buffer[i-1] = temp;
                local_sorted = false;
            }
        }
        // only each local buffer has odd number of elements,data send in odd process is necessary
        if(world_size % 2 == 1 && rank == world_size -1){}
        else if(rank % 2 != 0){
            MPI_Sendrecv(local_data_buffer.data(), 1, MPI_INT, (rank - 1 + world_size)%world_size, 0,
                local_data_buffer.data(), 1, MPI_INT, (rank - 1 + world_size)%world_size, 0, 
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else{ //even number process receive from later process
            MPI_Recv(&receive_data,1,MPI_INT, (rank+1+world_size)%world_size , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(receive_data < local_data_buffer[local_data_size-1]){
                int temp = receive_data;
                receive_data = local_data_buffer[local_data_size-1];
                local_data_buffer[local_data_size-1] = temp;
                local_sorted = false;
            }
            MPI_Send(&receive_data,1,MPI_INT, (rank+1+world_size)%world_size, 0, MPI_COMM_WORLD);
        }
        //even process
        //odd number element send to even element
        for(int i = 2; i < local_data_size; i+=2){
            if(local_data_buffer[i] < local_data_buffer[i-1]){
                int temp = local_data_buffer[i];
                local_data_buffer[i] = local_data_buffer[i-1];
                local_data_buffer[i-1] = temp;
                local_sorted = false;
            }
        }
        // only each local buffer has even number elments, data send is necessary in even step
        if(rank != 0){
            if(world_size % 2 == 0 && rank == world_size -1){}
            else if(rank % 2 != 1){
                MPI_Sendrecv(local_data_buffer.data(), 1, MPI_INT, (rank - 1 + world_size)%world_size, 0,
                local_data_buffer.data(), 1, MPI_INT, (rank - 1 + world_size)%world_size, 0, 
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else{
                MPI_Recv(&receive_data,1,MPI_INT, (rank+1+world_size)%world_size , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(receive_data < local_data_buffer[local_data_size-1]){
                    int temp = receive_data;
                    receive_data = local_data_buffer[local_data_size-1];
                    local_data_buffer[local_data_size-1] = temp;
                    local_sorted = false;
                }
                MPI_Send(&receive_data,1,MPI_INT, (rank+1+world_size)%world_size, 0, MPI_COMM_WORLD);
            }
        }
        // check whether no swap has happened in all processes
        MPI_Reduce(&local_sorted, &total_sorted, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

        // broadcast the signal whether another sort iteration is necessary
        MPI_Bcast(&total_sorted, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    // step 3 : gather sorted array 
    MPI_Gather(local_data_buffer.data(), local_data_size, MPI_INT, copy_sortData_array.data(), local_data_size, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank == 0){
        // strip the padded data and put needed data back to original array
        std::copy(copy_sortData_array.begin(), copy_sortData_array.begin()+(num_elements), sorted_elements); /////////attention need to change
    }

    
    /* TODO END */

    if (rank == 0){ // record time (only executed in master process)
        t2 = std::chrono::high_resolution_clock::now();  
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Student ID: " << "119010378" << std::endl; // replace it with your student id
        std::cout << "Name: " << "Yang Hongmeng" << std::endl; // replace it with your name
        std::cout << "Assignment 1" << std::endl;
        std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
        std::cout << "Input Size: " << num_elements << std::endl;
        std::cout << "Process Number: " << world_size << std::endl; 
    }

    if (rank == 0){ // write result to file (only executed in master process)
        std::ofstream output(argv[2]+std::string(".parallel.out"), std::ios_base::out);
        for (int i = 0; i < num_elements; i++) {
            output << sorted_elements[i] << std::endl;
        }
    }
    
    MPI_Finalize();
    
    return 0;
}


