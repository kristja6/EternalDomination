import random
# n = int(input(""))
# m = int(input(""))

# n = int(random.randrange(5, 25))
# m = int(random.randrange(n - 1, 3*n))
n = 50
m = 450

for i in range(0, m):
    print(str(random.randrange(0, n)), str(random.randrange(0, n)))
