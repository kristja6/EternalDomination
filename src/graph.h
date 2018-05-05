//
// Created by maty on 1/31/18.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <set>
#include <map>
#include <unordered_set>

#include "max-flow.h"

using namespace std;

struct ConfigGraph;

struct GraphVertex {
  vector<int> edges;
  int guards = 0;
};

// represents one graph
struct Graph {
  vector<GraphVertex> vertices;

  // load undirected graph from stdin
  void loadFromFile(const string & filename);

  // creates a list of graphs with all possible configurations of k guards
  // and keep only those, which are a dominating set
  void iterateCombinations(int index, int free, vector<Graph*> & result, Graph* curVertex, bool allowMultiple = true);

  // creates a configuration graph of all possible configurations of k guards on this graph.
  // Each vertice in the graph is one configuration
  ConfigGraph* createConfigurationGraph(int k, bool multipleGuards);

  // is one configuration passable to other by the rules of eternal domination
  bool oneMoveDistance(Graph & g, Graph & h, int k);

  // check whether the current configuration of guards induces a dominating set
  bool isDominatingSet();

  int size() const;

  void output();
};

struct ConfigGraphVertex {
  ConfigGraphVertex() = default;
  ConfigGraphVertex(Graph * g): g(g) {};
  vector<int> edges;
  Graph* g;
  // bitset of which vertices are safe
  vector<bool> safe;
  bool removed = false;
};

struct ConfigGraph {
  vector<ConfigGraphVertex> vertices;

  void outputAllUnremoved();

  void reduceToSafe();

  int size() const;
};

#endif //EDN_MAIN_H
