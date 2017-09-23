set -eu
log_dir=${LOG_DIR:-./log/}
output_dir=${OUTPUT_DIR:-./output/}
mkdir -p $log_dir $output_dir
program=$1
input=$2
case_num=`basename $2 ".txt"`
datetime=`date +"%Y-%m-%d-%H-%M-%S"`
filename=$case_num-$datetime
$program $input 1> $output_dir/$filename.txt 2> $log_dir/$filename.log