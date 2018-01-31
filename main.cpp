#include "main.h"

#define dbg cout;

int main(int argc, const char* argv[]) {
  Graph g;
  g.loadFromStdin();
  // find the smallest k for which there is a safe configuration
  bool found = false;
  int edn;
  int dn = 0;
  Graph configGraph;
  for (int i = 1; i < g.size() && !found; ++ i) {
    cout << "Testing k = " << i << "..." << endl;
    //cout << "creating config graph...\r";
    configGraph = g.createConfigurationGraph(i);
    //cout << "Possible states: " << configGraph.size() << "\r";
    if (configGraph.size() && !dn) dn = i;
    //cout << "reducing...\r";
    configGraph.reduceToSafe();
    int safeStates = 0;
    bool found = false;
    for (int j = 0; j < configGraph.size(); ++j) {
      // at least one unremoved after reducing
      if (!configGraph.vertices[j].removed) {
        safeStates ++;
        found = true;
      }
    }
    //cout << "Safe states: " << safeStates << "\r" << endl;
    if (found) {
      edn = i;
      break;
    }
  }

  // output to file
  //configGraph.outputAllUnremoved();
  configGraph.outputAllForDrawing(edn, g);
  cout << "DOMINATION NUMBER = " << dn << endl;
  cout << "ETERNAL DOMINATION NUMBER = " << edn << endl;
  return 0;
}

