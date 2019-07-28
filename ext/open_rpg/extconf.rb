require "mkmf"

$LDFLAGS += " #{`pkg-config --static --libs glfw3 freetype2`}"
$CFLAGS += " #{`pkg-config --cflags glfw3 freetype2`}"

create_makefile("open_rpg/open_rpg")
