#include "graph.h"
#include "arguments.h"
#include "blockcuttree.h"
#include "cactus.h"
#include "cliqueTree.h"

int main(int argc, const char* argv[]) {
  try {
    Arguments args;
    if (!args.readArguments(argc, argv)) {
      args.displayHelp();
      return 1;
    }

    Graph g(args.progressOutput);
    g.loadFromFile(args.inputFile);

    BlockCutTree bc(&g);

    CliqueTree cliqueTree(bc);
    cout << cliqueTree.EDN() << endl;

    if (bc.isCactus()) {
      Cactus cactus(bc);
      cout << "cactus alg. result: " << cactus.EGC() << "       " << endl;
    } else if (bc.isCliqueTree()) {
      CliqueTree cliqueTree(bc);
      cout << "clique tree alg. result: " << cliqueTree.EDN() << "       " << endl;
    } else {
      cout << "polynomial algorithm inapplicable" << endl;
    }

    // find the smallest k for which there is a safe configuration
    if (args.bruteforce || args.dominationNumber) {
      int edn = -1;
      int dn = 0;
      ConfigGraph *configGraph = nullptr;
      for (int i = 1; i <= g.size(); ++i) {
        delete configGraph;
        configGraph = g.createConfigurationGraph(i, args.multipleGuards, args.heuristics);
        if (configGraph->size() && !dn) {
          dn = i;
          if (!args.bruteforce) break;
        }

        bool found = configGraph->reduceToSafe();
        if (found) {
          edn = i;
          break;
        }
      }

      if (args.displaySets && configGraph != nullptr)
        configGraph->outputAllUnremoved();

      if (args.dominationNumber)
        cout << "domination number = " << dn << "              " << endl;

      if (args.bruteforce)
        cout << edn << endl;
        //cout << "m-eternal domination number = " << edn << "       " << endl;

      delete configGraph;
    }

  } catch (const char* msg) {
    cout << "Error!" << endl;
    cout << msg << endl;
  }

  return 0;
}

