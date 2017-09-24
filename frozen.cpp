#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

const int INF = 12345;
const double TIME_LIMIT = 9.75;

const unsigned int RANDMAX = -1;

unsigned int randxor() {
  static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
  unsigned int t;
  t = (x ^ (x << 11));
  x = y;
  y = z;
  z = w;
  return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

template <class T> ostream &operator<<(ostream &os, const vector<T> &v) {
  for (int i = 0; i < v.size(); i++) {
    os << v[i] << (i + 1 == v.size() ? "" : " ");
  }
  return os;
}

namespace Timer {
bool is_started = false;
unsigned long long int cycle_per_sec = 2500000000;
unsigned long long int beginCycle;
unsigned long long int get_cycle() {
  unsigned int low, high;
  __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
  return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}
double time_elapsed() {
  assert(is_started);
  return (double)(get_cycle() - beginCycle) / cycle_per_sec;
}
void reset_timer() {
  is_started = true;
  beginCycle = get_cycle();
}
bool is_TLE(double limit) {
  assert(is_started);
  return time_elapsed() >= limit;
}
}; // namespace Timer
using namespace Timer;

template <class T> string unpack(const T &v) {
  stringstream ss;
  ss << v;
  return ss.str();
}
template <class T, class... Args> string unpack(const T &v, Args... args) {
  stringstream ss;
  ss << v << " " << unpack(args...);
  return ss.str();
}

template <class... Args> void log(int line, Args... args) {
  char time[16];
  sprintf(time, "%2.4f", time_elapsed());
  cerr << time << "s " << unpack(args...);
  if (line >= 0) {
    cerr << " (line " << line << ")";
  }
  cerr << endl;
}

#ifdef DEBUG
#define LOG(...)
#else
#define LOG(...)
#undef assert
#define assert(x)
#endif

#ifdef ANALYSIS
#define ANALYSIS_LOG(...) log(-1, "[ANALYSIS]", __VA_ARGS__)
#else
#define ANALYSIS_LOG(...)
#endif

#ifdef ENABLE_FIZZY_ANALYSIS
#define FIZZY_ANALYSIS_LOG(...) log(-1, "[ANALYSIS]", __VA_ARGS__)
#else
#define FIZZY_ANALYSIS_LOG(...)
#endif

struct Constraint {
  int i, j;
  Constraint(int i, int j) : i(i), j(j) {}
  bool operator<(const Constraint &op) {
    if (this->i != op.i)
      return this->i < op.i;
    return this->j < op.j;
  }
};

struct Constraints {
  vector<Constraint> raw;
  int graph[1000][1000];
  int graph_size[1000];
  int reversed_graph[1000][1000];
  int reversed_graph_size[1000];
  int matrix[1000][1000];
  int N;

  Constraints(vector<string> raw_str, int N) {
    vector<Constraint> raw;
    for (auto &&s : raw_str) {
      stringstream ss(s);
      int i, j;
      ss >> i >> j;
      raw.push_back(Constraint(i, j));
    }
    constructor(raw, N);
  }

  void constructor(vector<Constraint> raw, int N) {
    this->N = N;
    this->raw = raw;
    for (auto &&c : raw) {
      graph[c.i][graph_size[c.i]++] = c.j;
      reversed_graph[c.j][reversed_graph_size[c.j]++] = c.i;
      matrix[c.i][c.j] = 1;
    }
  }
  int get_N() { return N; }
  int get_K() { return raw.size(); }
};

struct Solution {
public:
  vector<int> perm;
  vector<int> pos;
  Constraints *constraints;
  int score;

  int real_score() { return 1000000 * score / constraints->raw.size(); }
  Solution(vector<int> perm, Constraints *constraints)
      : perm(perm), constraints(constraints) {
    score = evaluate();
  }

  int score_for_swap(int pi, int pj) {
    if (perm[pi] > perm[pj])
      swap(pi, pj);
    assert(perm[pi] < perm[pj]);

    int sum = 0;
    for (int i = 0; i < constraints->graph_size[pi]; i++) {
      if (perm[pi] < perm[constraints->graph[pi][i]])
        sum--;
      if (perm[pj] < perm[constraints->graph[pi][i]])
        sum++;
    }
    for (int i = 0; i < constraints->graph_size[pj]; i++) {
      if (perm[pj] < perm[constraints->graph[pj][i]])
        sum--;
      if (perm[pi] < perm[constraints->graph[pj][i]])
        sum++;
    }
    for (int i = 0; i < constraints->reversed_graph_size[pi]; i++) {
      if (perm[constraints->reversed_graph[pi][i]] < perm[pi])
        sum--;
      if (perm[constraints->reversed_graph[pi][i]] < perm[pj])
        sum++;
    }
    for (int i = 0; i < constraints->reversed_graph_size[pj]; i++) {
      if (perm[constraints->reversed_graph[pj][i]] < perm[pj])
        sum--;
      if (perm[constraints->reversed_graph[pj][i]] < perm[pi])
        sum++;
    }
    sum += constraints->matrix[pj][pi];
    sum += constraints->matrix[pi][pj];
    return sum;
  }

  int update(int pi, int pj) {
    int inc = score_for_swap(pi, pj);
    swap(perm[pi], perm[pj]);
    score += inc;
    return inc;
  }

  void revert(int pi, int pj, int previous_score_diff) {
    swap(perm[pi], perm[pj]);
    score -= previous_score_diff;
  }

  int evaluate() {
    int ans = 0;
    for (auto &&c : constraints->raw) {
      ans += perm[c.i] < perm[c.j];
    }
    return ans;
  }
};

class ConstrainedPermutation {
  Constraints *constraints;

  Solution initial_solution(int N) {
    vector<pair<int, int>> height(N);
    for (int i = 0; i < N; i++)
      height[i].second = i;

    for (int i = 0; i < constraints->raw.size(); i++) {
      height[constraints->raw[i].j].first++;
    }
    sort(height.begin(), height.end());

    vector<int> p(N);
    for (int i = 0; i < N; i++)
      p[height[i].second] = i;

    return Solution(p, constraints);
  }

  double predicted_max_t(double K) { return 1.044e9 * pow(K, -0.398695); }

  Solution simulated_annealing(Solution solution) {

    const double temprature_begin = 0.00001;
    const double template_end = 0;
    const int max_t = predicted_max_t(constraints->get_K());
    ANALYSIS_LOG("max_t", max_t);
    int t = 0;
    ANALYSIS_LOG("sa_start_time", time_elapsed());
    Solution best_solution = solution;

    int metrics_last_updated = 0;
    int metrics_last_updated_by_probability = 0;

    while (!is_TLE(TIME_LIMIT)) {
      int vi = randxor() % constraints->get_N();
      int vj = randxor() % constraints->get_N();
      if (vi == vj)
        continue;

      if (t % 1000 == 0) {
        FIZZY_ANALYSIS_LOG("score_incomplete", solution.real_score(), t,
                           time_elapsed());
      }

      int score_diff = solution.update(vi, vj);

      bool do_update = false;
      if (score_diff == 0) {
      }
      if (score_diff >= 0) {
        do_update = true;
      } else {
      }

      if (!do_update) {
        solution.revert(vi, vj, score_diff);
      }
      if (best_solution.score < solution.score) {
        best_solution = solution;
        FIZZY_ANALYSIS_LOG("updated", solution.real_score(), t, time_elapsed());
        LOG("update!", score_diff, solution.real_score(), "(", t, ")");
        metrics_last_updated = t;
      }
      t++;
    }
    ANALYSIS_LOG("last_updated", metrics_last_updated);
    ANALYSIS_LOG("last_updated_by_probability",
                 metrics_last_updated_by_probability);

    ANALYSIS_LOG("final_t", t);
    ANALYSIS_LOG("sa_finish_time", time_elapsed());
    return best_solution;
  }

public:
  vector<int> permute(int N, vector<string> constraints_str) {

    reset_timer();
    constraints = new Constraints(constraints_str, N);
    auto solution = initial_solution(N);
    auto res = simulated_annealing(solution);
    ANALYSIS_LOG("final_score", res.real_score());
    return res.perm;
  }
};

#ifdef LOCAL
int main(int argv, char *argc[]) {
  reset_timer();
  ANALYSIS_LOG("compiled_datetime", __DATE__, __TIME__, "CDT");
  ANALYSIS_LOG("sourcefile_name", __FILE__);
  ANALYSIS_LOG("compiler_version", __VERSION__);
  ANALYSIS_LOG("program_name", argc[0]);
  if (argv >= 2)
    ANALYSIS_LOG("file_name", argc[1]);
  else
    ANALYSIS_LOG("file_name", "stdin");
  auto procedure = [](istream &in) {
    int N, K;
    in >> N >> K;
    ANALYSIS_LOG("N", N);
    ANALYSIS_LOG("K", K);
    vector<string> constraints;
    for (int i = 0; i < K; i++) {
      int vi, vj;
      in >> vi >> vj;
      stringstream ss;
      ss << vi << " " << vj;
      constraints.push_back(ss.str());
    }
    auto res = ConstrainedPermutation().permute(N, constraints);

#ifndef NO_OUTPUT
    cout << res.size() << endl;
    for (int i = 0; i < res.size(); i++)
      cout << res[i] << endl;
#endif
  };
  if (argv >= 2) {
    ifstream ifs(argc[1]);
    assert(ifs.is_open());
    procedure(ifs);
  } else {
    procedure(cin);
  }
}
#endif
