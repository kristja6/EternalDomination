cnt=0
iter=0
while true; do
  python3 ./scripts/randomGraph.py > inputs/random10a.in;
  rm -r configGraphs
  mkdir configGraphs
  echo "running... ($iter)"
  iter=$((iter + 1))
  ./medn -ehs inputs/random10a.in
  head -n 1 configGraphs/configGraph1_cmp_.in > /tmp/a.txt
  ./check.out < /tmp/a.txt

  if [ $? -eq 0 ]; then
    echo "found!"
    cnt=$((cnt + 1))
    cp inputs/random10a.in nonCliqueStrategies/res$cnt.in
    cp configGraphs/configGraph1_cmp_.in nonCliqueStrategies/str_$cnt.in
  fi
done
