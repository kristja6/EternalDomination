//
// Created by maty on 5/9/18.
//

#ifndef EDN_CLIQUETREE_H
#define EDN_CLIQUETREE_H

#include "blockcuttree.h"

struct CliqueTree {
  const BlockCutTree & bc;
  vector<int> size;
  vector<int> deg;
  vector<int> cliques;
  vector<bool> deleted;

  CliqueTree(const BlockCutTree & bc);

  int EDN();
};


#endif //EDN_CLIQUETREE_H
