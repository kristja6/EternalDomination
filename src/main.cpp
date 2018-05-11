#include "graph.h"
#include "arguments.h"
#include "blockcuttree.h"
#include "cactus.h"
#include "cliqueTree.h"

int main(int argc, const char* argv[]) {
  try {
    Arguments args;
    if (!args.ReadArguments(argc, argv)) {
      args.DisplayHelp();
      return 1;
    }

    Graph g(args.progressOutput);
    g.LoadFromFile(args.inputFile);

    BlockCutTree bc(&g);

    int resultCactus = -1;
    int resultCliqueTree = -1;

    if (bc.IsCactus()) {
      Cactus cactus(bc);
      cout << "cactus result: " << (resultCactus = cactus.EGC()) << "       " << endl;
    }
    if (bc.IsCliqueTree()) {
      CliqueTree cliqueTree(bc);
      cout << "clique tree result: " << (resultCliqueTree = cliqueTree.EDN()) << "       " << endl;
    }

    // find the smallest k for which there is a safe configuration
    if (args.bruteforce || args.dominationNumber) {
      int edn = -1;
      int dn = 0;
      ConfigGraph *configGraph = nullptr;
      for (int i = 1; i <= g.Size(); ++i) {
        delete configGraph;
        configGraph = g.CreateConfigurationGraph(i, args.multipleGuards, args.heuristics);
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
        cout << "m-eternal domination number = " << edn << "       " << endl;

      if (args.testingMode) {
        if (resultCactus != -1 && edn != resultCactus) {
          cout << "Error on cactus algorithm!" << endl;
        }
        if (resultCliqueTree != -1 && edn != resultCliqueTree) {
          cout << "Error on clique tree algorithm!" << endl;
        }
      }

      delete configGraph;
    }

  } catch (const char* msg) {
    cout << "Error!" << endl;
    cout << msg << endl;
  }

  return 0;
}

