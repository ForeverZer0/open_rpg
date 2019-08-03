require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/colors'
require_relative 'open_rpg/scene'
require_relative 'open_rpg/game'
require_relative 'open_rpg/cache'

##
# Top-level namepsace for the OpenRPG API.
#
# @author Eric Freed
module OpenRPG

  class TestScene < Scene

    include Input

    def initialize

      # Mouse.change_cursor('/home/eric/Pictures/cursor.png', 0, 0)

      r1 = Rect.new(0, 0, 0, 32)
      r2 = Rect.new(0, 0, 32, 0)
      r3 = Rect.new(0, 32, 0, 0)
      r4 = Rect.new(32, 0, 0, 0)

      a = Windowskin.new
      a.set_cursors(r1, r2, r3, r4)
      p a.cursors

      icon_path = '/home/eric/Pictures/arc-icon.png'
      App.set_icon(Image.from_file icon_path)

      path = '/home/eric/Pictures/RTP/XP/Graphics/Characters/001-Fighter01.png'
      img = Image.from_file(path)

      img.draw_text(0, 0, img.width, img.height, 'OpenRPG')


      @sprite = Sprite.new(image: img)
   
      
    end

    def close
      @sprite.dispose(true)
    end

    def update
      p Mouse.location
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
      if Keyboard.trigger?(Key::H) || Keyboard.repeat?(Key::H)
        @sprite.hue += 6.0
      end
      if Keyboard.trigger?(Key::F)
        @sprite.flash(Colors.green, 8)
      end
      if Keyboard.trigger?(Key::G)
        @sprite.tone = Tone.new(0, 0, 0, 255)
      end
      if Keyboard.trigger?(Key::C)
        Graphics.capture.save('capture.png', Image::FORMAT_PNG)
      end
    end

  end



  Graphics.create(800, 600, "OpenRPG #{VERSION}") 
  Graphics.bg_color = Colors.cornflower_blue

  Game.goto(TestScene)
  Game.main

  Graphics.destroy

end