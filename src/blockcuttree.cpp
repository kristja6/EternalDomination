//
// Created by maty on 4/22/18.
//

#include "blockcuttree.h"

BlockCutTree::BlockCutTree(Graph *in) : in(in) {
  // copy the vertices of the input graph
  for (int i = 0; i < in->Size(); ++i) {
    inputVertices.push_back(InputGraphVertex());
    for (int j = 0; j < in->vertices[i].edges.size(); ++j) {
      inputVertices[i].edges.push_back(in->vertices[i].edges[j]);
    }
  }

  blockEdgeCounts = vector<int>(inputVertices.size(), 0);
  blockVertexCounts = vector<int>(inputVertices.size(), 0);

  // mark every vertex with its blocks
  MarkDfs();

  // add all blocks into the block-cut tree
  // the first n blocks are ordered by their ids
  for (int blockCount : blockVertexCounts) {
    if(blockCount > 0) {
      vertices.push_back(BlockCutTreeVertex(true, blockCount));
    }
  }

  // every vertex in more than one block has to be an articulation
  for (int i = 0; i < blockVertexCounts.size(); ++i) {
    if (inputVertices[i].blockIds.size() > 1) {
      vertices.push_back(BlockCutTreeVertex(false, 0));

      for (int blockId : inputVertices[i].blockIds) {
        vertices.back().edges.push_back(blockId);
        // first vertices are the blocks
        vertices[blockId].edges.push_back(vertices.size() - 1);
      }
    }
  }

  // in the case the graph is 2-connected and no articulations were found
  if (vertices.empty()) {
    vertices.push_back(BlockCutTreeVertex(true, in->Size()));
  }
}

// source: https://www.geeksforgeeks.org/biconnected-components/
void BlockCutTree::MarkDfs() {
  int cnt = 0;
  for (int i = 0; i < inputVertices.size(); ++i) {
    if (inputVertices[i].time == -1) {
      cnt ++;
      vector<pair<int,int>> edgesSt;
      MarkDfsHelp(i, edgesSt);

      int j = 0;
      unordered_set<int> edgeVertices;
      while (!edgesSt.empty()) {
        j = 1;
        edgeVertices.insert(edgesSt.back().first);
        edgeVertices.insert(edgesSt.back().second);
        edgesSt.pop_back();
        blockEdgeCounts[curCliqueId] ++;
      }
      if (j == 1) {
        blockVertexCounts[curCliqueId] += edgeVertices.size();
        for (auto v = edgeVertices.begin(); v != edgeVertices.end(); ++ v) {
          inputVertices[*v].blockIds.push_back(curCliqueId);
        }
        curCliqueId ++;
      }
    }
  }
  connected = (cnt == 1);
}

void BlockCutTree::MarkDfsHelp(int u, vector<pair<int, int>> &edgesSt) {
  time ++;
  inputVertices[u].time = time;
  inputVertices[u].low = time;

  int children = 0;

  for (int j = 0; j < inputVertices[u].edges.size(); ++j) {
    const int v = inputVertices[u].edges[j];

    if (inputVertices[v].time == -1) {
      children ++;
      inputVertices[v].parent = u;
      edgesSt.push_back({u, v});
      MarkDfsHelp(v, edgesSt);
      inputVertices[u].low = min(inputVertices[u].low, inputVertices[v].low);

      // If u is an articulation, pop all edges from stack till (u, v)
      if ((inputVertices[u].time == 1 && children > 1)
          || (inputVertices[u].time > 1 && inputVertices[v].low >= inputVertices[u].time)) {
        unordered_set<int> edgeVertices;
        while (edgesSt.back().first != u || edgesSt.back().second != v) {
          edgeVertices.insert(edgesSt.back().first);
          edgeVertices.insert(edgesSt.back().second);

          edgesSt.pop_back();
          blockEdgeCounts[curCliqueId] ++;
        }
        edgeVertices.insert(edgesSt.back().first);
        edgeVertices.insert(edgesSt.back().second);

        edgesSt.pop_back();
        blockEdgeCounts[curCliqueId] ++;
        blockVertexCounts[curCliqueId] += edgeVertices.size();
        for (auto i = edgeVertices.begin(); i != edgeVertices.end(); ++ i) {
          inputVertices[*i].blockIds.push_back(curCliqueId);
        }
        curCliqueId ++;
      }
    } else if (v != inputVertices[u].parent && inputVertices[v].time < inputVertices[u].time) {
      inputVertices[u].low = min(inputVertices[u].low, inputVertices[v].time);
      edgesSt.push_back({u, v});
    }
  }
}

bool BlockCutTree::IsCactus() {
  for (int i = 0; i < vertices.size(); ++i) {
    if (!vertices[i].block && vertices[i].edges.size() > 2) return false;
  }
  for (int i = 0; i < blockEdgeCounts.size(); ++i) {
    if (blockEdgeCounts[i] == 0 || blockEdgeCounts[i] == 1) continue;
    else {
      if (blockEdgeCounts[i] != blockVertexCounts[i]) {
        return false;
      }
    }
  }
  // also has to be connected
  return connected;
}

bool BlockCutTree::IsCliqueTree() {
  for (int i = 0; i < blockEdgeCounts.size(); ++i) {
    if (blockEdgeCounts[i] != (blockVertexCounts[i]*(blockVertexCounts[i]-1))/2) {
      return false;
    }
  }
  // also has to be connected
  return connected;
}
