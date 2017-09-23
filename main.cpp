#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
using namespace std;

const int INF = 12345;
const double TIME_LIMIT = 9.75;

const unsigned int RANDMAX = -1;

unsigned int randxor()
{
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

template<class T> ostream& operator<<(ostream& os, const vector<T>& v)  
{
	for(int i = 0 ; i < v.size() ; i++){
		os << v[i] << (i+1==v.size()?"":" ");
	}
	return os;  
} 

namespace Timer{
	bool is_started = false;
	unsigned long long int cycle_per_sec = 2500000000;
	unsigned long long int beginCycle;
	unsigned long long int get_cycle()
	{
	  unsigned int low, high;
	  __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
	  return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
	}
	double time_elapsed()
	{
		assert(is_started);
		return (double)(get_cycle() - beginCycle) / cycle_per_sec;
	}
	void reset_timer(){
		is_started = true;
		beginCycle = get_cycle();
	}
	bool is_TLE(double limit){
		assert(is_started);
		return time_elapsed() >= limit;
	}
};
using namespace Timer;

#ifdef DEBUG
	#define LOG(...) log(__LINE__,__VA_ARGS__)
	template<class T> string unpack(const T &v){
		stringstream ss;
		ss << v;
		return ss.str();	
	}
	template<class T, class ...Args> string unpack(const T &v, Args... args)  
	{
		stringstream ss;
		ss << v << " " << unpack(args...);
		return ss.str();  
	} 

	template<class ...Args> void log(int line, Args... args){
		char time[16];
		sprintf(time, "%2.4f", time_elapsed());
		cerr << time << "s " << unpack(args...) << " (line " << line << ")" << endl;
	}
#else
	#define LOG(...)
	#undef assert
	#define assert(x)
#endif


struct Constraint{
	int i,j;
	Constraint(int i,int j) : i(i), j(j){}
	bool operator < (const Constraint &op){
		if( this->i != op.i ) return this->i < op.i;
		return this->j < op.j;
	}
};

struct Constraints {
	vector<Constraint> raw;
	vector< vector<int> > graph;
	vector< vector<int> > reversed_graph;
	vector< vector<bool> > matrix;

	Constraints(vector<string> raw_str, int N){
		vector<Constraint> raw;
		for(auto &&s : raw_str){
			stringstream ss(s);
			int i,j;
			ss >> i >> j;
			raw.push_back(Constraint(i,j));
		}
		constructor(raw, N);
	}

	void constructor(vector<Constraint> raw, int N){
		this->raw = raw;
		graph.resize(N);
		reversed_graph.resize(N);
		matrix = vector< vector<bool> >(N,vector<bool>(N));
		for( auto && c : raw ){
			graph[c.i].push_back(c.j);
			reversed_graph[c.j].push_back(c.i);
			matrix[c.i][c.j] = true;
		}
	}
};


struct Solution {
public:
	vector<int> perm;
	vector<int> pos;
	Constraints *constraints;
	int score;

	Solution(vector<int> perm, Constraints *constraints) : perm(perm), constraints(constraints){
		score = evaluate();
	}

	int partial_evaluate(int pi){
		int sum = 0;
		for(int i = 0 ; i < constraints->graph[pi].size() ; i++){
			if( perm[pi] < perm[constraints->graph[pi][i]] ) sum++;
		}
		
		for(int i = 0 ; i < constraints->reversed_graph[pi].size() ; i++){
			if( perm[constraints->reversed_graph[pi][i]] < perm[pi] ) sum++;
		}
		return sum;
	}
	int N(){ return perm.size(); }

	int update(int pi,int pj){
		if( perm[pi] > perm[pj] ) return update(pj,pi);
		assert(pi!=pj);
		// LOG(vi,vj, constraints->matrix[vi][vj]);

		int sum_before = partial_evaluate(pi) + partial_evaluate(pj) - constraints->matrix[pi][pj];
		// LOG(sum_before, "before");
		swap(perm[pi], perm[pj]);
		assert(perm[pj] < perm[pi]);
		int sum_after = partial_evaluate(pi) + partial_evaluate(pj) - constraints->matrix[pj][pi];
		// LOG(sum_after, "after");
		score += sum_after - sum_before;;
		return (sum_after - sum_before);
	}

	void revert(int pi,int pj,int previous_score_diff){
		swap(perm[pi], perm[pj]);
		score -= previous_score_diff;
	}

	int evaluate(){
		int ans = 0;
		for( auto &&c : constraints->raw){
			ans += perm[c.i] < perm[c.j];
		}
		return ans;
	}
};

class ConstrainedPermutation{
	Constraints *constraints;
	
	Solution initial_solution(int N){
		vector<int> p(N);
		for(int i = 0 ; i < N ; i++) p[i] = i;
		for(int i = 0 ; i < 114 ; i++){
			random_shuffle(p.begin(),p.end());
			// cout << i 
		}

		return Solution(p, constraints);
	}

	Solution simulated_annealing(Solution solution){

		const double temprature_begin = solution.constraints->raw.size();
		const double template_end = 0;
		const int max_t = 10000000;

		int t = 0;
		
		Solution best_solution = solution; 
		while( t < max_t && !is_TLE(TIME_LIMIT)){
			int vi = randxor() % solution.N();
			int vj = randxor() % solution.N();
			if( vi == vj ) continue;

			// LOG("score_before", solution.score, "current[", solution.perm, "]", vi, vj);
			int score_diff = solution.update(vi,vj);
			// LOG("score_diff", score_diff);
			// LOG("score_after", solution.score, "next[", solution.perm, "]", vi, vj);
			//LOG("true_score", solution.evaluate());
				

			//assert(solution.score == solution.evaluate());
			bool do_update = false;

			if( score_diff == 0 ) LOG("no changed score", 1.0 * solution.score / solution.constraints->raw.size() , "(", t, ")");
			if( score_diff > 0 ){
				do_update = true;
			}else{
				// double temprature = temprature_begin + (template_end - temprature_begin) * t / max_t;
				// double prob = exp( score_diff / temprature);
				// do_update = randxor() < prob * RANDMAX;
			}

			if(!do_update){
				solution.revert(vi,vj,score_diff);
			}
			if( best_solution.score < solution.score ){
				best_solution = solution;
				LOG("update!", score_diff, 1.0 * solution.score / solution.constraints->raw.size() , "(", t, ")");
			}
			t++;
		}
		return best_solution;
	}

public:
	vector<int> permute(int N,vector<string> constraints_str){
		
		reset_timer();
		constraints = new Constraints(constraints_str, N);
		auto solution = initial_solution(N);
		return simulated_annealing(solution).perm;
	}


};

#ifdef LOCAL
int main(int argv, char *argc[]){
	auto procedure = [](istream &in){
		int N,K;
		in >> N >> K;
		// cerr << N << endl;
		// cerr << K << endl;
		vector<string> constraints;
		for(int i = 0 ; i < K ; i++){
			int vi,vj;
			in >> vi >> vj;
			stringstream ss;
			ss << vi << " " << vj;http://qiita.com/yohm/items/450fbae1ee0aebe1261e
			// cerr << vi << " " << vj << endl;
			constraints.push_back(ss.str());
		}
		auto res = ConstrainedPermutation().permute(N,constraints);
		// cerr << res << endl;

		cout << res.size() << endl;
		for(int i = 0 ; i < res.size() ; i++)
			cout << res[i] << endl;
	};
	if( argv >= 2 ){
		ifstream ifs(argc[1]);
		assert(ifs.is_open());
		procedure(ifs);
	}else{
		procedure(cin);
	}
}
#endif
