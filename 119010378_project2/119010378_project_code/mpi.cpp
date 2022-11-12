#include "asg2.h"
#include <stdio.h>
#include <mpi.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <complex>


int rank;
int world_size;
MPI_Datatype Point_TYPE;


void master() {
	Point* p=data;
	int Total_size = total_size;
	std::vector<Point> calculate_Vector;
	std::copy(p,p + Total_size,std::back_inserter(calculate_Vector));
	int calcu_size = Total_size;
	int local_data_size = Total_size/world_size;
	int remainder = total_size%world_size;
	if(remainder != 0){
		int lack_element = world_size - remainder;
		calcu_size += lack_element;
		Point last_ele = calculate_Vector[Total_size-1];
		for(int i = 0; i < lack_element; i++){
			calculate_Vector.push_back(last_ele);
		}
		local_data_size = calcu_size/world_size;
	}
	MPI_Bcast(&local_data_size,1,MPI_INT,0,MPI_COMM_WORLD);

	std::vector<Point> local_data_buffer(local_data_size);

	MPI_Scatter(calculate_Vector.data(), local_data_size, Point_TYPE, local_data_buffer.data(), local_data_size, Point_TYPE, 0, MPI_COMM_WORLD);
	for(int i = 0; i < local_data_size; i++){
		compute(&local_data_buffer[i]);
	}
	MPI_Gather(local_data_buffer.data(), local_data_size, Point_TYPE, calculate_Vector.data(), local_data_size, Point_TYPE, 0, MPI_COMM_WORLD);
	std::copy(calculate_Vector.begin(), calculate_Vector.begin()+(Total_size), data);
}


void slave() {
	int local_data_size;
	std::vector<Point> calculate_Vector;
	MPI_Bcast(&local_data_size,1,MPI_INT,0,MPI_COMM_WORLD);
	std::vector<Point> local_data_buffer(local_data_size);
	MPI_Scatter(calculate_Vector.data(), local_data_size, Point_TYPE, local_data_buffer.data(), local_data_size, Point_TYPE, 0, MPI_COMM_WORLD);
	for(int i = 0; i < local_data_size; i++){
		compute(&local_data_buffer[i]);
	}
	MPI_Gather(local_data_buffer.data(), local_data_size, Point_TYPE, calculate_Vector.data(), local_data_size, Point_TYPE, 0, MPI_COMM_WORLD);
}


int main(int argc, char *argv[]) {
	if ( argc == 4 ) {
		X_RESN = atoi(argv[1]);
		Y_RESN = atoi(argv[2]);
		max_iteration = atoi(argv[3]);
	} else {
		X_RESN = 1000;
		Y_RESN = 1000;
		max_iteration = 100;
	}

	if (rank == 0) {
		#ifdef GUI
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(500, 500); 
		glutInitWindowPosition(0, 0);
		glutCreateWindow("MPI");
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0, X_RESN, 0, Y_RESN);
		glutDisplayFunc(plot);
		#endif
	}

	/* computation part begin */
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// initialize MPI data type
	
	const int count = 3;
	int blocklengths[3] = {1,1,1};
	MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_FLOAT};
	MPI_Aint displacements[3];
	displacements[0] = offsetof(Point,x);
	displacements[1] = offsetof(Point,y);
	displacements[2] = offsetof(Point,color);
	MPI_Type_create_struct(count, blocklengths, displacements, types, &Point_TYPE);
    MPI_Type_commit(&Point_TYPE);


	if (rank == 0) {
		t1 = std::chrono::high_resolution_clock::now();

		initData();
		master();

		t2 = std::chrono::high_resolution_clock::now();  
		time_span = t2 - t1;

		printf("Student ID: 119010378\n"); // replace it with your student id
		printf("Name: Yang Hongmeng\n"); // replace it with your name
		printf("Assignment 2 MPI\n");
		printf("Run Time: %f seconds\n", time_span.count());
		printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
		printf("Process Number: %d\n", world_size);
		
	} 
	else {
		slave();
	}

	MPI_Type_free(&Point_TYPE);
	MPI_Finalize();
	/* computation part end */

	if (rank == 0){
		#ifdef GUI
		glutMainLoop();
		#endif
	}

	return 0;
}

