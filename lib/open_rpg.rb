require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/input'
require_relative 'open_rpg/padding'
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

      Font.default.size = 18
      @viewport = Viewport.new(32, 32, 400, 300)

      @window = Window.new(0, 480 - 192, 640, 192)
      @window.windowskin = Image.from_file('/home/eric/Pictures/Window.png')
      @window.contents = Image.new(@window.width - 24, @window.height - 24)
      (0..7).each do |i|
        @window.draw_text(0, i * 24, @window.contents.width, 24, "Line #{i}")
      end
      

      
      App.set_icon('/home/eric/Pictures/arc-icon.png')
      img = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Characters/001-Fighter01.png')

      # @sprite = Sprite.new(nil, image: img)
      @sprite = Sprite.new(@viewport, image: img)
      @sprite.src_rect = Rect.new(0, 0, 32, 48)
      @sprite.x = 64
      @sprite.y = 64
      Input.begin_capture
    end

    def close
      # @viewport.dispose
      # @window.dispose(true)
      # @win.dispose
    end

    def update

      if Input.trigger?(:A)
        p Input.end_capture
      end


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




  Graphics.create(640, 480, "OpenRPG #{VERSION}") 
  Graphics.background = Colors.cornflower_blue


  Input.bind(:A, [Input::Key::A, Input::Key::O], [Input::Mouse::RIGHT])
  Input.each_binding do |sym, keys, buttons|
    p [sym, keys, buttons]
  end



  App.client_size = Size.new(800, 600)
  App.on_file_drop do |files|
    files.each { |f| p f }
  end

  Game.start(TestScene)

end