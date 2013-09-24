import sys
import MapReduce

def mapper(record):
	key = record[1]
	val = record
	mr.emit_intermediate(key,val)

# Search tuples for order
# Handle not found by returning index of -1
def tuple_search(list_of_values):
	for x in range(0,len(list_of_values)):
		if list_of_values[x][0] == 'order':
			return x
	return -1

# Check if tuple is order
def tuple_is_order(v):
	return v[0] == 'order'

# Generate the final list of strings
def generate_row(order, v):
	row = []
	for item in order:
		row.append(item)
	for item in v:
		row.append(item)
	return row

def reducer(key, list_of_values):
	order = list_of_values[tuple_search(list_of_values)]
	for v in list_of_values:
		if not (tuple_is_order(v)):
			val = generate_row(order, v)
			mr.emit(val)

mr = MapReduce.MapReduce()
inputdata = open(sys.argv[1])
mr.execute(inputdata, mapper, reducer)