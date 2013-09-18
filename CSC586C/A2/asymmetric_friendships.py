import sys
import MapReduce

def mapper(record):
	key = record[0]
	val = record[1]
	if key < val:
		mr.emit_intermediate((key,val),1)
	else:
		mr.emit_intermediate((val,key),1)

def reducer(key, list_of_values):
	total = 0
	for v in list_of_values:
		total += 1
	if total <= 1:
		mr.emit((key[0], key[1]))
		mr.emit((key[1], key[0]))

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)