import sys
import MapReduce

# Can't do this without seeing the input file

def mapper(record):
	key = record[0]
	val = record[1]
	words = val.split()
	for w in words:
		mr.emit_intermediate(w,1)

def reducer(key, list_of_values):
	total = 0
	for v in list_of_values:
		total += v
	mr.emit((key, total))

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)