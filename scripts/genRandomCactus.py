import random

# n = int(random.randrange(10, 60))
n = 80

v = 0

art = set()
edges = []

while n > 0:
    x = v + random.randrange(1, 15)

    for i in range(v, x - 1):
        print(i, i + 1)

    if (v > 0):
        w = random.randrange(0, v)

        while w in art:
            w = random.randrange(0, v)

        art.add(w)

        print(w, v)
        print(w, x-1)
    else:
        print(v, x-1)

    v = x
    n -= x
