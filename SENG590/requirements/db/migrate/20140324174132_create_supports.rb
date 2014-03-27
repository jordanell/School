class CreateSupports < ActiveRecord::Migration
  def change
    create_table :supports do |t|
      t.text :body
      t.text :nouns

      t.timestamps
    end
  end
end
