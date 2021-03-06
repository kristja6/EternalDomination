#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <bits/stdc++.h>

using namespace std;
struct Arguments {
  const string helpText = "Usage: medn [OPTION] ... [FILE]\n\
\n\
  Options:\n\
    -m\t\t\tallow multiple guards to stand on one vertex\n\
    -v\t\t\tdisplay all m-eternal dominating configurations/sets\n\
    -e\t\t\trun the bruteforce algorithm\n\
    -d\t\t\tdisplay the domination number\n\
    -h\t\t\tdisable heuristics (will find all possible configurations)\n\
    -q\t\t\thide progress output\n\
    -c\t\t\toutput config graph\n\
    -s\t\t\toutput the size of the minimal configuration graph\n\
\n\
  Input format:\n\
    The first line contains the number of vertices in the graph\n\
    The input file contains two numbers on every line, representing an edge.\n\
    The indexes of the vertices always start from zero:\n\
\n\
  Example input (path on 4 vertices):\n\
0 1\n\
1 2\n\
2 3\
";

  // return true on success
  bool ReadArguments(int argc, const char **argv);
  void DisplayHelp();

  string inputFile;
  bool multipleGuards = false;
  bool displaySets = false;
  bool bruteforce = false;
  bool dominationNumber = false;
  bool heuristics = true;
  bool progressOutput = true;
  bool testingMode = false;
  bool outputConfigGraph = false;
  bool minConfigSize = false;
};

#endif
