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
  vector<int> blockIds;
  int low = 0;
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
  vector<InputGraphVertex> inputVertices;
  vector<BlockCutTreeVertex> vertices;
  vector<int> blockVertexCounts;
  vector<int> blockEdgeCounts;
  bool connected = false;
  Graph* in = nullptr;

  BlockCutTree(Graph* in);

  int time = 0;
  int curCliqueId = 0;

  // source: https://www.geeksforgeeks.org/biconnected-components/
  void markDfs();
  void markDfsHelp(int u, vector<pair<int,int>> & edgesSt);

  // check if it is a cactus with every articulation in two blocks
  bool isCactus();

  int size() const { return (int)vertices.size(); }
};

#endif //EDN_BLOCKCUTTREE_H
