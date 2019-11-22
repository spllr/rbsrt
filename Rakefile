require 'bundler'
require 'rake/extensiontask'

Bundler::GemHelper.install_tasks

task :default => :test

# desc "Generate Makefile"
# file "Makefile" do
#   cd File.expand_path("../ext/rbsrt/extconf", __FILE__)
#   require File.expand_path("../ext/rbsrt/extconf", __FILE__)
# end
#
# desc "Compile extension"
# task :compile => ["Makefile"] do
#   sh "make"
# end

desc "Open an irb session with rbsrt library"
task :console do
  sh "irb  -r ./lib/rbsrt.rb"
end

Rake::ExtensionTask.new 'rbsrt' do |ext|
  ext.lib_dir = "lib/rbsrt"
end

desc "Run tests"
task :test => [ :compile ] do
    require './test/test_helper'
    Dir.glob( './test/**/*_test.rb').each { |file| require file }
end