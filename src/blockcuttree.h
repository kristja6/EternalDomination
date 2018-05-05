//
// Created by maty on 4/22/18.
//

#ifndef EDN_BLOCKCUTTREE_H
#define EDN_BLOCKCUTTREE_H
#include <bits/stdc++.h>
#include "graph.h"

using namespace std;

struct InputGraphVertex {
  vector<int> edges;
  int low;
  vector<int> cliqueId;
  int time = -1;
  int parent = -1;
};

struct BlockCutTreeVertex {
  BlockCutTreeVertex(bool block, int size) : block(block), size(size) {}
  bool block; // is either a block or an articulation
  int size; // size of the block
  vector<int> edges;
};

struct BlockCutTree {
  vector<InputGraphVertex> inV;
  vector<BlockCutTreeVertex> vertices;
  Graph* in;
  vector<int> cliqueVertices;
  vector<int> cliqueEdges;
  bool connected;

  BlockCutTree(Graph* in): in(in) {
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
    for (int i = 0; i < cliqueVertices.size(); ++i) {
      if(cliqueVertices[i] > 0) {
        vertices.push_back(BlockCutTreeVertex(true, cliqueVertices[i]));
      }
    }

    // every vertex in more than one block has to be an articulation
    for (int i = 0; i < cliqueVertices.size(); ++i) {
      if (inV[i].cliqueId.size() > 1) {
        vertices.push_back(BlockCutTreeVertex(false, 0));

        for (int j = 0; j < inV[i].cliqueId.size(); ++j) {
          vertices.back().edges.push_back(inV[i].cliqueId[j]);
          vertices[inV[i].cliqueId[j]].edges.push_back(vertices.size() - 1);
        }
      }
    }
    // in the case the graph is 2-connected
    if (vertices.empty()) {
      vertices.push_back(BlockCutTreeVertex(true, in->size()));
    }
  }

  int time = 0;
  int curCliqueId = 0;

  // source: https://www.geeksforgeeks.org/biconnected-components/
  void markDfs() {
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

  void markDfsHelp(int u, vector<pair<int,int>> & edgesSt) {
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

  // check if it is a cactus with every articulation in two blocks
  bool isCactus() {
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

  int size() const { return (int)vertices.size(); }
};

#endif //EDN_BLOCKCUTTREE_H
