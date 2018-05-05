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

  void addEdge(int from, int to, int capacity) { // 2 edges, back is residual, accessible by ^1 (even/odd)
    ng[from].push_back(edges.size());
    edges.push_back(NetworkEdge{from, to, capacity});

    ng[to].push_back(edges.size());
    edges.push_back(NetworkEdge{to, from, 0});
  }

  bool augementPath(int s, int t) { // source,sink
    for(int i = 0; i < vertices; ++ i) back[i] = -1;
    back[s] = -2;
    for(int i = 0; i < vertices; ++ i) fromS[i] = 0;

    stack<int> q; q.push(s);
    while (!q.empty() && back[t] == -1) { // exists augment path to sink
      int u = q.top(); q.pop();
      fromS[u]=1;
      for(int i = 0; i < ng[u].size(); ++ i) {
        NetworkEdge & edge = edges[ng[u][i]];
        if (edge.cap && back[edge.to] == -1) { // has capacity
          back[edge.to] = ng[u][i];
          q.push(edge.to);
        }
      }
    }
    return back[t] != -1;
  }

  int maxFlow(int s, int t) {
    int maxFlow = 0;
    while (augementPath(s, t)) {
      int flow = 1<<30, node = t; // from sink to source(=-2)
      // find size of the flow = min capacity on the way:
      while (back[node] != -2) {
        NetworkEdge & edge = edges[back[node]];
        flow = min(flow, edge.cap);
        node = edge.from;
      }
      // push the flow:
      node=t;
      while (back[node] != -2) {
        NetworkEdge & edge = edges[back[node]],
            & edge2 = edges[back[node]^1];
        edge.cap -= flow;
        edge2.cap += flow;
        node = edge.from; // going back
      }

      maxFlow += flow;
    }
    return maxFlow;
  }
};

#endif
