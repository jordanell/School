class AddRequirementsToSupport < ActiveRecord::Migration
  def change
  	add_column :supports, :requirement_id, :integer
  end
end
