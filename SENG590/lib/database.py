# This file is used as a class to connect to a PostgreSQL database

import psycopg2
import ConfigParser

def ConfigSectionMap(section):
  dict1 = {}
  options = Config.options(section)
  for option in options:
    try:
      dict1[option] = Config.get(section, option)
      if dict1[option] == -1:
        DebugPrint("skip: %s" % option)
    except:
      print("exception on %s!" % option)
      dict1[option] = None
  return dict1

Config = ConfigParser.ConfigParser()
Config.read("defaults.cfg")

class db_connector:

	conn = None
	cur = None

	def __init__(self):
		self.conn = psycopg2.connect("dbname=" + ConfigSectionMap("SectionOne")['dbname'] + \
    	" user=" + ConfigSectionMap("SectionOne")['user'] + \
    	" password=" + ConfigSectionMap("SectionOne")['password'] + \
    	" host=" + ConfigSectionMap("SectionOne")['host'])
		self.cur = self.conn.cursor()

	def insert_discussion(self, discussion_id, title, created_at):
		if created_at is None:
			self.cur.execute(
				"""INSERT INTO discussions (discussion_id, title)
				VALUES(%s, %s);""", (discussion_id, title))
		else:
			self.cur.execute(
				"""INSERT INTO discussions (discussion_id, title, created_at)
				VALUES(%s, %s, %s);""", (discussion_id, title, created_at))
		self.commit()

	def update_discussion(self, discussion_id, updated_at):
		self.cur.execute(
			"""UPDATE discussions SET updated_at=%s WHERE discussion_id=%s;""", 
			(updated_at, discussion_id))

	def discussion_exists(self, discussion_id): 
		self.cur.execute(
			"""SELECT * FROM discussions WHERE discussion_id=%s;""", (discussion_id,))
		if self.cur.fetchone() is None:
			return False
		return True

	def insert_post(self, discussion_id, created_at, user_name, user_title, body):
		self.cur.execute(
			"""INSERT INTO posts (discussion_id, created_at, user_name, user_title, body)
			VALUES (%s, %s, %s, %s, %s);""", (discussion_id, created_at, user_name, user_title, body))
		self.commit()

	def commit(self):
		self.conn.commit()