//
// Created by maty on 4/22/18.
//

#include "cactus.h"

Cactus::Cactus(const BlockCutTree &bc) :
    bc(bc), size(vector<int>(bc.size())), deg(vector<int>(bc.size()))
    , cliques(vector<int>(bc.size(), 0)), deleted(vector<bool>(bc.size(), false)) {
  for (int i = 0; i < size.size(); ++i) {
    if (bc.vertices[i].block) {
      size[i] = bc.vertices[i].size;
    } else {
      size[i] = -1;
    }
    deg[i] = bc.vertices[i].edges.size();
  }
}

int Cactus::EGC() {
  stack<int> st;
  for (int i = 0; i < deg.size(); ++i) {
    if (deg[i] <= 1) st.push(i);
  }
  int g = 0;
  while (!st.empty()) {
    int v = st.top();
    st.pop();
    int u = -1;
    for (int i = 0; i < bc.vertices[v].edges.size(); ++i) {
      const int j = bc.vertices[v].edges[i];
      if (!deleted[j]) {
        u = j;
        break;
      }
    }
    bool del = false;

    if (size[v] != -1) {
      Block(u, v, g, st, del);
    } else {
      Articulation(u, v, g, st, del);
    }

    if (del && deg[v] > 0) {
      deg[u] --;
      deleted[v] = true;
      if (deg[u] == 1) {
        st.push(u);
      }
    }
  }
  return g;
}

void Cactus::Block(int u, int v, int &g, stack<int> &st, bool &del) {
  if (size[v] > 3) {
    LeafCycle(v, g, st, del);
  } else if (size[v] <= 3 && size[v] >= 2) {
    if (deg[v] == 0) g ++; // a disjoint clique
    else cliques[u] ++;
    del = true;
  } else if (size[v] == 1) {
    if (deg[v] == 0) {
      g ++;
    }
    del = true;
  } else if (size[v] == 0) {
    del = true;
  }
}

void Cactus::Articulation(int u, int v, int &g, stack<int> &st, bool &del) {
  if (cliques[v] == 1 && size[u] >= 3) {
    size[u] --;
    g ++;
    del = true;
  } else if (cliques[v] == 1 && size[u] == 2) {
    size[u] --;
    g ++;
    del = true;
  } else if (cliques[v] == 1 && size[u] == 1) {
    del = true;
  } else if (cliques[v] > 1) {
    g ++;
    del = true;
  } else if (cliques[v] == 0) {
    del = true;
  }
}

void Cactus::LeafCycle(int v, int &g, stack<int> &st, bool &del) {
  if (size[v] % 3 == 0) {
    g += size[v]/3 - 1;
    size[v] = 3;
    st.push(v);
  } else if (size[v] % 3 == 1) {
    g += (size[v]-1)/3;
    if (deg[v] == 0) { // The case is a disjoint C_{3k + 1}
      g ++;
    }
    del = true;
  } else if (size[v] % 3 == 2) {
    g += (size[v]-2)/3;
    size[v] = 2;
    st.push(v);
  }
}
