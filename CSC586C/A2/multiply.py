import sys
import json
import MapReduce

# Basic algorithm learned from here:
# http://adhoop.wordpress.com/2012/03/31/matrix-multiplication-using-mapreduce-1-step-solution/

def mapper_1(record):
	matrix = record[0]
	if matrix == 'a':
		mr.emit_intermediate(('a',record[2]),1)
	elif matrix == 'b':
		mr.emit_intermediate(('b',record[1]),1)

def reducer_1(key, list_of_values):
	if key[0] is 'a' and rows is 0:
		rows = len(list_of_values)
	elif key[0] is 'b' and cols is 0:
		cols = len(list_of_values)

def mapper_2(record):
	matrix = record[0]
	if matrix == 'a':
		for k in range(1,cols+1):
			emit_intermediate((record[0],k),(('a',record[2]),record[3]))
	elif matrix == 'b':
		for i in range(1,rows+1):
			emit_intermediate((i,record[2]),(('b',record[1]),record[3]))

# Fancy tuple searching found here:
# http://stackoverflow.com/questions/2917372/how-to-search-a-list-of-tuples-in-python
def tuple_search(list_of_values, matrix, j):
	try:
		[x[0] for x in list_of_values].index((matrix,j))
	except IndexError:
		return 'null'

def multiplier(list_of_values):
	sum = 0
	for j in range(n):
		a_val = tuple_search(list_of_values, 'a', j)
		b_val = tuple_search(list_of_values, 'b', j)
		if a_val == 'null' or b_val == 'null':
			return sum
		sum += (a_val * b_val)
	return sum


def reducer_2(key, list_of_values):
	val = multiplier(list_of_values)
	mr.emit(key,val)

rows = 0
cols = 0

mr_1 = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr_1.execute(inputdata, mapper_1, reducer_1)

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper_2, reducer_2)