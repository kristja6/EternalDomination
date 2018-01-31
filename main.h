//
// Created by maty on 1/31/18.
//

#ifndef EDN_MAIN_H
#define EDN_MAIN_H

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>

using ll=long long;

using namespace std;

struct NetworkEdge {
  //int vertex;
  int from, to;
  int cap;
  //int flow;
};

struct NetworkVertex {
  vector<NetworkEdge> edges;
  bool visited;
};

//#define vertices 1000

// EdmondKarp from ACM codebook
struct Network {
  vector<NetworkEdge> edges;
  vector<vector<int>> ng; // indexes of edges
  vector<int> back; // indexes of edges for reconstructing augment path
  vector<bool> fromS; // for minCut: can get from s when augment not found?
  int vertices;
  Network(int n): vertices(n), ng(vector<vector<int>>(n)), back(vector<int>(n)), fromS(vector<bool>(n)) {

  }

  /*void init(){
    edges.clear();
    for(int i = 0; i < vertices; ++ i){
      ng[i].clear();
      back[i] = 0;
      fromS[i] = false;
    }
  }*/

  void addEdge(int from, int to, int capacity) { // 2 edges, back is residual, accessible by ^1 (even/odd)
    //cout << "add: " << from << "->" << to << "; " << capacity << endl;
    ng[from].push_back(edges.size());
    edges.push_back(NetworkEdge{from, to, capacity});

    ng[to].push_back(edges.size());
    edges.push_back(NetworkEdge{to, from, 0});
  }

  bool bfs(int s, int t) { // source,sink
    for(int i = 0; i < vertices; ++ i) back[i] = -1;
    back[s] = -2;
    for(int i = 0; i < vertices; ++ i) fromS[i] = 0;

    //CL(back,-1); back[s] = -2;
    //CL(fromS,0);
    queue<int> q; q.push(s);
    while (!q.empty() && back[t] == -1) { // exists augment path to sink
      int u = q.front(); q.pop();
      fromS[u]=1;
      //F(ng[u].size()) {
      for(int i = 0; i < ng[u].size(); ++ i) {
        NetworkEdge & edge = edges[ng[u][i]];
        if (edge.cap && back[edge.to] == -1) { // has capacity
          back[edge.to] = ng[u][i];
          q.push(edge.to);
        }
      }
    }
    return back[t] != -1;
  }

  int maxFlow(int s, int t) {
    int maxFlow = 0;
    while (bfs(s, t)) {
      //cout << "found way" << endl;
      int flow = 1<<30, node = t; // from sink to source(=-2)
      // find size of the flow = min capacity on the way:
      while (back[node] != -2) {
        NetworkEdge & edge = edges[back[node]];
        flow = min(flow, edge.cap);
        node = edge.from;
      }
      // push the flow:
      node=t;
      while (back[node] != -2) {
        NetworkEdge & edge = edges[back[node]],
            & edge2 = edges[back[node]^1];
        edge.cap -= flow;
        edge2.cap += flow;
        node = edge.from; // going back
      }

      maxFlow += flow;
    }
    /*cout<<"Max flow: "<< maxFlow <<endl;
    cout<<"Min cut: "; //F(edges.size()) {
    for (int i = 0; i < edges.size(); ++ i) {
      if(i&1)continue;
      auto& e=edges[i];
      if(fromS[e.from] != fromS[e.to]) cout<<e.from<<","<<e.to<<" ";
    }cout<<endl;*/
    return maxFlow;
  }

};

struct Vertex {
  vector<int> edges;
  int guards;
  bool visited;
};

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

  void output() {
    for (int i = 0; i < vertices.size(); ++i) {
      if (vertices[i].guards) {
        cout << i << ": " << vertices[i].guards << endl;
      }
    }
  }

  void outputAllUnremoved() {
    int cnt = 0;
    for (int i = 0; i < vertices.size(); ++i) {
      if (!vertices[i].removed) {
        cnt ++;
        cout << "-------" << endl;
        cout << "State " << cnt << ": " << endl;
        vertices[i].g->output();
      }
    }
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
    int n = 0;
    int e;
    cin >> e;
    edgesBuffer = vector<pair<int,int>>(e);
    //cerr << e << " edgesBuffer" << endl;
    for (int i = 0; i < e; ++i) {
      cin >> edgesBuffer[i].first >> edgesBuffer[i].second;
      n = max(edgesBuffer[i].first + 1, n);
      n = max(edgesBuffer[i].second + 1, n);
    }
    vertices = vector<GraphVertex>(n);
    for (auto i = edgesBuffer.begin(); i != edgesBuffer.end(); ++ i) {
      vertices[i->first].edges.push_back(i->second);
      vertices[i->second].edges.push_back(i->first);
    }
    cerr << "finished loading" << endl;
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
      for (int j = i; j < result.size(); ++ j) {
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
