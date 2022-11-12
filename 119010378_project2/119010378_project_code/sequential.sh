#!/bin/bash
#SBATCH --job-name=your_job_name # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=20                   # number of processes = 20
#SBATCH --cpus-per-task=1            # Number of CPU cores allocated to each process
#SBATCH --partition=Project            # Partition name: Project or Debug (Debug is default)
# cd /nfsmnt/119010378/project1_template/

g++ sequential.cpp -o seq -O2 -std=c++11
mkdir ./sequential_report_result
for ele_num in {4000,8000,16000} # 100,200,400,600,800,1000,1200,1400,1600,1800,2000
    do 
        echo "sequential $ele_num"
        ./seq $ele_num $ele_num 100 >> ./sequential_report_result/$core/$ele_num.txt
    done