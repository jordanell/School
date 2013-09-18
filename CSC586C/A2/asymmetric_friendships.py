import sys
import MapReduce

def mapper(record):
	key = record[0]
	val = record[1]
	if key < val:
		mr.emit_intermediate(key,val)
	else:
		mr.emit_intermediate(val,key)

def reducer(key, list_of_values):
	total = 0
	for v in list_of_values:
		total += 1
	if total is 1:
		mr.emit((key, list_of_values[0]))
		mr.emit((list_of_values[0], key))

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)