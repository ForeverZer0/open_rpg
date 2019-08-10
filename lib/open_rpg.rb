require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/input'
require_relative 'open_rpg/padding'
require_relative 'open_rpg/colors'
require_relative 'open_rpg/scene'
require_relative 'open_rpg/game'
require_relative 'open_rpg/window/window'
require_relative 'open_rpg/cache'
require_relative 'open_rpg/transition'

##
# Top-level namepsace for the OpenRPG API.
#
# @author Eric \"ForeverZer0\" Freed
module OpenRPG
  
  class TestScene2 < Scene

    def initialize
      @sprite = Sprite.new(image: Image.new(640, 480, Colors.red))
    end

    def close
      @sprite.dispose(false)
    end
  end


  ##
  # @!parse [ruby]
  #   # The absolute path to the directory of the base installation, set dynamically during compilation.
  #   # @return [String]
  #   BASE_DIRECTORY = nil

  class TestScene < Scene

    include Input

    def initialize

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
      @plane = Plane.new(nil, image: fog)
      @plane.alpha = 0.35
      @plane.zoom_x = 0.5
      @plane.zoom_y = 0.5
 
    end

    def close
      
      @sprite.dispose(true)
     
      @window.dispose
      @plane.dispose

      @viewport.dispose
    end

    def update
      @window.update
      # @sprite.update

      if Input::Keyboard.trigger?(Key::T)

        Game.goto(TestScene2)
        return


        file = "./assets/transitions/circle.glsl"
        $shader = Transition.create_shader(file) 
        Transition.execute($shader, 8) do 
          Game.goto(TestScene2)
        end
  
  
        return
      end



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

      if Input.press?(:UP)
        @window.y -= 8
      elsif Input.press?(:DOWN)
        @window.y += 8
      end

      if Input.press?(:LEFT)
        @window.x -= 8
      elsif Input.press?(:RIGHT)
        @window.x += 8
      end
    end

  end

  #################################################################
  # Example Main
  #################################################################

  # Create graphics with 640x480 internal resolution
  Graphics.create(640, 480, "OpenRPG #{VERSION}") 
  Graphics.background = Colors.cornflower_blue
  # Set window size to 800x600
  App.client_size = Size.new(800, 600)

  # Bind input keys
  Input.bind(:LEFT, [Input::Key::LEFT, Input::Key::A], nil)
  Input.bind(:RIGHT, [Input::Key::RIGHT, Input::Key::D], nil)
  Input.bind(:UP, [Input::Key::UP, Input::Key::W], nil)
  Input.bind(:DOWN, [Input::Key::DOWN, Input::Key::S], nil)
 
  # Start the first scene, entering main game loop
  Game.start(TestScene)

end