for i in ../inputs/*; do
  echo "testing: $i"
  ../medn -qhec $i >/dev/null
  ../../mcs/a.out < configGraph.in
done
