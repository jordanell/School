# Imports
import math
import sys

def read_input(file_name):
	input = open(file_name, "r")
	for line in input:
		if not line.startswith("#") and not line.isspace():
			matrix.append([float(n) for n in line.split()])


def list_average(list):
	sum = 0
	for x in range(0, len(list)):
		sum += list[x]
	return sum / len(list)

def get_exp(y, width, w_old):
	e = 0
	for x in range(0, width-1):
		e += (w_old[x] * matrix[y][x])
	return e


def exp_calc(y, x, width, w_old):
	e = get_exp(y,width,w_old)
	x = (matrix[y][width-1]*matrix[y][x]) / ( 1 + math.exp(matrix[y][width-1]*(e)))
	return x

def e_calc(width, w_old):
	e = []
	for y in range(0, height):
		e.append(math.log(1+math.exp((-1*matrix[y][width-1])*(get_exp(y,width,w_old)))))
	avg = list_average(e)
	print ''
	print 'E:  {0}'.format(avg)


def log_step(w_old, w_new, k, width, height):
	for x in range(0, width-1):
		g = []
		for y in range(0, height):
			g.append(exp_calc(y,x,width,w_old))
		w_new[x] = w_old[x] + k*(list_average(g))
	print 'The new weight vector'
	for x in range(0, width-1):
		if x < width-2:
			print 'W{0}: {1}'.format(x+1, w_new[x])
		else:
			print 'B:  {0}'.format(w_new[x])
	e_calc(width, w_old)


# Globals
k = 2
w_old = []
w_new = []

height = 0;
width = 0;
matrix = []

# Main program entrance
if len(sys.argv) >= 2:
	read_input(sys.argv[1])
else:
	print 'You must supply an input file.'
	exit()

height = len(matrix)
width = len(matrix[0])

for i in range(0, width-1):
	w_old.append(0)
	w_new.append(0)

while True:
	enter = raw_input('Press enter to preform a single step in logistic regression or type exit to stop: ')
	if enter == 'exit':
		break
	print 'Preforming one step of regression'
	log_step(w_old, w_new, k, width, height)
	w_old = w_new
	print''



