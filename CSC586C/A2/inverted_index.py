import sys
import MapReduce

def mapper(record):
	val = record[0]
	keys = record[1]
	words = keys.split()
	for w in words:
		mr.emit_intermediate(w,val)

def reducer(key, list_of_values):
	values = []
	for v in list_of_values:
		if v not in values:
			values.append(v)
	mr.emit((key, values))

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)