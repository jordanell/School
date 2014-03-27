from database 			import db_connector
from pos_extractor 	import pos_extractor
from collections 		import Counter

import re
import math

ignore_words = ("amposted", "pmposted")

requirement_words = ("want", "wish", "should", "think", "could", "might", "hope")

db = db_connector()

pairs = []
bins 	= []

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

def contains_ignore_words(sentence):
	words = sentence.split()
	words = [word.lower() for word in words]
	for ignore_word in ignore_words:
		if ignore_word in words:
			return True
	return False

def generate_pairs():
	for post in db.get_all_posts():
		sentences 	= re.split(r' *[\.\?!][\'"\)\]]* *', post[2])

		for sentence in sentences:
			# Ignore first part of quoted text
			if contains_ignore_words(sentence):
				continue

			pos = pos_extractor(sentence)
			pairs.append((sentence, nouns(pos.extract_pos())))

def generate_bins():
	i = 0
	for root in pairs:
		i += 1
		print "Generating pair " + str(i) + " / " + str(len(pairs))
		# Only process if is has nouns
		if not root[1]:
			continue

		# Add to existing requirement bins
		for bin in bins:
			add = True
			# Scan each part of each bin
			for part in bin:
				# Do not add duplicates to bins
				if root[0].lower() == part[0].lower():
					add = False
					break
				if root[1] and part[1]:
					if counter_cosine_similarity(root[1], part[1]) < .4:
						add = False
						break
			if add:
				bin.append(root)

		if root[1]:
			bins.append([root])

def output():
	db.clear_output()
	i = 0
	for bin in bins:
		print "Outputting " + str(i) + " / " + str(len(bins))
		db.insert_requirement(i)

		insert = False
		for part in bin:
			for word in part[0].split():
				if word in requirement_words:
					out = True
					break;
			if insert:
				break

		if insert:
			for part in bin:
				nouns = ", ".join(part[1])
				body 	= part[0]
				db.insert_support(i, body, nouns)
		i += 1

def main():
	print "Clearing output"
	db.clear_output()
	print "Extracting terms."
	generate_pairs()
	print "Generating bins."
	generate_bins()
	print "Outputting to DB."
	output()

if __name__ == "__main__":
	main()