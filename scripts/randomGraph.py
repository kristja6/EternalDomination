import random
# n = int(input(""))
# m = int(input(""))

n = 15
m = random.randrange(n - 1, (n*(n-1))//2)
print(n)

for i in range(0, m):
    print(str(random.randrange(0, n)), str(random.randrange(0, n)))
