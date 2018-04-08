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
#include <assert.h>
#include <unordered_set>

#include "max-flow.h"

using namespace std;

struct Graph;

struct GraphVertex {
  GraphVertex() = default;
  GraphVertex(Graph * g): g(g) {};
  vector<int> edges;
  int guards;
  Graph* g;
  // bitset of which vertices are safe
  vector<bool> safe;
  bool removed = false;
};

// todo: make with templates
struct Graph {
  vector<GraphVertex> vertices;
  vector<pair<int,int>> edgesBuffer;

  void output();

  void outputAllUnremoved() {
    int cnt = 0;
    for (int i = 0; i < vertices.size(); ++i) {
      if (!vertices[i].removed) {
        cout << "-------" << endl;
        cout << "State " << i << ": " << endl;
        for (int j = 0; j < vertices[i].edges.size(); ++ j) {
          cout << vertices[i].edges[j] << " ";
        }
        cout << endl;
        vertices[i].g->output();
      }
    }
  }


  bool canStrategyBeaClique(int edn, int inputSize) {
    // test various clique sizes up to |V| - edn
    for (int i = 1; i <= inputSize - edn + 1; ++i) {
      cout << "phase: " << i << endl;
      if (canStrategyBeACliqueRec(i, inputSize, 0, vector<int>())) {
        cout << "size of the clique is " << i << endl;
        return true;
      }
    }
    cout << "STRATEGY CANNOT BE A CLIQUE!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    return false;
  }

  bool canStrategyBeACliqueRec(int k, int inputSize, int start, vector<int> selected) {
    if (k == 0) { // test if the selected vertices create a safe configuration
      // test if the selected are a clique
      for (int j = 0; j < selected.size(); ++j) {
        //cout << selected[j] << " ";
      }
      //cout << endl;
      if (!isAClique(selected)) return false;

      Graph* reduced = new Graph(*this);
      for (int i = 0; i < size(); ++i) {
        reduced->vertices[i].removed = true;
      }
      // set not selected as removed
      for (int i = 0; i < selected.size(); ++i) {
        reduced->vertices[ selected[i] ].removed = vertices[ selected[i] ].removed;
      }
      reduced->reduceToSafe();
      for (int i = 0; i < size(); ++i) {
        if (!reduced->vertices[i].removed) {
          delete reduced;
          return true;
        }
      }
      delete reduced;
      return false;
    }

    for (int i = start; i < size(); ++i) {
      vector<int> s = selected;
      s.push_back(i);
      if (canStrategyBeACliqueRec(k - 1, inputSize, i + 1, s))
        return true;
    }
    return false;
  }

  // returns true if G contains some clique of size k
  bool cliqueProblem(int k) {
    if (k == 0) return true;

    for (int i = 0; i < size(); ++i) {
      if (cliqueProblemRec(k - 1, i + 1, vector<int>(1, i)))
        return true;
    }
  }

  bool cliqueProblemRec(int k, int start, vector<int> selected) {
    if (k == 0) { // test if selected is a clique
      return isAClique(selected);
    }
    for (int i = start; i < size(); ++i) {
      vector<int> s = selected;
      s.push_back(i);
      cliqueProblemRec(k - 1, i + 1, s);
    }
    return false;
  }

  bool isAClique(const vector<int> & selected) {
    map<int,int> foundAsNeighborCnt;
    for (int i = 0; i < selected.size(); ++i) {
      for (int j = 0; j < vertices[selected[i]].edges.size(); ++j) {
        foundAsNeighborCnt[ vertices[selected[i]].edges[j] ] ++;
      }
    }
    // each of them must a neighbor of k - 1 vertices in a k-clique
    for (int i = 0; i < selected.size(); ++i) {
      assert(foundAsNeighborCnt[ selected[i] ] <= selected.size() - 1);
      if (foundAsNeighborCnt[ selected[i] ] != selected.size() - 1) {
        //cout << "not: " << selected[i] << ", " << foundAsNeighborCnt[ selected[i] ] << endl;
        return false;
      }
    }
    return true;
  }

  // k is the number of guards
  void outputAllForDrawing(int k, const Graph & inputG) {
    ofstream output("drawing.in");
    output << inputG.edgesBuffer.size() << endl;
    for (int i = 0; i < inputG.edgesBuffer.size(); ++i) {
      output << inputG.edgesBuffer[i].first << " " << inputG.edgesBuffer[i].second << endl;
    }
    int totalSafeStates = 0;
    for (int i = 0; i < vertices.size(); ++i) {
      if (!vertices[i].removed) totalSafeStates ++;
    }
    output << totalSafeStates << endl;
    for (int i = 0; i < vertices.size(); ++i) {
      if (!vertices[i].removed) {
        output << k << endl;

        const Graph* curG = vertices[i].g;
        for (int j = 0; j < curG->size(); ++j) {
          for (int k = 0; k < curG->vertices[j].guards; ++k) {
            output << j << " ";
          }
        }
        output << endl;

      }
    }
  }

  bool isDominatingSet() {
    vector<bool> dominated(vertices.size(), false);
    for (int i = 0; i < vertices.size(); ++i) {
      if (vertices[i].guards) {
        dominated[i] = true;
        for (int j = 0; j < vertices[i].edges.size(); ++j) {
          dominated[ vertices[i].edges[j] ] = true;
        }
      }
    }
    for (int i = 0; i < vertices.size(); ++i) {
      if (!dominated[i]) return false;
    }
    return true;
  }

