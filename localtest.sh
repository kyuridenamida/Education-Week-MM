set -eu
PROG_PATH=/tmp/main.out
rm $PROG_PATH
g++ main.cpp -o $PROG_PATH -O3 -std=c++14 -DNO_OUTPUT -DLOCAL -DANALYSIS

datetime=`date +"%Y-%m-%d-%H-%M-%S"`
mkdir -p ./results_localtest/$datetime

for seed in `seq 1 100`
do
	echo "running seed $seed..." >&2
	time LOG_DIR=./results_localtest/$datetime/log OUTPUT_DIR=./results_localtest/$datetime/output ./run.sh $PROG_PATH ./test_inputs/$seed.txt
done