#include "graph.h"
#include "arguments.h"
#include "blockcuttree.h"
#include "cactus.h"

int main(int argc, const char* argv[]) {
  try {
    Arguments args;
    if (!args.readArguments(argc, argv)) {
      args.displayHelp();
      return 1;
    }

    Graph g;
    g.loadFromFile(args.inputFile);
    // find the smallest k for which there is a safe configuration
    if (args.bruteforce || args.dominationNumber) {
      int edn = -1;
      int dn = 0;
      ConfigGraph *configGraph = nullptr;
      for (int i = 1; i <= g.size(); ++i) {
        configGraph = g.createConfigurationGraph(i, args.multipleGuards);
        if (configGraph->size() && !dn) {
          dn = i;
          if (!args.bruteforce) break;
        }

        configGraph->reduceToSafe();
        bool found = false;
        for (int j = 0; j < configGraph->size(); ++j) {
          // at least one unremoved after reducing
          if (!configGraph->vertices[j].removed) {
            found = true;
          }
        }
        if (found) {
          edn = i;
          break;
        }
      }

      // output to file
      if (args.displaySets && configGraph != nullptr)
        configGraph->outputAllUnremoved();

      if (args.dominationNumber)
        cout << "domination number = " << dn << endl;

      if (args.bruteforce)
        cout << "m-eternal domination number = " << edn << endl;

      delete configGraph;
    }

    BlockCutTree bc(&g);
    if (bc.isCactus()) {
      Cactus cactus(bc);
      cout << "cactus result:                " << cactus.EGC() << endl;

    }
  } catch (const char* msg) {
    cout << "Error!" << endl;
    cout << msg << endl;
  }

  return 0;
}

