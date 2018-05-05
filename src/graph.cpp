#include "graph.h"

void Graph::output() {
  for (int i = 0; i < vertices.size(); ++i) {
    if (vertices[i].guards) {
      cout << i << ": " << vertices[i].guards << endl;
    }
  }
}

void ConfigGraph::outputAllUnremoved() {
  int cnt = 0;
  for (int i = 0; i < vertices.size(); ++i) {
    if (!vertices[i].removed) {
      cout << "-------" << endl;
      cout << "State " << i << ": " << endl;
      for (int edge : vertices[i].edges) {
        cout << edge << " ";
      }
      cout << endl;
      vertices[i].g->output();
    }
  }
}

void Graph::iterateCombinations(int index, int free, vector<Graph *> &result, Graph *curVertex, bool allowMultiple) {
  if (index == curVertex->size()) {
    if (free == 0 && curVertex->isDominatingSet()) {
      result.push_back(curVertex);
    } else {
      delete curVertex;
    }
    return;
  }

  // add i guards to the current vertices
  const int maxGuards = allowMultiple ? free : min(free, 1);

  for (int i = 0; i <= maxGuards; ++ i) {
    Graph* newVertex = new Graph(*curVertex);
    newVertex->vertices[index].guards = i;
    iterateCombinations(index + 1, free - i, result, newVertex, allowMultiple);
  }
}

bool Graph::oneMoveDistance(Graph &g, Graph &h, int k) {
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

ConfigGraph *Graph::createConfigurationGraph(int k, bool multipleGuards) {
  vector<Graph*> allConfigs;
  //cout << "iterating combinations..." << endl;
  iterateCombinations(0, k, allConfigs, new Graph(*this), multipleGuards);

  ConfigGraph *result = new ConfigGraph();
  for (auto &config : allConfigs) {
    result->vertices.push_back(ConfigGraphVertex(config));
  }

  //cout << "creating edges..." << endl;
  // create edges between configurations
  int cnt = 0;
  for (int i = 0; i < result->size(); ++i) {
    for (int j = i + 1; j < result->size(); ++ j) {
      cnt ++;
      if (oneMoveDistance(*allConfigs[i], *allConfigs[j], k)) {
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
    // clear information on which vertices are safe
    for (auto &vertex : vertices) {
      vertex.safe = vector<bool>(vertex.g->size(), false);
      for (int j = 0; j < vertex.g->vertices.size(); ++j) {
        if (vertex.g->vertices[j].guards) vertex.safe[j] = true;
      }
    }

    removedAny = false;
    for (auto &vertex : vertices) {
      if (vertex.removed) continue;

      for (int j = 0; j < vertex.edges.size(); ++j) {
        // check which vertices are saved by this move
        const int neighbor = vertex.edges[j];
        if (vertices[neighbor].removed) continue;

        const Graph *config = vertices[neighbor].g;
        for (int l = 0; l < config->size(); ++l) {
          if (config->vertices[l].guards) vertex.safe[l] = true;
        }
      }
      bool isUnsafe = false;
      for (int j = 0; j < vertex.safe.size(); ++j) {
        if (!vertex.safe[j]) isUnsafe = true;
      }
      if (isUnsafe) {
        vertex.removed = true;
        removedAny = true;
      }
    }
  } while (removedAny);
}

int ConfigGraph::size() const { return static_cast<int>(vertices.size()); }

bool Graph::isDominatingSet() {
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

int Graph::size() const { return (int)(vertices.size()); }
