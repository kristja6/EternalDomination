#!/usr/bin/env bash
cnt=0
while [ "$cnt" -lt "1000" ]; do
  python3 randomGraph.py > out.txt
  r=$(../main out.txt)
  if ! [ -z "$r" ]; then
    echo ${cnt}
    echo "$r"
    cp out.txt ../inputs/random_cacti/${cnt}.txt
    cnt=$((cnt+1))
  fi
done
