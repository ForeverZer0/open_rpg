lib = File.expand_path('lib', __dir__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'open_rpg/version'

Gem::Specification.new do |spec|
  spec.name          = 'open_rpg'
  spec.version       = OpenRPG::VERSION
  spec.authors       = ['ForeverZer0']
  spec.email         = ['efreed09@gmail.com']

  spec.summary       = %q{Powerful, cross-platform 2D game engine, heavily inspired by RPG Maker XP/VX, with many additional features.}
  # spec.description   = %q{Write a longer description or delete this line.} FIXME: 
  spec.homepage      = 'https://github.com/ForeverZer0/open_rpg'
  spec.license       = 'MIT'

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = 'https://github.com/ForeverZer0/open_rpg'
  spec.metadata['changelog_uri'] = 'https://github.com/ForeverZer0/open_rpg/blob/master/CHANGELOG.md'

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files         = Dir.chdir(File.expand_path('..', __FILE__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end

  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/open_rpg/extconf.rb']

  spec.add_development_dependency 'bundler', '~> 2.0'
  spec.add_development_dependency 'rake', '~> 12.0'
  spec.add_development_dependency 'rake-compiler', '~> 1.0'
end
