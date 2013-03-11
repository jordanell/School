# Imports
import math

# Input for program
matrix = [[1, 1, 1, 1], 
					[0.9, 1, 1, 1],
					[0.9, 0.875, 1, 1],
					[0.7, 0.75, 1, -1],
					[0.6, 0.875, 1, -1],
					[0.6, 0.875, 1, 1],
					[0.5, 0.75, 1, -1],
					[0.5, 0.8125, 1, -1],
					[0.5, 1, 1, 1],
					[0.5, 0.875, 1, -1],
					[0.5, 0.875, 1, 1]]

def list_average(list):
	sum = 0
	for x in range(0, len(list)):
		sum += list[x]
	return sum / len(list)

def exp_calc(y, x, w_old):
	x = (matrix[y][3]*matrix[y][x]) / ( 1 + math.exp(matrix[y][3]*((w_old[0]*matrix[y][0]) + (w_old[1]*matrix[y][1]) + (w_old[2]*matrix[y][2]) )))
	return x


def log_step(w_old, w_new, k):
	g1, g2, g3 = [], [], []
	for y in range(0, 11):
		g1.append(exp_calc(y,0,w_old))
		g2.append(exp_calc(y,1,w_old))
		g3.append(exp_calc(y,2,w_old))
	w_new[0] = w_old[0] + k*(list_average(g1))
	w_new[1] = w_old[1] + k*(list_average(g2))
	w_new[2] = w_old[2] + k*(list_average(g3))
	print 'The new weight vector'
	print 'W1: {0}'.format(w_new[0])
	print 'W2: {0}'.format(w_new[1])
	print 'B:  {0}'.format(w_new[2])


# Globals
k = 2
w_old = [0, 0, 0]
w_new = [0, 0, 0]

# Main program entrance
while True:
	enter = raw_input('Press enter to preform a single step in logistic regression or type exit to stop: ')
	if enter == 'exit':
		break
	print 'Preforming one step of regression'
	log_step(w_old, w_new, k)
	w_old = w_new
	print''



