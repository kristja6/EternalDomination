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
  unordered_set<int> kn_ids;
  int deg() const { return edges.size(); }
  int visited = 0;
  int low;
  int depth;
  int childCount;
  int articulationComps = 1;
  int totalArtComps = 1;
  vector<int> cliqueId;
  int visitedEdges = 0;
  int time = -1;
  int parent = -1;

  // information for articulation
  bool holdsLeaves = false;
  int cliquesCnt; // basically same as articulationComps
  bool removed = false;
};

struct Clique {
  vector<int> articulations;
  int deg() const { return articulations.size(); }
  int size = 0;
  int artCnt = 0;
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
    /*markCliques();
    for (int i = 0; i < cliqueEdges.size(); ++i) {
      cliqueEdges[i] /= 2;
    }
    // find out size of every clique
    cliqueVertices = vector<int>(in->size());
    for (int i = 0; i < inV.size(); ++i) {
      for (int j = 0; j < inV[i].cliqueId.size(); ++j) {
        cliqueVertices[ inV[i].cliqueId[j] ] ++;
      }
    }*/
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

        //cout << "deg: " << endl;
        for (int j = 0; j < inV[i].cliqueId.size(); ++j) {
          vertices.back().edges.push_back(inV[i].cliqueId[j]);
          vertices[inV[i].cliqueId[j]].edges.push_back(vertices.size() - 1);
          //cout << "::" << inV[i].cliqueId[j] << endl;
        }
        //cout << vertices.back().edges.size() << endl;
      }
    }
    // in the case the graph is 2-connected
    if (vertices.empty()) {
      vertices.push_back(BlockCutTreeVertex(true, in->size()));
    }
  }

  // Hopcroft and Tarjan's algorithm
  void dfsArt(int i, int d, int parent = -1) {
    inV[i].visited = true;
    inV[i].depth = d;
    inV[i].low = d;
    inV[i].childCount = 0;

    int visitedChildren = 0;
    for (int j = 0; j < inV[i].deg(); ++ j) {
      const int u = inV[i].edges[j];
      if (!inV[u].visited) {
        dfsArt(u, d + 1, i);
        inV[i].childCount ++;
        if (inV[u].low >= inV[i].depth) {
          //vertices[i].isArticulation = true;
          // compute how much components would be left in G after removing i
          inV[i].articulationComps ++;
          inV[i].totalArtComps ++;
        }
        inV[i].low = min(inV[i].low, inV[u].low);
      } else if (u != parent) {
        inV[i].low = min(inV[i].low, inV[u].depth);
      }
    }
    //if (parent == -1 && vertices[i].childCount > 1) vertices[i].isArticulation = true;
    if (parent == -1) inV[i].totalArtComps = inV[i].articulationComps = inV[i].childCount;
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
          //cout << edgesSt.back().first << ", " << edgesSt.back().second << endl;

          edgesSt.pop_back();
          cliqueEdges[curCliqueId] ++;
        }
        if (j == 1) {
          cliqueVertices[curCliqueId] += edgeVertices.size();
          for (auto i = edgeVertices.begin(); i != edgeVertices.end(); ++ i) {
            inV[*i].cliqueId.push_back(curCliqueId);
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

  // returns max size so far
  void markingDfs(int i, int cliqueId, vector<int> & unmark, int size = 0) {
    // every time we try to visit this vertex, it must be through a different
    // vertex, therefore using a different edge.
    //cout << i << endl;
    // count for each passed edge. Starts with -1 because of the initial call

    if (inV[i].visited == cliqueId + 1 || inV[i].visited == 0)cliqueEdges[cliqueId] ++;
    if (inV[i].visited) return;

    //cout << "markingDfs: " << i << ", " << cliqueId << endl;
    inV[i].visited = cliqueId + 1;
    inV[i].cliqueId.push_back(cliqueId);
    if (inV[i].articulationComps >= 2) {
      unmark.push_back(i);
      return;
    }

    for (int j = 0; j < inV[i].deg(); ++ j) {
      const int u = inV[i].edges[j];
      // add the edge for articulation now, as it won't try recursing to other vertices
      if (inV[u].articulationComps >= 2) cliqueEdges[cliqueId] ++;
      markingDfs(u, cliqueId, unmark, size + 1);
    }
  }

  // mark every vertex with its cliqueId.
  // returns the number of cliques
  int markCliques() {
    for (int i = 0; i < inV.size(); ++ i) { inV[i].visited = false; }
    //cerr << "finding articulations" << endl;
    for (int i = 0; i < inV.size(); ++ i) { // find ariculations on all connected components
      if (!inV[i].visited) {
        dfsArt(i, 0);
      }
    }

    // clear visited
    for (int i = 0; i < inV.size(); ++ i) {
      inV[i].visited = false;
      inV[i].visitedEdges = 0;
    }

    int curCliqueId = 0;
    vector<int> unmark;
    vector<int> visit; // analyze the complexity here

    //cerr << "coloring by clique ids" << endl;
    for (int i = 0; i < inV.size(); ++ i) {
      // any nonarticulation vertex must be part of a clique. Mark it's component as a new clique
      if (!inV[i].visited && inV[i].articulationComps <= 1) {
        markingDfs(i, curCliqueId ++, unmark);
        cout << endl;
        // all articulations must be visited again, as they can be part of another clique
        while (!unmark.empty()) {
          int b = unmark.back();
          unmark.pop_back();
          //cerr << "unmarking: " << b << endl;
          inV[b].visited = false;
          inV[b].articulationComps --;
          // we simulate removing the visited clique by keeping its status as visited.
          // in case the articulation is no longer an articulation, it will be used to mark its clique in future
          if (inV[b].articulationComps == 1) visit.push_back(b);
        }
      }
    }

    //cerr << "marking previous articulations" << endl;
    // analyze this, should be still linear
    // only in case some of their neighbors hasn't been visited
    while (!visit.empty()) {
      // check neighbors
      bool hasUnvisitedNeighbor = false;
      for (int i = 0; i < inV[visit.back()].deg(); ++i) {
        const int u = inV[visit.back()].edges[i];
        if (!inV[u].visited) {
          hasUnvisitedNeighbor = true;
          break;
        }
      }

      if (hasUnvisitedNeighbor) {
        markingDfs(visit.back(), curCliqueId++, unmark);
        visit.pop_back();

        while (!unmark.empty()) {
          int b = unmark.back();
          unmark.pop_back();
          //cerr << "unmarking: " << b << endl;
          //cerr << inV[b].articulationComps << endl;
          inV[b].visited = false;
          inV[b].articulationComps --;
          if (inV[b].articulationComps == 1) visit.push_back(b);
        }
      } else {
        visit.pop_back();
      }

    }
    return curCliqueId;
  }

  int eternalDominationNumber() {
    cerr << "finding cliques" << endl;
    int cliquesCnt = markCliques();
    cerr << "total cliques: " << cliquesCnt << endl;

    cerr << "building clique info" << endl;
    // for every clique, we want to keep a list of its articulations
    vector<Clique> cliques = vector<Clique>(cliquesCnt);
    for (int i = 0; i < inV.size(); ++i) {
      if (inV[i].totalArtComps >= 2) {
        for (int j = 0; j < inV[i].cliqueId.size(); ++j) {
          const int curClique = inV[i].cliqueId[j];

          cliques[curClique].articulations.push_back(i);
        }
      }

      inV[i].cliquesCnt = (int)inV[i].cliqueId.size();
    }

    cerr << "output of cliques: " << endl;
    cerr << "---" << endl;
    for (int i = 0; i < cliques.size(); ++i) {
      cerr << i << ": ";
      for (int j = 0; j < inV.size(); ++j) {
        bool has = false;
        for (int k = 0; k < inV[j].cliqueId.size(); ++k) {
          if (inV[j].cliqueId[k] == i) has = true;
        }
        if (has)
          cerr << j << " ";
      }
      cerr << endl;
    }
    cerr << inV[1].visited << endl;

    // set clique sizes
    for (int i = 0; i < inV.size(); ++i) {
      for (int j = 0; j < inV[i].cliqueId.size(); ++j) {
        cliques[ inV[i].cliqueId[j] ].size ++;
      }
    }

    cerr << "finding leaf cliques" << endl;
    // perform reduction on the cliques
    stack<int> leaves;
    vector<bool> inStack(cliquesCnt, false);
    for (int i = 0; i < cliques.size(); ++i) {
      cliques[i].artCnt = cliques[i].articulations.size();
      if (cliques[i].deg() <= 1) {
        cerr << "pushing: " << i << endl;
        leaves.push(i);
        inStack[i] = true;
      }
    }

    cerr << "performing reductions" << endl;
    cerr << "---" << endl;
    int edn = 0;
    vector<bool> removedCliques(cliquesCnt, false);



    while (!leaves.empty()) {
      int cur = leaves.top();
      leaves.pop();

      cerr << "popping clique" << endl;
      cerr << "id: " << cur << endl;
      cerr << "size: " << cliques[cur].size << endl;
      cerr << "arts: " << cliques[cur].artCnt << endl;

      // this clique must be contained inside some other clique
      if (cliques[cur].size - cliques[cur].artCnt <= 0) {
        cerr << "not a clique!" << endl;

        int validArt = -1; // some valid articulation
        // find some valid articulation
        for (int i = cliques[cur].articulations.size() - 1; i >= 0; --i) {
          cerr << cliques[cur].articulations[i] << endl;
          if (!inV[ cliques[cur].articulations[i] ].removed) {
            validArt = cliques[cur].articulations[i];
          }
        }
        removedCliques[cur] = true;
        if (validArt != -1) {
          cerr << "no valid articulation" << endl;
          continue;
        }

        inV[validArt].cliquesCnt --;

        /*if (vertices[validArt].cliquesCnt == 2) {

        }*/
        // add the clique that contains this articulation if it is a leaf
        // could have been only one valid articulation
        cerr << "finding leaf: " << endl;
        for (int i = 0; i < inV[validArt].cliqueId.size(); ++i) {
          const int curC = inV[validArt].cliqueId[i];
          cliques[curC].artCnt --;

          cerr << curC << endl;

          if (!removedCliques[curC] && cliques[curC].artCnt <= 1 && !inStack[curC]) {
            cerr << "pushing: " << curC << endl;
            inStack[curC] = true;
            leaves.push(curC);
          }
        }
        continue;
      }

      // if has no valid articulation, then it is an isolated clique
      if (cliques[cur].artCnt == 0) {
        edn ++;
        cerr << "edn ++" << endl;
        continue;
      }

      // find the one valid articulation
      for (int i = cliques[cur].articulations.size() - 1; i >= 0; --i) {
        const int curArt = cliques[cur].articulations[i];
        if (inV[curArt].cliquesCnt <= 1) continue; // not an articulation
        bool removed = false;
        bool removeTheArticulation = true;

        if (inV[curArt].cliquesCnt == 2) { // a leaf clique, Reduction 1, add one guard
          cerr << "a leaf clique with a single neighbor" << endl;
          if (!inV[curArt].holdsLeaves) {// if it hold leaves, edn was already increased
            edn++;
            cerr << "edn ++" << endl;
          }

          removed = true;
        } else if (inV[curArt].cliquesCnt > 2) {
          removeTheArticulation = false;
          // in this case, don't remove the articulation as it will be removed when it becomes a leaf with
          // a single neighbor
          cerr << "has multiple cliques: " << inV[curArt].cliquesCnt << endl;
          if (!inV[curArt].holdsLeaves) {// if it hold leaves, edn was already increased
            edn++;
            cerr << "edn ++" << endl;
          }
          inV[curArt].holdsLeaves = true;

          removed = true;
        }

        if (removed) {
          // TODO: check complexity
          // remove all articulations on this clique
          if (removeTheArticulation) {
            for (int j = 0; j < cliques[cur].articulations.size(); ++j) {
              const int curArt = cliques[cur].articulations[j];
              inV[cliques[cur].articulations[j]].removed = true;

              // make all the cliques one size smaller
              for (int k = 0; k < inV[curArt].cliqueId.size(); ++k) {
                cliques[inV[curArt].cliqueId[k]].size--;
                cliques[inV[curArt].cliqueId[k]].artCnt--;
              }
            }
          }

          // remove the current clique


          // if it is no longer an articulation, check the other clique
          if (inV[curArt].cliquesCnt == 1) {
            // find the one still existing clique
            for (int j = 0; j < inV[curArt].cliqueId.size(); ++j) {
              cerr << "cl: " << inV[curArt].cliqueId[j] << endl;
              if (!removedCliques[inV[curArt].cliqueId[j]] && cliques[ inV[curArt].cliqueId[j] ].artCnt <= 1 && !inStack[inV[curArt].cliqueId[j]]) {
                cerr << "pushing: " << inV[curArt].cliqueId[j] << endl;
                inStack[inV[curArt].cliqueId[j]] = true;
                leaves.push(inV[curArt].cliqueId[j]);
              }
            }
          }

          break;
        }
      }
    }

    return edn;
  }

  // check if it is a cactus with every articulation in two blocks
  bool isCactus() {
    for (int i = 0; i < vertices.size(); ++i) {
      if (!vertices[i].block && vertices[i].edges.size() > 2) return false;
    }
    /* G is a cactus iff |E| */
    int v = 0;
    int e = 0;
    for (int i = 0; i < cliqueEdges.size(); ++i) {
      if (cliqueEdges[i] == 0) continue;
      else if (cliqueEdges[i] == 1) continue;
      else {
        //cout << i << ": " << cliqueEdges[i] << ", " << cliqueVertices[i] << endl;
        if (cliqueEdges[i] != cliqueVertices[i]) {
          //cout << "not a cactus" << endl;
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
