# OpenRPG

Welcome to your new gem! In this directory, you'll find the files you need to be able to package up your Ruby library into a gem. Put your Ruby code in the file `lib/open_rpg`. To experiment with that code, run `bin/console` for an interactive prompt.

TODO: Delete this and the text above, and describe your gem

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'open_rpg'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install open_rpg

## Dependencies

### GLFW

OpenRPG uses [GLFW](https://www.glfw.org/) for creating an OpenGL context and providing native window management. The library is cross-platform, and available on all major Linux distributions, macOS, and Windows. GLFW is licensed under the liberal [zlib/libpng](https://www.glfw.org/license.html) license, which allows free distribution, including commercial applications.

The binaries for GLFW must be located on the system where they can be found by the application. Binaries for select systems can be downloaded manually [here](https://www.glfw.org/download.html).

### Microsoft Windows
Installing the `open_rpg-ms` gem is the easiest method, which will include the dependencies locally, and will load the appropriate binary for the target CPU architecture.

### macOS
Installing the `open_rpg-mac` gem is the easiest method, which will include the dependencies locally

### Linux
Install using your distributions package manager. The package is typically labeled simply as `glfw`, though this may vary by distribution. If desired, or if no package is available, GLFW can be compiled from source. Instructions for compiling manually can be found [here](https://www.glfw.org/docs/latest/compile.html).

#### Arch Linux
```
pacman -S glfw
```
#### Ubuntu/Debian
```
apt-get install libglfw3
apt-get install libglfw-dev
```


## Usage

TODO: Write usage instructions here

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake test` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/[USERNAME]/open_rpg. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [Contributor Covenant](http://contributor-covenant.org) code of conduct.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the OpenRpg project’s codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/open_rpg/blob/master/CODE_OF_CONDUCT.md).
