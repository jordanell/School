import urlparse
import urllib
from bs4 import BeautifulSoup
from sets import Set
from crawler import crawler
import datetime

class hearthstone_crawler(crawler):

	def __init__(self, base_url):
		crawler.__init__(self, base_url)

	def get_title(self, soup):
		return soup.find("span", {"class": "topic-title"}).string

	def get_discussion_id(self):
		if "?" in self.visiting:
			return self.visiting[:self.visiting.find("?")]
		elif "#" in self.visiting:
			return self.visiting[:self.visiting.find("#")]
		else:
			return self.visiting

	def parse_discussion(self, soup):
		discussion_id = self.get_discussion_id()
		if self.db.discussion_exists(discussion_id) is True:
			return
		title = self.get_title(soup)
		self.db.insert_discussion(discussion_id, title, None)

	def get_user_name(self, post):
		return post.find("div", {"class": "context-user"}).get_text().strip().replace("\n", "")

	def get_user_title(self, post):
		user_type = post.find("div", {"class": "forum-title"})
		if user_type is not None:
			return user_type.get_text().strip()
		return None

	def get_time(self, post):
		date = post.find("div", {"class": "date"}).get("data-tooltip")
		return datetime.datetime.strptime(date, "%m/%d/%Y %I:%M %p %Z").strftime("%Y-%m-%d %H:%M %Z")

	def get_body(self, post):
		try:
			return post.find("div", {"class": "post-detail", "itemprop": "text"}).get_text()
		except UnicodeEncodeError:
			return None

	def parse_post(self, post):
		discussion_id = self.get_discussion_id()
		time = self.get_time(post)
		user = self.get_user_name(post)
		user_title = self.get_user_title(post)
		body = self.get_body(post)
		self.db.insert_post(discussion_id, time, user, user_title, body)

	def get_posts(self, soup):
		for post in soup.findAll("div", {"class": "topic-post"}):
			self.parse_post(post)

	def parse_forum(self, soup):
		if soup.find("div", {"id": "post-list"}) is None:
			return
		self.parse_discussion(soup)
		self.get_posts(soup)

def main():
	hs_crawler = hearthstone_crawler("http://us.battle.net/hearthstone/en/forum/")
	hs_crawler.visit_from_stack()

if __name__ == '__main__':
  main()