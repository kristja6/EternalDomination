#include "graph.h"

void Graph::output() {
  for (int i = 0; i < vertices.size(); ++i) {
    if (vertices[i].guards) {
      cout << i << ": " << vertices[i].guards << endl;
    }
  }
}
