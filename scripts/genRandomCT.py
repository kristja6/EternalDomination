import random

# n = int(random.randrange(10, 60))
n = 500

v = 0

edges = []

while n > 0:
    x = v + random.randrange(1, 6)

    for i in range(v, x):
        for j in range(i + 1, x):
            print(i, j)

    # connect to random
    if v > 0:
        w = random.randrange(0, v)
        for i in range(v, x):
            print(i, w)

    v = x
    n -= x
