#include "arguments.h"

void Arguments::DisplayHelp() {
  cout << helpText << endl;
}

bool Arguments::ReadArguments(int argc, const char **argv) {
  if (argc <= 1) return false;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') { // a switch argument
      size_t len = strlen(argv[i]);
      for (int j = 1; j < len; ++j) {
        switch (argv[i][j]) { // display help
          case 'm': multipleGuards = true; break;
          case 'v': displaySets = true; break;
          case 'e': bruteforce = true; break;
          case 'd': dominationNumber = true; break;
          case 'h': heuristics = false; break;
          case 'q': progressOutput = false; break;
          case 't': testingMode = true; break;
          case 'c': outputConfigGraph = true; break;
          case 's': minConfigSize = true; break;
          default: return false;
        }
      }
    } else { // is the filenames
      inputFile = string(argv[i]);
    }
  }
  return true;
}
