set -eu
mkdir -p ./log/ ./output/
program=$1
input=$2
case_num=`basename $2 ".txt"`
datetime=`date +"%Y-%m-%d-%H-%M-%S"`
filename=$datetime-$case_num
$program $input 1> ./output/$filename.txt 2> ./log/$filename.log