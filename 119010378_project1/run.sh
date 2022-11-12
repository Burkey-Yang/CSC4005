#!/bin/bash

# HiBench 目录
hibench_home=/home/account0/HiBench3
hibench_report_path=/home/account0/HiBench3/report
result_home=/home/account0/results

# 数据量类型
datasize_type=("tiny" "small" "large")

terasort_result_dir=$result_home/terasort
wordcount_result_dir=$result_home/wordcount
pagerank_result_dir=$result_home/pagerank


error_exit() {
  echo "$1" 1>&2
  exit 1
}

kinit_login() {
    echo "====== Start obtain and cache Kerberos ticket-granting ticket ======"

    echo "csc4160?" | kinit team13 || error_exit "Unable to login with kinit"

    echo "==================== Start execute hibench test ======================"
}

change_datasize() {
    # change datasize
	echo "Start changing datasize ..."
	hibench_conf_path=$hibench_home/conf/hibench.conf
	text="hibench.scale.profile                $1"
	sed -i "3c $text" $hibench_conf_path
}

change_mapper_reducer() {
    echo "Start changing mapper and reducer ..."
	hibench_conf_path=$hibench_home/conf/hibench.conf
	text="hibench.default.map.parallelism                $1"
	sed -i "5c $text" $hibench_conf_path
	text="hibench.default.shuffle.parallelism            $2"
	sed -i "8c $text" $hibench_conf_path
}

copy_report() {
    tail -n 1 "$hibench_report_path/hibench.report" | awk '{printf "%-20s %-16s %-12s %-6s %-3s %-3s %-16s %-10s %-10s\n", $1, $2, $3, v1, v2, v3, $5, $6, $7}' v1="$2" v2="$3" v3="$4" >> "$1/hibench.report"
}


# run_single_bench data_size mapper_size reducer_size
run_single_bench() {
    echo "====================Start execute hibench datasize: $1, mapper: $2, reducer: $3 ======================"

    sleep 3
    echo "Running terasort test ..."
    sh $hibench_home/bin/workloads/micro/terasort/hadoop/run.sh || error_exit "Unable to run terasort"
    target_dir=$terasort_result_dir/$1_$2_$3
    mkdir "$target_dir"
    copy_report $terasort_result_dir "$1" "$2" "$3"
    cp "$hibench_report_path/terasort/hadoop/bench.log" "$target_dir/bench.log"
    
    
    sleep 3
    echo "Running wordcount test ..."
    sh $hibench_home/bin/workloads/micro/wordcount/hadoop/run.sh || error_exit "Unable to run wordcount"
    target_dir=$wordcount_result_dir/$1_$2_$3
    mkdir "$target_dir"
    copy_report $wordcount_result_dir "$1" "$2" "$3"
    cp "$hibench_report_path/wordcount/hadoop/bench.log" "$target_dir/bench.log"


    sleep 3
    echo "Running pagerank test ..."
    sh $hibench_home/bin/workloads/websearch/pagerank/hadoop/run.sh || error_exit "Unable to run pagerank"
    target_dir=$pagerank_result_dir/$1_$2_$3
    mkdir "$target_dir"
    copy_report $pagerank_result_dir "$1" "$2" "$3"
    cp "$hibench_report_path/pagerank/hadoop/bench.log" "$target_dir/bench.log"
}


gen_data() {
    change_datasize "$1"
	echo "Creating wordcount data ..."
	sh $hibench_home/bin/workloads/micro/wordcount/prepare/prepare.sh

	echo "Creating terasort data ..."
	sh $hibench_home/bin/workloads/micro/terasort/prepare/prepare.sh

	echo "Creating pagerank data ..."
	sh $hibench_home/bin/workloads/websearch/pagerank/prepare/prepare.sh
}

main() {
    sleep 3000
    kinit_login
    mkdir $result_home
    mkdir $terasort_result_dir
    mkdir $wordcount_result_dir
    mkdir $pagerank_result_dir
    for data_size in ${datasize_type[*]}
    do
        gen_data "$data_size"
        for i in {1..4}
        do
            for j in {1..4}
            do
                change_mapper_reducer "$i" "$j"
                run_single_bench "$data_size" "$i" "$j"
            done
        done

    done
}

main
