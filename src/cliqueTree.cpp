//
// Created by maty on 5/9/18.
//

#include "cliqueTree.h"

CliqueTree::CliqueTree(const BlockCutTree &bc): bc(bc), size(vector<int>(bc.Size())), deg(vector<int>(bc.Size()))
    , cliques(vector<int>(bc.Size(), 0)), deleted(vector<bool>(bc.Size(), false)) {
  for (int i = 0; i < size.size(); ++i) {
    if (bc.vertices[i].block) {
      size[i] = bc.vertices[i].size;
    } else {
      size[i] = -1;
    }
    deg[i] = (int)bc.vertices[i].edges.size();
  }
}

int CliqueTree::EDN() {
  stack<int> st;
  for (int i = 0; i < deg.size(); ++i) {
    if (deg[i] <= 1) st.push(i);
  }
  int edn = 0;
  while (!st.empty()) {
    int v = st.top();
    st.pop();
    int u = -1;
    // find the neighbor of v
    for (int i = 0; i < bc.vertices[v].edges.size(); ++i) {
      const int j = bc.vertices[v].edges[i];
      if (!deleted[j]) {
        u = j;
        break;
      }
    }
    if (size[v] >= 1) { // an undeleted clique
      Clique(edn, v, u);
    } else if (size[v] == -1) { // an articulation
      Articulation(edn, v, u);
    }
    deleted[v] = true;
    if (deg[v] > 0) {
      deg[u] --;
    }
    if (deg[u] == 1) {
      st.push(u);
    }
  }
  return edn;
}

void CliqueTree::Clique(int &edn, int v, int u) {
  if (size[v] > 1) { // an undeleted clique
    if (deg[v] == 0) { // an isolated Kn
      edn ++;
    } else { // a leaf clique
      cliques[u] ++;
    }
  } else if (size[v] == 1) {
    if (deg[v] == 0) { // an isolated vertex
      edn ++;
    }
  }
}

void CliqueTree::Articulation(int &edn, int v, int u) {
  if (size[v] == -1) { // an articulation
    if (cliques[v] == 1) {
      size[u] --;
      edn ++;
    } else if (cliques[v] > 1) {
      edn ++;
    }
  }
}
