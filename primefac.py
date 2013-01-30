import sys

n = int(sys.argv[1])

z = 2

while z*z <= n:
  if n%z == 0:
    print z
    n /= z
  else:
    z += 1

print n
