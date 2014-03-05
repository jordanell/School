# This file is used as the base class crawler for any web forum crawler.
#
# This class is set up as a skeleton. In order to properly crawl a web page
# with a subclass of crawler, parse_forum should be fully implemented in
# the subclass.

import urlparse
import urllib
from bs4 import BeautifulSoup
from sets import Set
from database import db_connector
from pos_extractor import pos_extractor
from sentiment import analyzer

class forum_crawler:
	base_url = ""

	url_pending = Set([])
	visiting = None
	visited = Set([])

	db = db_connector()

	pos = pos_extractor(None)
	sent_analyzer = analyzer()

	def __init__(self, base_url):
		self.base_url = base_url
		self.url_pending.add(base_url)

	def get_base_url(self, url):
		if "#" in url:
			return url[:url.find("#")]
		return url

	#Place holder and should be implemented in real crawlers.
	def parse_forum(self, soup):
		return

	def get_urls(self, soup):
		for tag in soup.findAll('a', href=True):
			if tag['href'].startswith("?"):
				tag['href'] = str(self.visiting) + tag['href']
			else:
				tag['href'] = urlparse.urljoin(self.base_url, tag['href'])
			base = self.get_base_url(tag['href'])
			if self.base_url in base and base not in self.visited and base not in self.url_pending:
				self.url_pending.add(base)
				self.parse_forum(soup)
		print 'Pending: ' + str(len(self.url_pending))
		print 'Visited: ' + str(len(self.visited))

	def visit_page(self, soup):
		self.get_urls(soup)

	def visit_from_stack(self):
		while len(self.url_pending) > 0:
			attempt = self.url_pending.pop()
			try:
				html_text = urllib.urlopen(attempt).read()
			except:
				print 'URL not reachable: ' + str(attempt)
			soup = BeautifulSoup(html_text)
			self.visiting = attempt
			self.visited.add(attempt)
			self.visit_page(soup)