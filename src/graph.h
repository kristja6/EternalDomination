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
};

class Graph {
public:
  Graph(bool outputProgress): outputProgress(outputProgress) {}
  void LoadFromFile(const string &filename);
  int Size() const;
  // creates a configuration graph of all possible configurations of k guards on this graph.
  // Each vertice in the graph is one configuration
  ConfigGraph* CreateConfigurationGraph(int k, bool multipleGuards, bool heuristics);

  vector<GraphVertex> vertices;
private:

  long long NchooseK(long long n, long long k);

  // creates a list of graphs with all possible configurations of k guards
  // and keep only those, which are a dominating set
  void IterateCombinations(int index, int free, vector<vector<int> *> &result, vector<int> *curConfig,
                           bool allowMultiple = true);
  long long combinationsCounter;
  long long combinationsTotal;
  int currentMaxGuards;


  // is one configuration passable to other by the rules of eternal domination
  bool OneMoveDistance(const vector<int> &g, const vector<int> &h, int k);
  // check whether the current configuration of guards induces a dominating set
  bool IsDominatingSet(const vector<int> &input);

  bool outputProgress;
};

struct ConfigGraphVertex {
  ConfigGraphVertex() = default;
  ConfigGraphVertex(vector<int> * g): guards(g) {};
  vector<int> edges;
  // bitset of which vertices are safe
  vector<int>* guards = nullptr;
  bool removed = false;
};

struct ConfigGraph {
  vector<ConfigGraphVertex> vertices;
  ConfigGraph (Graph* g): g(g) {}
  Graph* g;

  void outputAllUnremoved();
  bool reduceToSafe(bool setRemoved);
  bool reduceToSafe(vector<bool>, bool setRemoved);
  int size() const;
  bool isVertexSafe(int, vector<bool> &) const;
  ~ConfigGraph();
  void OutputToFile(int, vector<bool>, bool);
  void OutputToFile();

  int GetMinimalStrategy();
  void getAllSubsetsOfSize(int k, int idx, vector<bool> cur, vector<vector<bool>> &res);
};

#endif //EDN_MAIN_H
