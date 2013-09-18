import sys
import MapReduce

# Cutting off the string algorithm found here:
# http://stackoverflow.com/questions/1798465/python-remove-last-3-characters-of-a-string
def mapper(record):
	val = record[0]
	key = record[1][:-10]
	mr.emit_intermediate(key,val)

def reducer(key, list_of_values):
	total = 0
	for v in list_of_values:
		total += 1
	if total is 1:
		mr.emit(key)

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)