n = 6
for i in range(n):
    for j in range(n):
        for x in range(-1, 2):
            for y in range(-1, 2):
                if (x*x + y*y == 1 and i+y >= 0 and i+y < n and j+x >= 0 and j+x < n):
                    print(i*n + j, (i+y)*n + (j+x))
