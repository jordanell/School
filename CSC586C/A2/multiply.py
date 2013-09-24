import sys
import json
import MapReduce

# Basic algorithm learned from here:
# http://adhoop.wordpress.com/2012/03/31/matrix-multiplication-using-mapreduce-1-step-solution/

def compute_sizes():
	global rows
	global cols
	data = open(sys.argv[1])
	for line in data:
		record = json.loads(line)
		if record[0] == 'a' and record[1]+1 > rows:
			rows = record[1]+1
		elif record[0] == 'b' and record[2]+1 > cols:
			cols = record[2]+1

def mapper(record):
	matrix = record[0]
	if matrix == 'a':
		for k in range(0,cols):
			mr.emit_intermediate((record[1],k),(('a',record[2]),record[3]))
	elif matrix == 'b':
		for i in range(0,rows):
			mr.emit_intermediate((i,record[2]),(('b',record[1]),record[3]))

# Fancy tuple searching found here:
# http://stackoverflow.com/questions/2917372/how-to-search-a-list-of-tuples-in-python
# Handle the 0 entries here by return index of -1
def tuple_search(list_of_values, matrix, j):
	try:
		return [x[0] for x in list_of_values].index((matrix,j))
	except:
		if j < rows:
			return -1
		return None

# Multiple for the answer
# Handle the out of range as the end of the matrix and the -1 as the
# zero values in the sparse matrix
def multiplier(list_of_values):
	sum = 0
	for j in range(0,200):
		a_index = tuple_search(list_of_values, 'a', j)
		b_index = tuple_search(list_of_values, 'b', j)
		if a_index == None or b_index == None:
			return sum
		if a_index == -1:
			a_val = 0
		else:
			a_val = list_of_values[a_index][1]
		if b_index == -1:
			b_val = 0
		else:
			b_val = list_of_values[b_index][1]
		if a_val == None or b_val == None:
			return sum
		sum += (a_val * b_val)
	return sum


def reducer(key, list_of_values):
	val = multiplier(list_of_values)
	mr.emit((key[0],key[1],val))

rows = 0
cols = 0

compute_sizes()
mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)