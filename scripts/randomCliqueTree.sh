#!/usr/bin/env bash
cnt=0
while [ "$cnt" -lt "1000" ]; do
  python3 genRandomCT.py > out.txt
  r=$(../medn out.txt)
  if ! [ -z "$r" ]; then
    echo ${cnt}
    echo "$r"
    cp out.txt ../inputs/random_clique_trees/${cnt}.txt
    cnt=$((cnt+1))
  fi
done
