# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20140324174453) do

  create_table "discussions", force: true do |t|
    t.string   "discussion_id"
    t.string   "title"
    t.string   "topic"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "discussions", ["discussion_id"], name: "index_discussions_on_discussion_id", unique: true, using: :btree

  create_table "posts", force: true do |t|
    t.string   "user_name"
    t.text     "body"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "user_title"
    t.string   "discussion_id"
  end

  create_table "posts_unique", id: false, force: true do |t|
    t.integer  "id"
    t.string   "user_name"
    t.text     "body"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "user_title"
    t.string   "discussion_id"
  end

  create_table "requirements", force: true do |t|
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "supports", force: true do |t|
    t.text     "body"
    t.text     "nouns"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "requirement_id"
  end

end
