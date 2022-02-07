import random

with open('input', 'wb') as f:
    for i in range(1024 * 1024 * 64):
        f.write(random.randint(0, 65535).to_bytes(2, 'little'))
