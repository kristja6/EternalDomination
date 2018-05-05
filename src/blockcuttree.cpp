//
// Created by maty on 4/22/18.
//

#include "blockcuttree.h"

BlockCutTree::BlockCutTree(Graph *in) : in(in) {
  for (int i = 0; i < in->size(); ++i) {

    inV.push_back(InputGraphVertex());
    for (int j = 0; j < in->vertices[i].edges.size(); ++j) {
      inV[i].edges.push_back(in->vertices[i].edges[j]);
    }
  }
  cliqueEdges = vector<int>(inV.size(), 0);
  cliqueVertices = vector<int>(inV.size(), 0);
  markDfs();
  // add all blocks
  // the first n blocks are ordered by their ids
  for (int cliqueVertex : cliqueVertices) {
    if(cliqueVertex > 0) {
      vertices.push_back(BlockCutTreeVertex(true, cliqueVertex));
    }
  }

  // every vertex in more than one block has to be an articulation
  for (int i = 0; i < cliqueVertices.size(); ++i) {
    if (inV[i].cliqueId.size() > 1) {
      vertices.push_back(BlockCutTreeVertex(false, 0));

      for (int j : inV[i].cliqueId) {
        vertices.back().edges.push_back(j);
        vertices[j].edges.push_back(vertices.size() - 1);
      }
    }
  }
  // in the case the graph is 2-connected
  if (vertices.empty()) {
    vertices.push_back(BlockCutTreeVertex(true, in->size()));
  }
}

void BlockCutTree::markDfs() {
  int cnt = 0;
  for (int i = 0; i < inV.size(); ++i) {
    if (inV[i].time == -1) {
      cnt ++;
      vector<pair<int,int>> edgesSt;
      markDfsHelp(i, edgesSt);

      int j = 0;
      unordered_set<int> edgeVertices;
      while (!edgesSt.empty()) {
        j = 1;
        edgeVertices.insert(edgesSt.back().first);
        edgeVertices.insert(edgesSt.back().second);
        edgesSt.pop_back();
        cliqueEdges[curCliqueId] ++;
      }
      if (j == 1) {
        cliqueVertices[curCliqueId] += edgeVertices.size();
        for (auto v = edgeVertices.begin(); v != edgeVertices.end(); ++ v) {
          inV[*v].cliqueId.push_back(curCliqueId);
        }
        curCliqueId ++;
      }
    }
  }
  connected = (cnt == 1);
}

void BlockCutTree::markDfsHelp(int u, vector<pair<int, int>> &edgesSt) {
  time ++;
  inV[u].time = time;
  inV[u].low = time;

  int children = 0;

  for (int j = 0; j < inV[u].edges.size(); ++j) {
    const int v = inV[u].edges[j];

    if (inV[v].time == -1) {
      children ++;
      inV[v].parent = u;
      edgesSt.push_back({u, v});
      markDfsHelp(v, edgesSt);
      inV[u].low = min(inV[u].low, inV[v].low);

      // If u is an articulation, pop all edges from stack till u -- v
      if ((inV[u].time == 1 && children > 1)
          || (inV[u].time > 1 && inV[v].low >= inV[u].time)) {
        unordered_set<int> edgeVertices;
        while (edgesSt.back().first != u || edgesSt.back().second != v) {
          edgeVertices.insert(edgesSt.back().first);
          edgeVertices.insert(edgesSt.back().second);

          edgesSt.pop_back();
          cliqueEdges[curCliqueId] ++;
        }
        edgeVertices.insert(edgesSt.back().first);
        edgeVertices.insert(edgesSt.back().second);

        edgesSt.pop_back();
        cliqueEdges[curCliqueId] ++;
        cliqueVertices[curCliqueId] += edgeVertices.size();
        for (auto i = edgeVertices.begin(); i != edgeVertices.end(); ++ i) {
          inV[*i].cliqueId.push_back(curCliqueId);
        }
        curCliqueId ++;
      }
    } else if (v != inV[u].parent && inV[v].time < inV[u].time) {
      inV[u].low = min(inV[u].low, inV[v].time);
      edgesSt.push_back({u, v});
    }
  }
}

bool BlockCutTree::isCactus() {
  for (int i = 0; i < vertices.size(); ++i) {
    if (!vertices[i].block && vertices[i].edges.size() > 2) return false;
  }
  int v = 0;
  int e = 0;
  for (int i = 0; i < cliqueEdges.size(); ++i) {
    if (cliqueEdges[i] == 0 || cliqueEdges[i] == 1) continue;
    else {
      if (cliqueEdges[i] != cliqueVertices[i]) {
        return false;
      }
    }
  }
  // also has to be connected
  return connected;
}
