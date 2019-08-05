require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/colors'
require_relative 'open_rpg/scene'
require_relative 'open_rpg/game'
require_relative 'open_rpg/window/window'
require_relative 'open_rpg/cache'

##
# Top-level namepsace for the OpenRPG API.
#
# @author Eric Freed
module OpenRPG

  class TestScene < Scene

    include Input

    def initialize

      # @viewport = Viewport.new(32, 32, 400, 300)

      @window = Window.new(0, 0, 400, 200)
      @window.windowskin = Image.from_file('/home/eric/Pictures/Window.png')
      @window.contents = Image.new(600, 300, Colors.crimson)
      @window.draw_text(0, 0, 400, 100, 'Hello!', Align::CENTER)

      
      App.set_icon('/home/eric/Pictures/arc-icon.png')
      img = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Characters/001-Fighter01.png')
      img.draw_text(0, 0, img.width, img.height, 'OpenRPG')

      # @sprite = Sprite.new(nil, image: img)
      @sprite = Sprite.new(@viewport, image: img)
      @sprite.src_rect = Rect.new(32, 0, 32, 48)
      @sprite.x = 64
      @sprite.y = 64
   
    end

    def close
      # @viewport.dispose
      # @window.dispose(true)
      # @win.dispose
    end

    def update
      if Keyboard.press?(Key::UP)
        @window.y -= 8
      end
      if Keyboard.press?(Key::DOWN)
        @window.y += 8
      end
      if Keyboard.press?(Key::LEFT)
        @window.x -= 8
      end
      if Keyboard.press?(Key::RIGHT)
        @window.x += 8
      end
      if Keyboard.trigger?(Key::Z)
        @window.z += 100
      end
      if Keyboard.trigger?(Key::H) || Keyboard.repeat?(Key::H)
        @window.hue += 6.0
      end
      if Keyboard.trigger?(Key::F)
        @window.flash(Colors.green, 8)
      end
      if Keyboard.trigger?(Key::G)
        @window.tone = Tone.new(0, 0, 0, 255)
      end
      if Keyboard.trigger?(Key::C)
        Graphics.capture.save('capture.png', Image::FORMAT_PNG)
      end
    end

  end



  Graphics.create(800, 600, "OpenRPG #{VERSION}") 
  Graphics.background = Colors.cornflower_blue

  App.on_file_drop do |files|
    files.each { |f| p f }
  end


  Game.start(TestScene)

end