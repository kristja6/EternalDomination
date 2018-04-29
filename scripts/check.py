with open("log.txt", "r") as f:
    cnt = 0
    a = ""
    b = ""
    name = ""
    for line in f:
        #print("line: ", line)
        if cnt == 0:
            name = line
        elif cnt == 1:
            a = line
            if (a[0] == '.' and a[1] == '.'):
                name = a
                cnt = 0
        elif cnt == 2:
            b = line
            num1 = a.split(" ")[4]
            num2 = b.split(" ")[17]
            if (num1 != num2):
                print("error: ", name)

            cnt = -1

        cnt += 1


