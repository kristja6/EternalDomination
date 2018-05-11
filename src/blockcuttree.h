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
  int size; // Size of the block
  vector<int> edges;
};

class BlockCutTree {
public:
  BlockCutTree(Graph* in);
  // check if it is a cactus with every articulation in two blocks
  bool IsCactus();
  bool IsCliqueTree();
  int Size() const { return (int)vertices.size(); }

  vector<InputGraphVertex> inputVertices;
  vector<BlockCutTreeVertex> vertices;
  vector<int> blockVertexCounts;
  vector<int> blockEdgeCounts;

private:
  bool connected = false;
  Graph* in = nullptr;

  int time = 0;
  int curCliqueId = 0;

  // source: https://www.geeksforgeeks.org/biconnected-components/
  void MarkDfs();
  void MarkDfsHelp(int u, vector<pair<int, int>> &edgesSt);
};

#endif //EDN_BLOCKCUTTREE_H
