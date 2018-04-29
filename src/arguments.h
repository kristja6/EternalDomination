#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <bits/stdc++.h>

using namespace std;
struct Arguments {
  const string helpText = "Usage: edn [OPTION] ... [FILE]\n\
\n\
  Options:\n\
    -m\t\t\tallow multiple guards to stand on one vertex\n\
    -v\t\t\tdisplay all m-eternal dominating sets\n\
    -e\t\t\trun the bruteforce algorithm\
";

  // return true on success
  bool readArguments(int argc, const char* argv[]);

  void displayHelp();

  string inputFile;
  bool multipleGuards = false;
  bool displaySets = false;
  bool bruteforce = false;
};

#endif
