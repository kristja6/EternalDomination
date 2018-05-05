#!/usr/bin/env bash
for i in ../inputs/random_cacti/*; do
  echo "$i$"
  ../medn -e ${i}
done
