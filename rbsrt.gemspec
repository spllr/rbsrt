$:.push File.expand_path("../lib", __FILE__)

require "rbsrt/version"

Gem::Specification.new do |s|
  s.name        = 'rbsrt'
  s.version     = SRT::VERSION
  s.licenses    = ['MPLv2.0', 'MIT']
  s.summary     = "Ruby API for SRT (Secure Reliable Transport)"
  s.description = "Build SRT (Secure Reliable Transport) servers and clients with Ruby"
  s.authors     = ["Klaas Speller"]
  s.email       = ["klaas@recce.nl"]
  s.files       = Dir['README*.md', 'lib/**/*', 'ext/**/*', 'bin/**/*'] + ['Rakefile', 'rbsrt.gemspec']
  s.test_files  = Dir['test/**/*_test.rb']
  s.bindir      = "bin"
  s.executables = ["rbsrt-server", "rbsrt-client"]  
  s.homepage    = 'https://github.com/spllr/rbsrt'
  s.metadata    = { "source_code_uri" => "https://github.com/spllr/rbsrt" }
  s.extensions  = %w[ext/rbsrt/extconf.rb]
  s.required_ruby_version = ">= 2.2.0"
  s.add_development_dependency "rake-compiler", "~> 1.0"
  s.add_development_dependency "gem-release", "~> 2.2"
end
