set -eu
g++ input_generator.cpp -o /tmp/input_generator.out
javac ConstrainedPermutationVis.java
mkdir -p ./test_inputs/
for i in `seq 1 100`
do
	echo "processing seed $i" >&2
	java ConstrainedPermutationVis -seed $i -exec "/tmp/input_generator.out" | sed "/[SF]/d" > ./test_inputs/$i.txt
done 
