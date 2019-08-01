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

      path = '/home/eric/Pictures/RTP/XP/Graphics/Characters/001-Fighter01.png'
      img = Image.load(path)
      sub = img.slice(32 * 3, 48 * 3, 32, 48)

      dst = Rect.new(32 * 3, 48, 64, 48)
      img.blit(sub, sub.rect, dst)

      @sprite = Sprite.new(nil, img)
      @sprite.src_rect = img.rect
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

  end


  Graphics.create(800, 600, "OpenRPG #{VERSION}") 
  Graphics.bg_color = Colors.cornflower_blue

  Game.goto(TestScene)
  Game.main

  Graphics.destroy

end


