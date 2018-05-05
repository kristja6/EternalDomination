#ifndef MAX_FLOW_H
#define MAX_FLOW_H

#include <bits/stdc++.h>

using namespace std;

struct NetworkEdge {
  int from, to;
  int cap;
};

// Modified EdmondKarp from ACM codebook (source: Simon Lomic)
struct Network {
  vector<NetworkEdge> edges;
  vector<vector<int>> ng; // indexes of edges
  vector<int> back; // indexes of edges for reconstructing augment path
  vector<bool> fromS; // for minCut: can get from s when augment not found?
  int vertices;
  Network(int n): vertices(n), ng(vector<vector<int>>(n)), back(vector<int>(n)), fromS(vector<bool>(n)) {
  }

  void addEdge(int from, int to, int capacity);

  bool augementPath(int s, int t);

  int maxFlow(int s, int t);
};

#endif
