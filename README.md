# Education-Week-MM
This repository is my history of the development of a program for [Education Week MM](https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16997&pm=14690).

## Resources
Problem: https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16997&pm=14690
Standings: https://community.topcoder.com/longcontest/?module=ViewStandings&rd=16997

## Key Observations
- Encoding magnitude relations between pairs of position to a directed graph. 
- Encoding permutation as real numbers of 32(or anything enough)-bit integers. ([#26](https://github.com/kyuridenamida/Education-Week-MM/issues/26))
  - This makes transition more flexible.
  - This increases the number of iterations beecause it's fast in terms of constant optimization.
- Incremental update when change the value at *position k* with the time complexity of O(∑|Edges connected to k|).
- Hill climbling *allowing to transit with zero score improvement* or Simulated annealing.

## Other observations
- You can find the best value to assign with O(∑|Edges connected to k| log ∑|Edges connected to k|)
  - Sort current values and then use accumulated sum technique to specify the range of values you can achieve maximum increase. ([#33](https://github.com/kyuridenamida/Education-Week-MM/issues/33))
