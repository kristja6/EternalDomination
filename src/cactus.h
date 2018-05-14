//
// Created by maty on 4/22/18.
//

#ifndef EDN_CACTUS_H
#define EDN_CACTUS_H


#include "blockcuttree.h"

class Cactus {
public:
  Cactus(const BlockCutTree & bc);
  int EGC();

private:
  const BlockCutTree & bc;
  vector<int> size;
  vector<int> deg;
  vector<int> cliques;
  vector<bool> deleted;

  void Block(int u, int v, int & g, stack<int> & st, bool & del);
  void LeafCycle(int v, int & g, stack<int> & st, bool & del);
  void Articulation(int u, int v, int & g, stack<int> & st, bool & del);
};

#endif //EDN_CACTUS_H
