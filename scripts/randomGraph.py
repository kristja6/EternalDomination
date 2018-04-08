import random
n = int(input(""))
m = int(input(""))

print(m)
for i in range(0, m):
    print(str(random.randrange(0, n)), str(random.randrange(0, n)))
