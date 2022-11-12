#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=20                   # number of processes = 20
#SBATCH --cpus-per-task=1            # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
# cd /nfsmnt/119010378/project1_template/

# mpic++ odd_even_parallel_sort.cpp -o psort
# g++ test_data_generator.cpp -o gen

# for ele_num in 20
# do 
#     ./gen $ele_num ./report_test_data/$ele_num.in
# done

# salloc -n40 -t10 -p Project
 
for core in 1 # {1,2,4,8,12,16,20,24,28,32,36,40}
do 
    mkdir ./parallel_report_result/$core
    for ele_num in  960000 # {20,10000,20000,40000,80000,160000,320000,640000,960000}
    do 
        echo "$core and $ele_num"
        mpirun -np $core ./psort $ele_num ./report_test_data/$ele_num.in >> ./parallel_report_result/$core/$ele_num.txt
        ./check $ele_num ./report_test_data/$ele_num.in.parallel.out >> ./parallel_report_result/$core/$ele_num.txt
    done
done