  // load undirected graph from stdin
  void loadFromStdin() {
    int n = -1;
    //int e;
    //cin >> e;
    // set to prevent duplicate edges
    //int e = 0;
    auto edgesBuffer = set<pair<int,int>>();
    //cerr << e << " edgesBuffer" << endl;
    //for (int i = 0; i < e; ++i) {
    int tu, tv;
    while (cin >> tu >> tv) {
      //cin >> edgesBuffer[i].first >> edgesBuffer[i].second;
      //e ++;
      //cin >> tu >> tv;
      if (tu == tv) continue;
      edgesBuffer.insert({tu, tv});
      n = max(tv + 1, n);
      n = max(tu + 1, n);
    }
    if (n == -1) throw "no input";
    vertices = vector<GraphVertex>(n);
    for (auto i = edgesBuffer.begin(); i != edgesBuffer.end(); ++ i) {
      vertices[i->first].edges.push_back(i->second);
      vertices[i->second].edges.push_back(i->first);
    }
    //cerr << "finished loading" << endl;
  }
  // creates a list of graphs with all possible configurations of k guards
  // and keep only those, which are a dominating set
  void iterateCombinations(int index, int free, vector<Graph*> & result, Graph* curVertex, bool allowMultiple = true) {
    if (index == curVertex->size()) {
      if (free == 0 && curVertex->isDominatingSet()) {
        /*cout << curVertex->vertices[0].guards << " ";
        cout << curVertex->vertices[1].guards << " ";
        cout << curVertex->vertices[2].guards << endl;*/
        result.push_back(curVertex);
      } else {
        delete curVertex;
      }
      return;
    }

    // add i guards to the current vertices
    const int maxGuards = allowMultiple ? free : 1;

    for (int i = 0; i <= maxGuards; ++ i) {
      Graph* newVertex = new Graph(*curVertex);
      newVertex->vertices[index].guards = i;
      iterateCombinations(index + 1, free - i, result, newVertex);
    }
  }

  // is one configuration passable to other by the rules of eternal domination
  bool oneMoveDistance(Graph & g, Graph & h, int k) {
    Network net(g.size() + h.size() + 3);
    //net.init();
    const int source = 0, sink = 1;
    int offset = 2;
    // create edges for all vertices in G
    for (int i = 0; i < g.vertices.size(); ++ i) {
      if (g.vertices[i].guards) {
        net.addEdge(source, i + offset, g.vertices[i].guards);
      }
    }
    offset += g.vertices.size();
    // create edges for all vertices in H
    for (int i = 0; i < h.vertices.size(); ++ i) {
      if (h.vertices[i].guards) {
        net.addEdge(i + offset, sink, h.vertices[i].guards);
      }
    }
    // create edges between G and H
    for (int i = 0; i < g.vertices.size(); ++ i) {
      if (!g.vertices[i].guards) continue;

      // create edge to yourself
      net.addEdge(i + 2, i + g.vertices.size() + 2, 999);

      for (int j = 0; j < g.vertices[i].edges.size(); ++j) {
        if (!h.vertices[ g.vertices[i].edges[j] ].guards) continue;

        // TODO: 999 -> Inf
        // cerr << "a" << endl;
        net.addEdge(i + 2, g.vertices[i].edges[j] + g.vertices.size() + 2, 999);
      }
    }
    return net.maxFlow(source, sink) == k;
  }

  // creates a graph of all possible configurations of k guards on this graph.
  // Each vertice in the graph is one configuration
  Graph createConfigurationGraph(int k) {
    vector<Graph*> allConfigs;
    //cout << "iterating combinations..." << endl;
    iterateCombinations(0, k, allConfigs, new Graph(*this));

    Graph result;
    for (auto i = allConfigs.begin(); i != allConfigs.end(); ++ i) {
      result.vertices.push_back(GraphVertex(*i));
    }

    //cout << "creating edges..." << endl;
    // create edges between configurations
    int cnt = 0;
    int total = (result.size() * (result.size() - 1)) / 2;
    for (int i = 0; i < result.size(); ++i) {
      for (int j = i + 1; j < result.size(); ++ j) {
        cnt ++;
        if (cnt % 1000 == 0) cout << "progress (edges): " << (cnt*100) / total << "     \r";
        // you can always stay in the current state
        if (i == j || oneMoveDistance(*allConfigs[i], *allConfigs[j], k)) {
          /*cout << "adding edge" << endl;
          allConfigs[i]->output();
          cout << "----" << endl;
          allConfigs[j]->output();*/
          // the transition is always both ways
          result.vertices[i].edges.push_back(j);
          result.vertices[j].edges.push_back(i);
        }
      }
    }
    return result;
  }

  void reduceToSafe() {
    bool removedAny;
    do {
      // clear information on which vertices are safe
      for (int i = 0; i < vertices.size(); ++i) {
        vertices[i].safe = vector<bool>(vertices[i].g->size(), false);
        for (int j = 0; j < vertices[i].g->vertices.size(); ++j) {
          if (vertices[i].g->vertices[j].guards) vertices[i].safe[j] = true;
        }
      }

      removedAny = false;
      for (int i = 0; i < vertices.size(); ++i) {
        if (vertices[i].removed) continue;

        for (int j = 0; j < vertices[i].edges.size(); ++j) {
          // check which vertices are saved by this move
          const int neighbor = vertices[i].edges[j];
          if (vertices[neighbor].removed) continue;

          const Graph *config = vertices[neighbor].g;
          for (int l = 0; l < config->size(); ++l) {
            if (config->vertices[l].guards) vertices[i].safe[l] = true;
          }
        }
        bool isUnsafe = false;
        for (int j = 0; j < vertices[i].safe.size(); ++j) {
          if (!vertices[i].safe[j]) isUnsafe = true;
        }
        if (isUnsafe) {
          vertices[i].removed = true;
          removedAny = true;
        }
      }
    } while (removedAny);
  }

  int size() const { return static_cast<int>(vertices.size()); }
};

#endif //EDN_MAIN_H
