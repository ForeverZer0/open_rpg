require 'rake/extensiontask'

task :build => :compile

Rake::ExtensionTask.new('open_rpg') do |ext|
  ext.lib_dir = 'lib/open_rpg'
end

task :default => [:clobber, :compile]
