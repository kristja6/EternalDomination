#ifndef MAX_FLOW_H
#define MAX_FLOW_H

#include <bits/stdc++.h>

using namespace std;

struct NetworkEdge {
  int from, to;
  int cap;
};

// Modified EdmondKarp from ACM codebook by the great coder Šimon Lomič
class Network {
public:
  Network(int n): vertices(n), ng(vector<vector<int>>(n)), back(vector<int>(n)) { }
  void AddEdge(int from, int to, int capacity);
  int MaxFlow(int start, int target);

private:
  vector<NetworkEdge> edges;
  vector<vector<int>> ng; // indexes of edges
  vector<int> back; // indexes of edges for reconstructing augment path
  int vertices;

  bool AugementPath(int s, int t);
};

#endif
