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

      @sprite = Sprite.new(@viewport, image: img)
      @sprite.src_rect = Rect.new(0, 0, 32, 48)
      @sprite.x = 64
      @sprite.y = 64

      fog = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Fogs/001-Fog01.png')
      @plane = Plane.new(nil, image: Image.new(128, 128, Colors.red))
      @plane.alpha = 0.35
 
    end

    def close
      @viewport.dispose
      @sprite.dispose
      @window.dispose
    end

    def update
      @window.update
      # @sprite.update

      if Input.press?(:O)
        if Input.press?(:UP)
          @plane.oy -= 8.0
        elsif Input.press?(:DOWN)
          @plane.oy += 8.0
        end
        if Input.press?(:LEFT)
          @plane.ox -= 8.0
        elsif Input.press?(:RIGHT)
          @plane.ox += 8.0
        end
        return
      end


      # Change Hue
      if Input.press?(:HUE)
        if Input.trigger?(:UP) || Input.repeat?(:UP)
          @sprite.hue += 6.0
        elsif Input.trigger?(:DOWN) || Input.repeat?(:DOWN)
          @sprite.hue -= 6.0
        end
        return
      end

      # Change Z
      if Input.press?(:Z)
        if Input.trigger?(:UP) || Input.repeat?(:UP)
          @sprite.z += 1
        elsif Input.trigger?(:DOWN) || Input.repeat?(:DOWN)
          @sprite.z -= 1
        end
        return
      end

      if Input.press?(:UP)
        @window.y -= 8
      end
      if Input.press?(:DOWN)
        @window.y += 8
      end
      if Input.press?(:LEFT)
        @window.x -= 8
      end
      if Input.press?(:RIGHT)
        @window.x += 8
      end


      if Keyboard.trigger?(Key::F)
        GC.start
        yellow = Colors.yellow
        yellow.a = 0.25
        @window.flash(yellow, 8)
      end
      if Keyboard.trigger?(Key::G)
        @window.tone = Tone.new(0, 0, 0, 255)
      end


    end

  end

  Graphics.create(640, 480, "OpenRPG #{VERSION}") 
  Graphics.background = Colors.cornflower_blue

  Input.bind(:LEFT, [Input::Key::LEFT, Input::Key::A], nil)
  Input.bind(:RIGHT, [Input::Key::RIGHT, Input::Key::D], nil)
  Input.bind(:UP, [Input::Key::UP, Input::Key::W], nil)
  Input.bind(:DOWN, [Input::Key::DOWN, Input::Key::S], nil)
 
  Input.bind(:HUE, [Input::Key::H], nil)
  Input.bind(:Z, [Input::Key::Z], nil)
  Input.bind(:O, [Input::Key::O], nil)


  App.client_size = Size.new(800, 600)
  Game.start(TestScene)

end