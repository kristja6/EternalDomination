#!/usr/bin/env bash
for i in ../inputs/random_clique_trees/*; do
  ../medn -eq ${i} > /tmp/o.txt && sort -rn /tmp/o.txt > /tmp/o1.txt && sort -n /tmp/o.txt > /tmp/o2.txt
  diff /tmp/o1.txt /tmp/o2.txt > /dev/null
  if [ $? -eq 1 ]; then
    echo "Error on $i";
  fi
done
