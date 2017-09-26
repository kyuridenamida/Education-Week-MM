# Education-Week-MM
This repository is my history of the development of a program for [Education Week MM](https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16997&pm=14690).
I mainly tried to solve this problem based on Hill Climbing (HC) approach and Simulated Annealing (SA) approach.

## Resources
Problem: https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16997&pm=14690

Standings: https://community.topcoder.com/longcontest/?module=ViewStandings&rd=16997

## Google spreadsheets
Analysis for my solution: https://docs.google.com/spreadsheets/d/1-PTRg_Xf_O9qzIGzw3JzSuv0gfLR_74fFDSvBZ4VhoA/edit?usp=sharing

Pre & final results: https://docs.google.com/spreadsheets/d/1x_Fw4YIKTBzckW3tP_3StDez_oGyzaOa1njbBd9K4EU/edit?usp=sharing

## Score transition
1. 806455.16 HC (with incremental update) ([#6](https://github.com/kyuridenamida/Education-Week-MM/issues/6))
2. 820439.30 SA(\*) (with a sorted array as the initial solution) ([#23](https://github.com/kyuridenamida/Education-Week-MM/issues/23))
3. 822360.59 HC (with changing one element instead of swapping a pair) ([#28](https://github.com/kyuridenamida/Education-Week-MM/issues/28))
4. 822365.58 HC (the same as 3. except that used time increased 8.75s to 9.4s) ([#31](https://github.com/kyuridenamida/Education-Week-MM/issues/31))
5. 822393.73 SA (with tuned parameters and changed timer) ([#37](https://github.com/kyuridenamida/Education-Week-MM/issues/37))

(\*) To use SA was not important at that time (refer to [#24](https://github.com/kyuridenamida/Education-Week-MM/issues/24))


## Key Observations
- Actually sorted array [0,1,...,n-1] is very good initial solution.
- Encoding magnitude relations between pairs of position to a directed graph. 
- Encoding permutation as real numbers of 32(or anything enough)-bit integers. ([#26](https://github.com/kyuridenamida/Education-Week-MM/issues/26))
  - This makes transition more flexible.
  - This increases the number of iterations beecause it's fast in terms of constant optimization.
- Incremental update when change the value at *position k* with the time complexity of O(∑|Edges connected to k|).
- Hill climbling *allowing to transit with zero score improvement* or Simulated annealing.

## Other observations
- You can find the best value to assign with O(∑|Edges connected to k|) log ∑|Edges connected to k|)
  - Sort current values and then use accumulated sum technique to specify the range of values you can achieve maximum increase. ([#33](https://github.com/kyuridenamida/Education-Week-MM/issues/33))

## How to compile main.cpp
```g++ main.cpp -O3 std=c++14 -DLOCAL [-DDEBUG] [-DANALYSIS] [-DENABLE_DIZZY_ANALYSIS] [-DNO_OUTPUT]```

- `-DDEBUG` enables assertions and to show the outputs for `LOG()`.

- `-DANALYSIS` enables to show `ANALYSIS_LOG()`, which is for not fequent logging like *final score*.

- `-DENABLE_DIZZY_ANALYSIS` enables to show `DIZZY_ANALYSIS_LOG()`, which is for frequent logging like *current score at each time* 

- `-DNO_OUTPUT` disables to output answer.

## Scripts
- `localtest.sh` tests `main.cpp` with seed 1 to 100 and store the logs for each test case.
- `run.sh exec_file input_file` is for testing `exec_file` with `input_file`. The log will be stored into `./log/` and `./output` by default. You can change the directory by specifying `LOG_DIR` and `OUTPUT_DIR`.
- `input_generator.sh` generates input files of seed 1 to 100 to `./test_inputs/` directory.
- `generate_submittable_code.sh` generates the auto formatted code for main.cpp and save as `frozen.cpp`.
- `build_result_from_log.sh log_dir` converts all raw logs in `log_dir` into a structured csv file.
- `recent_log.sh` is just a useful command to build a structured csv file for the recent local test.
- `csv_builder.py log_file` is a faster way to build structured csv file, but no longer used due to its low flexibility.
