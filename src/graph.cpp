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
      for (int j = 0; j < g->Size(); ++j) {
        if ((*vertices[i].guards)[j])
          cout << j << ": " << (*vertices[i].guards)[j] << endl;
      }
    }
  }
}

void Graph::IterateCombinations(int index, int free, vector<vector<int> *> &result, vector<int> *curConfig,
                                bool allowMultiple) {
  if (index == curConfig->size()) {
    if (outputProgress && free == 0) {
      combinationsCounter ++;
      if (combinationsCounter % 100000 == 0) {
        cout << "k = " << currentMaxGuards << ". Creating vertices ("
             << ((combinationsCounter * 100.0) / double(combinationsTotal)) << "%)     \r" << flush;
      }
    }

    if (free == 0 && IsDominatingSet(*curConfig)) {
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
    IterateCombinations(index + 1, free - i, result, newConfig, allowMultiple);
  }
  delete curConfig;
}

bool Graph::OneMoveDistance(const vector<int> &g, const vector<int> &h, int k) {
  Network net(2* Size() + 3);
  const int source = 0, sink = 1;
  int offset = 2;
  // create edges for all vertices in G
  for (int i = 0; i < Size(); ++ i) {
    if (g[i]) {
      net.AddEdge(source, i + offset, g[i]);
    }
  }
  offset += Size();
  // create edges for all vertices in H
  for (int i = 0; i < Size(); ++ i) {
    if (h[i]) {
      net.AddEdge(i + offset, sink, h[i]);
    }
  }
  // create edges between G and H
  for (int i = 0; i < Size(); ++ i) {
    if (!g[i]) continue;

    // create edge to yourself
    net.AddEdge(i + 2, i + Size() + 2, k + 1);

    for (int j = 0; j < vertices[i].edges.size(); ++j) {
      if (!h[ vertices[i].edges[j] ]) continue;

      net.AddEdge(i + 2, vertices[i].edges[j] + Size() + 2, k + 1);
    }
  }
  return net.MaxFlow(source, sink) == k;
}

ConfigGraph *Graph::CreateConfigurationGraph(int k, bool multipleGuards, bool heuristics) {
  vector<vector<int>*> allConfigs;
  // get all possible guard configurations
  vector<int>* initialConfig = new vector<int>(Size(), 0);

  combinationsCounter = 0;
  if (multipleGuards) {
    combinationsTotal = NchooseK(vertices.size() + k - 1, k);
  } else {
    combinationsTotal = NchooseK(vertices.size(), k);
  }
  currentMaxGuards = k;
  IterateCombinations(0, k, allConfigs, initialConfig, multipleGuards);

  ConfigGraph *result = new ConfigGraph(this);
  for (auto &config : allConfigs) {
    // copy the pointers into the current graph
    result->vertices.push_back(ConfigGraphVertex(config));
  }

  unsigned long long cnt = 0;
  unsigned long long limit = 256;
  unsigned long long total = (result->size() * (result->size() - 1))/2;
  unsigned long long timer = 0;

  if (heuristics) {
    srand(time(0));
    random_shuffle(result->vertices.begin(), result->vertices.end());
  }


  // create edges between configurations
  for (int i = 0; i < result->size(); ++i) {
    for (int j = i + 1; j < result->size(); ++ j) {
      if (OneMoveDistance(*(result->vertices[i].guards), *(result->vertices[j].guards), k)) {
        // the transition is always both ways
        result->vertices[i].edges.push_back(j);
        result->vertices[j].edges.push_back(i);
      }
      if (outputProgress && timer % 1000 == 0) {
        cout << "testing k = " << k << " (" << (timer*100.0)/total << "%)             \r";
      }
      timer ++;
      cnt ++;
    }

    // return sooner if we already have a valid strategy
    if (heuristics && cnt >= limit) {
      cnt = 0;
      limit = (unsigned long long)(limit*1.25);
      for (int l = 0; l < Size(); ++l) {
        result->vertices[i].removed = false;
      }
      if (result->reduceToSafe(false)) {
        return result;
      }
    }

    // check whether the vertex on i is safe. If it isn't it won't be safe anytime in future
  }
  return result;
}

bool ConfigGraph::isVertexSafe(int i, vector<bool> & removedVertices) const {

  const ConfigGraphVertex & vertex = vertices[i];
  //if (vertex.removed) return false;
  if (removedVertices[i]) return false;

  vector<bool> safe(g->Size(), false);

  for (int j = 0; j < g->vertices.size(); ++j) {
    if ((*vertex.guards)[j]) safe[j] = true;
  }

  for (int j = 0; j < vertex.edges.size(); ++j) {
    // check which vertices are saved by this move
    const int neighbor = vertex.edges[j];
    if (removedVertices[neighbor]) continue;

    const vector<int> * config = vertices[neighbor].guards;
    for (int l = 0; l < g->Size(); ++l) {
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
    //vertex.removed = true;
    return false;
  }

  return true;
}

bool ConfigGraph::reduceToSafe(bool setRemoved = true) {
  vector<bool> enableAll(size(), true);
  return reduceToSafe(enableAll, setRemoved);
}

bool ConfigGraph::reduceToSafe(vector<bool> enabled, bool setRemoved) {
  vector<bool> removedVertices(size(), false);
  queue<int> unsafe;
  for (int i = 0; i < enabled.size(); ++i) {
    if (!enabled[i]) removedVertices[i] = true;
  }

  // clear information on which vertices are safe (meaning they can defend against any attack)

  for (int i = 0; i < size(); ++ i) {
    const ConfigGraphVertex & vertex = vertices[i];

    if (removedVertices[i]) continue;

    vector<bool> safe(g->Size(), false);
    for (int j = 0; j < g->vertices.size(); ++j) {
      if ((*vertex.guards)[j]) safe[j] = true;
    }

    for (int j = 0; j < vertex.edges.size(); ++j) {
      // check which vertices are saved by this move
      const int neighbor = vertex.edges[j];
      if (removedVertices[neighbor]) continue;

      //const Graph *config = vertices[neighbor].g;
      const vector<int> * config = vertices[neighbor].guards;
      for (int l = 0; l < g->Size(); ++l) {
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
      //vertex.removed = true;
      removedVertices[i] = true;
      unsafe.push(i);
    }
  }


  vector<bool> inQueue(vertices.size(), false);
  while (!unsafe.empty()) {
    int cur = unsafe.front();
    unsafe.pop();
    inQueue[cur] = false;

    if (!isVertexSafe(cur, removedVertices)) {
      removedVertices[cur] = true;
      //vertices[cur].removed = true;

      for (int i = 0; i < vertices[cur].edges.size(); ++i) {
        const int neigh = vertices[cur].edges[i];
        if (removedVertices[neigh] || inQueue[neigh]) continue;
        inQueue[neigh] = true;
        unsafe.push(vertices[cur].edges[i]);
      }
    }
  }

  // check if it is non-empty
  bool isSafe = false;
  // remove all vertices that are actually not safe
  for (int j = 0; j < size(); ++j) {
    if (setRemoved) vertices[j].removed = removedVertices[j];
    // at least one unremoved after reducing
    if (!removedVertices[j]) {
      isSafe = true;
    }
  }
  return isSafe;
}

int ConfigGraph::size() const { return static_cast<int>(vertices.size()); }

ConfigGraph::~ConfigGraph() {
  for (int i = 0; i < vertices.size(); ++i) {
    delete vertices[i].guards;
  }
}

int ConfigGraph::GetMinimalStrategy() {
  int cnt = 0;
  bool isSafeNow = false;
  for (int i = 1; i <= size(); ++i) {
    cout << "(" << size() << ") testing strategy size: " << i << "        \r" << flush;
    vector<bool> enabled(size(), false);
    vector<vector<bool>> sets;
    getAllSubsetsOfSize(i, 0, enabled, sets);

    for (int j = 0; j < sets.size(); ++j) {
      if (reduceToSafe(sets[j], false)) {
        isSafeNow = true;
        cnt ++;
        OutputToFile(cnt, sets[j], false);
        OutputToFile(cnt, sets[j], true);
      }
    }
    if (isSafeNow) return i;
  }
  return -1;
}

void ConfigGraph::getAllSubsetsOfSize(int k, int idx, vector<bool> cur, vector<vector<bool>> &res) {
  if (k == 0) {
    res.push_back(cur);
    return;
  }
  if (idx == size()) return;

  if (!vertices[idx].removed) {
    vector<bool> cp1 = cur;
    cp1[idx] = true;
    getAllSubsetsOfSize(k - 1, idx + 1, cp1, res);
  }
  getAllSubsetsOfSize(k, idx + 1, cur, res);
}

void ConfigGraph::OutputToFile(int n, vector<bool> enabled, bool compressIndexes = false) {
  string name = "configGraphs/configGraph" + to_string(n);
  if (compressIndexes) name = name + "_cmp_";
  name = name + ".in";
  ofstream output(name);
  int edges = 0;
  int v = 0;
  for (int i = 0; i < size(); ++i) {
    if (vertices[i].removed || !enabled[i]) continue;
    for (int j = 0; j < vertices[i].edges.size(); ++j) {
      if (i > vertices[i].edges[j]) continue;
      if (!vertices[ vertices[i].edges[j] ].removed && enabled[ vertices[i].edges[j] ]) {
        edges++;
        v = max(v, i);
        v = max(v, vertices[i].edges[j]);
      }
    }
  }
  v ++;
  vector<pair<int,int>> out;

  for (int i = 0; i < size(); ++i) {
    if (vertices[i].removed || !enabled[i]) continue;
    for (int j = 0; j < vertices[i].edges.size(); ++j) {
      if (i > vertices[i].edges[j]) continue;
      if (!vertices[ vertices[i].edges[j] ].removed && enabled[ vertices[i].edges[j] ])
        out.push_back({i, vertices[i].edges[j]});
    }
  }
  if (compressIndexes) {
    map<int,int> mapping;
    v = 0;
    int cur = 0;
    for (int i = 0; i < out.size(); ++i) {
      if (mapping.find(out[i].first) == mapping.end()) {
        mapping[out[i].first] = (cur ++);
      }
      if (mapping.find(out[i].second) == mapping.end()) {
        mapping[out[i].second] = (cur ++);
      }

      out[i].first = mapping[out[i].first];
      out[i].second = mapping[out[i].second];
      v = max(out[i].first + 1, max(out[i].second + 1, v));
    }
  }

  output << v << " " << edges << endl;

  for (int i = 0; i < out.size(); ++i) {
    output << out[i].first << " " << out[i].second << endl;
  }
}

void ConfigGraph::OutputToFile() {
  vector<bool> enableAll(size(), true);
  OutputToFile(0, enableAll, false);
}

bool Graph::IsDominatingSet(const vector<int> &guards) {
  vector<bool> dominated(Size(), false);
  for (int i = 0; i < Size(); ++i) {
    if (guards[i]) {
      dominated[i] = true;
      for (int j = 0; j < vertices[i].edges.size(); ++j) {
        dominated[vertices[i].edges[j]] = true;
      }
    }
  }
  for (int i = 0; i < Size(); ++i) {
    if (!dominated[i]) return false;
  }
  return true;
}

int Graph::Size() const { return (int)(vertices.size()); }

void Graph::LoadFromFile(const string &filename) {
  ifstream fileStream(filename);
  int n = -1;
  fileStream >> n;
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

long long Graph::NchooseK(long long n, long long k) {
  if (k == 0) return 1;
  if (n < k) return 0;

  if (k > n/2) return NchooseK(n, n-k);

  long long out = 1;
  for(int i = 1; i <= k; ++i) {
    out *= n-i+1;
    out /= i;
  }

  return out;
}
