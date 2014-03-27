class Requirement < ActiveRecord::Base
	has_many :supports, dependent: :destroy
end
