#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=20                   # number of processes = 20
#SBATCH --cpus-per-task=1            # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
# cd /nfsmnt/119010378/project1_template/

# g++ odd_even_sequential_sort.cpp -o ssort
# mkdir ./sequential_report_result
for ele_num in {640000,960000}
    do 
        echo "sequential $ele_num"
        ./ssort $ele_num ./report_test_data/$ele_num.in >> ./sequential_report_result/$ele_num.txt
        ./check $ele_num ./report_test_data/$ele_num.in.seq.out >> ./sequential_report_result/$core/$ele_num.txt
    done