from topia.termextract import tag
from topia.termextract import extract

class pos_extractor:

	tagger = tag.Tagger()
	extractor = extract.TermExtractor()

	text = None

	def __init__(self, text):
		self.tagger = tag.Tagger()
		self.tagger.initialize()
		self.extractor = extract.TermExtractor(self.tagger)
		self.text = text
		if self.text is not None:
			self.tagger.tokenize(self.text)

	def set_text(self, text):
		self.text = text
		self.tagger.tokenize(self.text)

	def extract_pos(self):
		if self.text is not None:
			return self.tagger(self.text)
		return None

def main():
	pos = pos_extractor("I think that setting mind control to 9 mana would be good for preists.")
	print pos.extract_pos()
	print pos.extract_keywords()

if __name__ == '__main__':
  main()