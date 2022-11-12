#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>



int main (int argc, char **argv){

    int num_elements; // number of elements to be sorted
    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int elements[num_elements]; // store elements
    int sorted_elements[num_elements]; // store sorted elements

    std::ifstream input(argv[2]);
    int element;
    int i = 0;
    while (input >> element) {
        elements[i] = element;
        i++;
    }
    std::cout << "actual number of elements:" << i << std::endl;

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;
    t1 = std::chrono::high_resolution_clock::now(); // record time

    /* TODO BEGIN
        Implement sequential odd even transposition sort
        Code in this block is not a necessary. 
        Replace it with your own code.
    */
    bool sorted = false;
    while(!sorted){
        // odd process sorted
        sorted = true;
        for(int i = 1; i < num_elements; i+=2){
            if(elements[i] < elements[i-1]){
                int temp = elements[i];
                elements[i] = elements[i-1];
                elements[i-1] = temp;
                sorted = false;
            }
        }
        //even process sorted
        for(int i = 2; i < num_elements; i+=2){
            if(elements[i] < elements[i-1]){
                int temp = elements[i];
                elements[i] = elements[i-1];
                elements[i-1] = temp;
                sorted = false;
            }
        }
    }
    // std::sort(elements,elements+num_elements);
    for (int i = 0; i < num_elements; i++) {
        sorted_elements[i] = elements[i];
    }
    /* TODO END */

    t2 = std::chrono::high_resolution_clock::now();  
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Student ID: " << "119010378" << std::endl; // replace it with your student id
    std::cout << "Name: " << "Yang Hongmeng" << std::endl; // replace it with your name
    std::cout << "Assignment 1" << std::endl;
    std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
    std::cout << "Input Size: " << num_elements << std::endl;
    std::cout << "Process Number: " << 1 << std::endl;   
    
    std::ofstream output(argv[2]+std::string(".seq.out"), std::ios_base::out);
    for (int i = 0; i < num_elements; i++) {
        output << sorted_elements[i] << std::endl;
    }
    
    return 0;
}


