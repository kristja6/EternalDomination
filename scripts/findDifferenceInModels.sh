a=0
while true; do
  n=$(($RANDOM*32768 + $RANDOM))
  python3 randomGraph.py > /tmp/$n.in
  echo "running... ($a)"
  a=$((a+1))
  diff <(../medn -eq /tmp/$n.in) <(../medn -eqm /tmp/$n.in) > /dev/null;
  if [ $? -ne 0 ]; then
    echo "Found!"
    cnt=$(ls -la difference | wc -l)
    cnt=$((cnt+1))
    cp /tmp/$n.in ./difference/$cnt.in
    echo "This is $cnt so far"
  fi
  rm /tmp/$n.in
done
