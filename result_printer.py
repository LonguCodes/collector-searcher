with open('result', 'rb') as f:
    i = 0
    non_empty = 0
    while True:
        res = f.read(4)
        if res == b'':
            break
        value = int.from_bytes(res, byteorder='little')
        print(i, value)
        if value != 0:
            non_empty += 1
        i += 1
    print("Not zero results: ", non_empty)
