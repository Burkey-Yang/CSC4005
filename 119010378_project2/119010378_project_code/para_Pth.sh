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
g++ pthread.cpp -o pthread -lpthread -O2 -std=c++11
for core in {1,2,4,8,12,16,20}
do 
    mkdir ./para_Pth1/$core
    for ele_num in  {4000,8000,16000}  # 100,200,400,600,800,1000,1200,1400,1600,1800,2000
    do 
        mkdir ./para_Pth1/$core/$ele_num
        for pro_num in {20,50,100,200}
        do 
            echo "$core and $ele_num"
            ./pthread $ele_num $ele_num $pro_num $core >> ./para_Pth1/$core/$ele_num/$pro_num.txt
        done
    done
done