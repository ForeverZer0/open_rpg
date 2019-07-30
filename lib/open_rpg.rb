require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/colors'
require_relative 'open_rpg/scene'
require_relative 'open_rpg/game'

##
# Top-level namepsace for the OpenRPG API.
#
# @author Eric Freed
module OpenRPG

  class TestScene < Scene


    include Input

    def initialize

      @viewport = Viewport.new(32, 32, 200, 200)
      @viewport.color = Tone.new(80, 120, 20)


      p @viewport.alpha
      p @viewport.visible

      @red = Sprite.new(@viewport, Bitmap.new(256, 256, Colors.red))
      @red.z = 250
      @red.alpha = 0.5


      @bitmap = Bitmap.new('/home/eric/Pictures/character.png')
      @sprite = Sprite.new(nil, @bitmap)
    end

    def update
      if Keyboard.press?(Key::UP)
        @sprite.y -= 8
      end
      if Keyboard.press?(Key::DOWN)
        @sprite.y += 8
      end
      if Keyboard.press?(Key::LEFT)
        @sprite.x -= 8
      end
      if Keyboard.press?(Key::RIGHT)
        @sprite.x += 8
      end
      if Keyboard.trigger?(Key::Z)
        @sprite.z += 100
      end
      if Keyboard.trigger?(Key::F)
        @sprite.flash(Colors.green, 8)
      end
      if Keyboard.trigger?(Key::G)
        @sprite.tone = Tone.new(0, 0, 0, 255)
      end
    end

    def close
      @sprite.dispose
      @red.dispose(true)
      @bitmap.dispose
      @viewport.dispose
    end

  end


  Graphics.create(800, 600, "OpenRPG #{VERSION}") 
  Graphics.bg_color = Colors.cornflower_blue

  Game.goto(TestScene)
  Game.main

  Game.scene.close

  Graphics.destroy

end


