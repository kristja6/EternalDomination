#include "graph.h"

void ConfigGraph::outputAllUnremoved() {
  for (int i = 0; i < vertices.size(); ++i) {
    if (!vertices[i].removed) {
      cout << "-------" << endl;
      cout << "State " << i << ": " << endl;
      for (int edge : vertices[i].edges) {
        if (!vertices[edge].removed)
        cout << edge << " ";
      }
      cout << endl;
      for (int j = 0; j < g->size(); ++j) {
        if ((*vertices[i].guards)[j])
          cout << j << ": " << (*vertices[i].guards)[j] << endl;
      }
    }
  }
}

void Graph::iterateCombinations(int index, int free, vector<vector<int>*> &result, vector<int> *curConfig, bool allowMultiple) {
  if (index == curConfig->size()) {
    if (free == 0 && isDominatingSet(*curConfig)) {
      result.push_back(curConfig);
    } else {
      delete curConfig;
    }
    return;
  }

  // add i guards to the current vertices
  const int maxGuards = allowMultiple ? free : min(free, 1);

  for (int i = 0; i <= maxGuards; ++ i) {
    vector<int>* newConfig = new vector<int>(*curConfig);
    (*newConfig)[index] = i;
    iterateCombinations(index + 1, free - i, result, newConfig, allowMultiple);
  }
  delete curConfig;
}

bool Graph::oneMoveDistance(const vector<int> & g, const vector<int> & h, int k) {
  Network net(2*size() + 3);
  const int source = 0, sink = 1;
  int offset = 2;
  // create edges for all vertices in G
  for (int i = 0; i < size(); ++ i) {
    if (g[i]) {
      net.addEdge(source, i + offset, g[i]);
    }
  }
  offset += size();
  // create edges for all vertices in H
  for (int i = 0; i < size(); ++ i) {
    if (h[i]) {
      net.addEdge(i + offset, sink, h[i]);
    }
  }
  // create edges between G and H
  for (int i = 0; i < size(); ++ i) {
    if (!g[i]) continue;

    // create edge to yourself
    net.addEdge(i + 2, i + size() + 2, k + 1);

    for (int j = 0; j < vertices[i].edges.size(); ++j) {
      if (!h[ vertices[i].edges[j] ]) continue;

      net.addEdge(i + 2, vertices[i].edges[j] + size() + 2, k + 1);
    }
  }
  return net.maxFlow(source, sink) == k;
}

ConfigGraph *Graph::createConfigurationGraph(int k, bool multipleGuards) {
  vector<vector<int>*> allConfigs;
  // get all possible guard configurations
  vector<int>* initialConfig = new vector<int>(size(), 0);
  iterateCombinations(0, k, allConfigs, initialConfig, multipleGuards);

  ConfigGraph *result = new ConfigGraph(this);
  for (auto &config : allConfigs) {
    // copy the pointers into the current graph
    result->vertices.push_back(ConfigGraphVertex(config));
  }

  // create edges between configurations
  int cnt = 0;
  for (int i = 0; i < result->size(); ++i) {
    for (int j = i + 1; j < result->size(); ++ j) {
      cnt ++;
      if (oneMoveDistance(*(allConfigs[i]), *(allConfigs[j]), k)) {
        // the transition is always both ways
        result->vertices[i].edges.push_back(j);
        result->vertices[j].edges.push_back(i);
      }
    }
  }
  return result;
}

void ConfigGraph::reduceToSafe() {
  bool removedAny;
  do {
    removedAny = false;
    // clear information on which vertices are safe (meaning they can defend against any attack)

    for (auto &vertex : vertices) {
      vector<bool> safe(g->size(), false);
      for (int j = 0; j < g->vertices.size(); ++j) {
        if ((*vertex.guards)[j]) safe[j] = true;
      }

      if (vertex.removed) continue;

      for (int j = 0; j < vertex.edges.size(); ++j) {
        // check which vertices are saved by this move
        const int neighbor = vertex.edges[j];
        if (vertices[neighbor].removed) continue;

        //const Graph *config = vertices[neighbor].g;
        const vector<int> * config = vertices[neighbor].guards;
        for (int l = 0; l < g->size(); ++l) {
          if ((*config)[l]) safe[l] = true;
        }
      }

      bool isUnsafe = false;
      for (int j = 0; j < safe.size(); ++j) {
        if (!safe[j]) {
          isUnsafe = true;
          break;
        }
      }
      if (isUnsafe) {
        vertex.removed = true;
        removedAny = true;
      }
    }
  } while (removedAny);
}

int ConfigGraph::size() const { return static_cast<int>(vertices.size()); }

bool Graph::isDominatingSet(const vector<int> & guards) {
  vector<bool> dominated(size(), false);
  for (int i = 0; i < size(); ++i) {
    if (guards[i]) {
      dominated[i] = true;
      for (int j = 0; j < vertices[i].edges.size(); ++j) {
        dominated[vertices[i].edges[j]] = true;
      }
    }
  }
  for (int i = 0; i < size(); ++i) {
    if (!dominated[i]) return false;
  }
  return true;
}

int Graph::size() const { return (int)(vertices.size()); }

void Graph::loadFromFile(const string &filename) {
  ifstream fileStream(filename);
  int n = -1;
  auto edgesBuffer = set<pair<int,int>>();
  int tu, tv;
  while (fileStream >> tu >> tv) {
    if (tu == tv) continue;
    edgesBuffer.insert({tu, tv});
    n = max(tv + 1, n);
    n = max(tu + 1, n);
  }
  if (n == -1) throw "no input";
  vertices = vector<GraphVertex>(n);
  for (const auto &i : edgesBuffer) {
    vertices[i.first].edges.push_back(i.second);
    vertices[i.second].edges.push_back(i.first);
  }
}
