require "mkmf"

unless have_library('xml2')
  abort('Unable to locate libxml2')
end


# Generate file for creating a few macros we can use, much more robust
# and error-proof using Ruby to get this information across platforms.
base = File.expand_path(File.join(__dir__, '..', '..'))
config = File.join(__dir__, 'config.h')
File.open(config, 'wb') do |io|
  io.puts '#ifndef OPEN_RPG_CONFIG_H'
  io.puts '#define OPEN_RPG_CONFIG_H 1'
  io.puts
  io.puts "#define RPG_BASE \"#{base}\""
  io.puts "#define RPG_ASSETS \"#{base}/assets\""
  io.puts "#define RPG_FONTS \"#{base}/assets/fonts\""
  io.puts "#define RPG_SHADERS \"#{base}/assets/shaders\""
  io.puts
  io.puts '#endif /* OPEN_RPG_CONFIG_H */'
end


$LDFLAGS += " #{`pkg-config --static --libs glfw3 freetype2`}"
$CFLAGS += " #{`pkg-config --cflags glfw3 freetype2`}"

# TODO: Windows configuration...ugh

create_makefile("open_rpg/open_rpg")
