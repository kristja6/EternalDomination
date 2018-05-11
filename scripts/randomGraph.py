import random
# n = int(input(""))
# m = int(input(""))

n = int(random.randrange(10, 30))
m = int(random.randrange(n - 1, 6*n))

for i in range(0, m):
    print(str(random.randrange(0, n)), str(random.randrange(0, n)))
