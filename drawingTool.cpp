//
// Created by maty on 1/31/18.
//
#include <cmath>
#include <fstream>
#include "main.h"

struct GraphicVertex {
  int x, y;
  uint8_t r, g, b;
  vector<int> e;
};

// A C++ program to check if two given line segments intersect
#include <iostream>
using namespace std;

// https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
struct Point
{
    int x;
    int y;
};

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r)
{
  return q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
   q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y);

}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r)
{
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1: 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}




struct Pixel { uint8_t r, g, b; };

struct Picture {
  Picture(int width, int height) : width(width), height(height), matrix(vector<vector<Pixel>>{width, vector<Pixel>{height, {255, 255, 255}}}) {}

  int width, height;
  vector<GraphicVertex> vertices;
  vector<vector<Pixel>> matrix;

  void drawLine(int x1, int y1, int x2, int y2) {
    double len = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
    const int dx = x2 - x1;
    const int dy = y2 - y1;
    for (int i = 0; i < len; ++i) {
      const int x = x1 + (int)(((double)i/len)*dx);
      const int y = y1 + (int)(((double)i/len)*dy);
      matrix[x][y].r = 0;
      matrix[x][y].g = 0;
      matrix[x][y].b = 0;
    }
  }

  void drawVertices() {
    for (int i = 0; i < vertices.size(); ++i) {
      // draw a circle
      for (int j = -5; j <= 5; ++j) {
        for (int k = -5; k <= 5; ++k) {
          if (j*j + k*k > 25) continue;
          const int x = vertices[i].x + k;
          const int y = vertices[i].y + j;
          matrix[x][y].r = vertices[i].r;
          matrix[x][y].g = vertices[i].g;
          matrix[x][y].b = vertices[i].b;
        }
      }
    }
  }

  void drawGraphIntoMatrix() {
    drawVertices();
      // draw all edges
    for (int i = 0; i < vertices.size(); ++i) {
      for (int j = 0; j < vertices[i].e.size(); ++j) {
        drawLine(vertices[i].x, vertices[i].y, vertices[ vertices[i].e[j] ].x, vertices[ vertices[i].e[j] ].y);
      }
    }
  }


  void saveToFile(const string & path) {
    ofstream outputFile(path);
    outputFile << "P6" << endl << width << endl << height << endl << 255 << endl;
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        outputFile << (char)matrix[j][i].r;
        outputFile << (char)matrix[j][i].g;
        outputFile << (char)matrix[j][i].b;
      }
    }
  }
};

int main() {
  Graph g;
  g.loadFromStdin();
  srand(time(NULL));
  vector<pair<int,int>> pos(g.size());

  // attempt to minimize edge intersections
  int intersectsLimit = 0;
  int intersects = intersectsLimit + 1;
  int attempts = 0;
  while (intersects > intersectsLimit) {
    for (int i = 0; i < g.size(); ++i) {
      pos[i].first = rand() % 620 + 10;
      pos[i].second = rand() % 460 + 10;
    }
    cout << "intersectsLimit: " << intersectsLimit << " (" << attempts << ")   \r";
    attempts ++;
    if (attempts > 10000) {
      attempts = 0;
      intersectsLimit ++;
      if (intersectsLimit > 40) intersectsLimit *= 2;
    }
    intersects = 0;

    for (int i = 0; i < g.size(); ++i) {
      for (int j = 0; j < g.vertices[i].edges.size(); ++j) {
        const int u = g.vertices[i].edges[j];

        for (int k = 0; k < g.size(); ++k) {
          if (k == i || k == u) continue;

          for (int l = 0; l < g.vertices[k].edges.size(); ++l) {
            const int v = g.vertices[k].edges[l];
            if (v == u || v == i) continue;

            if (doIntersect({pos[i].first, pos[i].second}
              , {pos[u].first, pos[u].second}
              , {pos[k].first, pos[k].second}
              , {pos[v].first, pos[v].second})) intersects ++;
          }
        }
      }
    }
  }
  cout << endl;



  // load guards from stdin
  int states;
  cin >> states;
  Picture pic(640, 480);
  // draw the graph
  for (int i = 0; i < g.vertices.size(); ++i) {
    pic.vertices.push_back({pos[i].first, pos[i].second, 0, 0, 0});
    for (int j = 0; j < g.vertices[i].edges.size(); ++j) {
      pic.vertices[i].e.push_back(g.vertices[i].edges[j]);
    }
  }
  pic.drawGraphIntoMatrix();

  for (int j = 0; j < states; ++j) {
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i) {
      int t;
      cin >> t;
      pic.vertices[t].r = 255;
      pic.vertices[t].g = 0;
      pic.vertices[t].b = 0;
    }
    pic.drawVertices();

    pic.saveToFile("states/state" + to_string(j) + ".pmm");

    for (int i = 0; i < g.size(); ++i) {
      pic.vertices[i].r = 0;
    }
  }
}

