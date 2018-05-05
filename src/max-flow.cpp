#include "max-flow.h"

void Network::addEdge(int from, int to, int capacity) { // 2 edges, back is residual, accessible by ^1 (even/odd)
  ng[from].push_back(edges.size());
  edges.push_back(NetworkEdge{from, to, capacity});

  ng[to].push_back(edges.size());
  edges.push_back(NetworkEdge{to, from, 0});
}

bool Network::augementPath(int s, int t) { // source,sink
  for(int i = 0; i < vertices; ++ i) back[i] = -1;
  back[s] = -2;
  for(int i = 0; i < vertices; ++ i) fromS[i] = false;

  stack<int> q; q.push(s);
  while (!q.empty() && back[t] == -1) { // exists augment path to sink
    int u = q.top(); q.pop();
    fromS[u] = true;
    for (int i : ng[u]) {
      NetworkEdge & edge = edges[i];
      if (edge.cap && back[edge.to] == -1) { // has capacity
        back[edge.to] = i;
        q.push(edge.to);
      }
    }
  }
  return back[t] != -1;
}

int Network::maxFlow(int s, int t) {
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
