require "mkmf"

def linux?
  have_macro('linux') || have_macro('__linux')
end

def windows?
  have_macro('_WIN32')
end

def apple?
  have_macro('_APPLE_') && have_macro('_MACH_')
end

def create_config
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

    if linux?
      io.puts "#define RPG_LINUX"
    elsif windows?
      io.puts "#define RPG_WINDOWS"
    elsif apple?
      io.puts "#define RPG_APPLE"
    else
      abort('unsupported platform')
    end

    io.puts
    io.puts '#endif /* OPEN_RPG_CONFIG_H */'
  end
end

def target_all_sources
  dir_config('open_rpg/open_rpg')
  $srcs = Dir.glob("#{$srcdir}/**/*.c").map { |path| File.basename(path) }
  Dir.glob("#{$srcdir}/*/") do |path|
    dir =  File.basename(path)
    $VPATH << "$(srcdir)/#{dir}"
    $INCFLAGS << " -I$(srcdir)/#{dir}"
  end
end

def find_openal
  abort unless have_library('openal')
  abort unless have_library('sndfile')
  # abort unless have_library('soundio')
  # abort unless have_library('vorbisfile')
end

def find_zlib
  abort unless have_library('z')
end

def find_libxml2
  # TODO: Test on Windows (MSYS base SHOULD have this installed...)
  paths = ['/opt', '/opt/local', '/usr/local', '/usr'].map { |path| path += '/include/libxml2' }
  unless find_header('libxml/xmlversion.h', *paths)
    abort('unable to locate libxml2 include directory')
  end
  paths = ['/opt/lib', '/opt/local/lib', '/usr/local/lib', '/usr/lib', '/lib']
  unless find_library('xml2', 'xmlParseDoc', *paths)
    abort('unable to locate libxml library')
  end
end

def find_packages(*packages)
  list = packages.join(' ')
  $LDFLAGS << " #{`pkg-config --static --libs #{list}`}"
  $CFLAGS << " #{`pkg-config --cflags #{list}`}"
end

create_config
find_zlib
find_libxml2
find_openal
find_packages('glfw3', 'freetype2')
target_all_sources



# TODO: Windows configuration...ugh

create_makefile("open_rpg/open_rpg")
