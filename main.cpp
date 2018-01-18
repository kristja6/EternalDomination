#include <iostream>
#include <vector>
#include <queue>


using ll=long long;

using namespace std;

#define dbg cout;
struct NetworkEdge {
  //int vertex;
  ll from, to;
  ll cap;
  //int flow;
};

struct NetworkVertex {
  vector<NetworkEdge> edges;
  bool visited;
};

#define MAX 1000

// EdmondKarp from ACM codebook
struct Network {
  vector<NetworkEdge> edges;
  vector<ll> ng[MAX]; // indexes of edges
  ll back[MAX]; // indexes of edges for reconstructing augment path
  bool fromS[MAX]; // for minCut: can get from s when augment not found?

  void init(){
    edges.clear();
    for(int i = 0; i < MAX; ++ i){
      ng[i].clear();
      back[i] = 0;
      fromS[i] = false;
    }
  }

  void addEdge(ll from, ll to, ll capacity) { // 2 edges, back is residual, accessible by ^1 (even/odd)
    //cout << "add: " << from << "->" << to << "; " << capacity << endl;
    ng[from].push_back(edges.size());
    edges.push_back(NetworkEdge{from, to, capacity});

    ng[to].push_back(edges.size());
    edges.push_back(NetworkEdge{to, from, 0});
  }

  bool bfs(ll s, ll t) { // source,sink
    for(int i = 0; i < MAX; ++ i) back[i] = -1;
    back[s] = -2;
    for(int i = 0; i < MAX; ++ i) fromS[i] = 0;

    //CL(back,-1); back[s] = -2;
    //CL(fromS,0);
    queue<ll> q; q.push(s);
    while (!q.empty() && back[t] == -1) { // exists augment path to sink
      ll u = q.front(); q.pop();
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

  ll maxFlow(ll s, ll t) {
    ll maxFlow = 0;
    while (bfs(s, t)) {
      //cout << "found way" << endl;
      ll flow = 1<<30, node = t; // from sink to source(=-2)
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

  // load undirected graph from stdin
  void loadFromStdin() {
    int n = 0;
    int e;
    cin >> e;
    //cerr << e << " edges" << endl;
    vector<pair<int,int>> edges(e);
    for (int i = 0; i < e; ++i) {
      cin >> edges[i].first >> edges[i].second;
      n = max(edges[i].first + 1, n);
      n = max(edges[i].second + 1, n);
    }
    vertices = vector<GraphVertex>(n);
    for (auto i = edges.begin(); i != edges.end(); ++ i) {
      vertices[i->first].edges.push_back(i->second);
      vertices[i->second].edges.push_back(i->first);
    }
    cerr << "finished loading" << endl;
  }

  // creates a list of graphs with all possible configurations of k guards
  void iterateCombinations(int index, int free, vector<Graph*> & result, Graph* curVertex) {
    if (index == curVertex->size()) {
      if (free == 0) {
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
    for (int i = 0; i <= free; ++ i) {
      Graph* newVertex = new Graph(*curVertex);
      newVertex->vertices[index].guards = i;
      iterateCombinations(index + 1, free - i, result, newVertex);
    }
  }

  // is one configuration passable to other by the rules of eternal domination
  bool oneMoveDistance(Graph & g, Graph & h, int k) {
    Network net;
    net.init();
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
      net.addEdge(i + 2, i + g.vertices.size() + 2, 999);
      for (int j = 0; j < g.vertices[i].edges.size(); ++j) {
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
    iterateCombinations(0, k, allConfigs, new Graph(*this));

    Graph result;
    for (auto i = allConfigs.begin(); i != allConfigs.end(); ++ i) {
      result.vertices.push_back(GraphVertex(*i));
    }

    // create edges between configurations
    for (int i = 0; i < result.size(); ++i) {
      for (int j = 0; j < result.size(); ++ j) {
        //cerr << "testing: " << i << ", " << j << endl;
        if (oneMoveDistance(*allConfigs[i], *allConfigs[j], k)) {
          /*cout << "adding edge" << endl;
          allConfigs[i]->output();
          cout << "----" << endl;
          allConfigs[j]->output();*/
          result.vertices[i].edges.push_back(j);
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

int main() {
  Graph g;
  g.loadFromStdin();
  // find the smallest k for which there is a safe configuration
  bool found = false;
  int result;
  Graph configGraph;
  for (int i = 1; i < g.size() && !found; ++ i) {
    cout << "Testing k = " << i << "..." << endl;
    configGraph = g.createConfigurationGraph(i);
    configGraph.reduceToSafe();
    cout << "Possible states: " << configGraph.size() << endl;
    int safeStates = 0;
    bool found = false;
    for (int j = 0; j < configGraph.size(); ++j) {
      // at least one unremoved after reducing
      if (!configGraph.vertices[j].removed) {
        safeStates ++;
        found = true;
      }
    }
    cout << "Safe states: " << safeStates << endl;
    if (found) {
      result = i;
      break;
    }
  }

  cout << "ETERNAL DOMINATION NUMBER = " << result << endl;
  configGraph.outputAllUnremoved();
  return 0;
}