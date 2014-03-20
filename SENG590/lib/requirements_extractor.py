from database 			import db_connector
from pos_extractor 	import pos_extractor
from collections 		import Counter

import re
import math

def counter_cosine_similarity(l1, l2):
	c1 = Counter(l1)
	c2 = Counter(l2)
	terms = set(c1).union(c2)
	dotprod = sum(c1.get(k, 0) * c2.get(k, 0) for k in terms)
	magA = math.sqrt(sum(c1.get(k, 0)**2 for k in terms))
	magB = math.sqrt(sum(c2.get(k, 0)**2 for k in terms))
	return dotprod / (magA * magB)

def nouns(pos):
	nouns = []
	for p in pos:
		part = p[1]
		word = p[0]
		if part.startswith("NN") and word.lower() != "i":
			nouns.append(p[0].lower())
	return nouns

def verbs(pos):
	nouns = []
	for p in pos:
		part = p[1]
		if part.startswith("VB"):
			nouns.append(p[0].lower())
	return nouns

def nouns_and_verbs(pos):
	nouns = []
	for p in pos:
		part = p[1]
		if part.startswith("VB") or part.startswith("NN"):
			nouns.append(p[0].lower())
	return nouns


db = db_connector()

pairs = []
bins = []

for post in db.get_all_posts():
	date_regex 	= re.compile(r'\d\d/\d\d/\d\d\d\d', re.I)
	sentences 	= re.split(r' *[\.\?!][\'"\)\]]* *', post[2])

	for sentence in sentences:
		if not date_regex.findall(sentence):
			continue
		pos = pos_extractor(sentence)
		pairs.append((sentence, nouns(pos.extract_pos())))


for root in pairs:
	if not root[1]:
		continue

	for bin in bins:
		add = True
		for part in bin:
			if (root != part) and root[1] and part[1]:
				if counter_cosine_similarity(root[1], part[1]) < .4:
					add = False
					break
		if add:
			bin.append(root)

	if root[1]:
		bins.append([root])

for bin in bins:
	print "BIN COUNT: " + str(len(bin))
	if len(bin) >= 3:
		print bin