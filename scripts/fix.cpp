#include <bits/stdc++.h>
using namespace std;

int main() {
  vector<int> a;
  int x;
  int n = -1;
  while (cin >> x) {
    a.push_back(x);
    n = max(n, x);
  }
  cout << n + 1 << endl;
  for (int i = 0; i < a.size(); ++ i) {
    cout << a[i];
    if (i % 2 == 0) cout << " ";
    else cout << endl;
  }
  return 0;
}
