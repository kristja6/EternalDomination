//
// Created by maty on 5/9/18.
//

#ifndef EDN_CLIQUETREE_H
#define EDN_CLIQUETREE_H

#include "blockcuttree.h"

class CliqueTree {
public:
  CliqueTree(const BlockCutTree & bc);
  int EDN();

private:
  void Clique(int & edn, int v, int u);
  void Articulation(int & edn, int v, int u);

  const BlockCutTree & bc;
  vector<int> size;
  vector<int> deg;
  vector<int> cliques;
  vector<bool> deleted;
};


#endif //EDN_CLIQUETREE_H
