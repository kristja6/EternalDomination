#include "arguments.h"

Arguments::Arguments(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') { // a switch argument
      size_t len = strlen(argv[i]);
      for (int j = 1; j < len; ++j) {
        switch (argv[i][j]) { // display help
          case 'm': multipleGuards = true; break;
          case 'v': displaySets = true; break;
          default: throw "unexpected argument";
        }
      }
    }
  }
}

void Arguments::displayHelp() {
  cout << helpText << endl;
}
