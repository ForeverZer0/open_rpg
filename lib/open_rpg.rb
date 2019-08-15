require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'
require_relative 'open_rpg/movable'

require_relative 'open_rpg/input'
require_relative 'open_rpg/padding'
require_relative 'open_rpg/colors'
require_relative 'open_rpg/scene'
require_relative 'open_rpg/game'
require_relative 'open_rpg/window/window'
require_relative 'open_rpg/tilemap/tilemap'
require_relative 'open_rpg/cache'
require_relative 'open_rpg/transition'


##
# Top-level namepsace for the OpenRPG API.
#
# @author Eric \"ForeverZer0\" Freed
module OpenRPG
  
  class TestScene2 < Scene

    def initialize
      Font.default.outline_color = Colors.red
      path = '/home/eric/Pictures/screen.gif'
      @sprite = Sprite.new(image: Image.from_file(path))
    end

    def close
      @sprite.dispose(true) if @sprite
    end

    def update
      if Input::Keyboard.trigger?(Input::Key::T)

  
        Transition.random(60) { Game.goto(TestScene) }
      
  
  
        return
      end
    end
  end


  ##
  # @!parse [ruby]
  #   # The absolute path to the base installation directory, set dynamically during compilation.
  #   # @return [String]
  #   BASE_DIRECTORY = nil

  class TestScene < Scene


    include Input

    def initialize

      t = Tiled::Tilemap.new('/home/eric/Desktop/sample/island.tmx')

      # p t.size
      # p t.tile_size
      # p t.back_color
      # p t.stagger_index
      # p t.stagger_axis
      # p t.orientation
      # p t.render_order
      # p t.tile_count
      # p t.side_length

      # p t.each_layer.map { |a| a.contents }


      @viewport = Viewport.new(0, 0, 640, 480)
      @viewport.z = 10

      @window = Window.new(0, 480 - 192, 640, 192)
      @window.windowskin = Image.from_file('/home/eric/Pictures/Window.png')
      @window.contents = Image.new(@window.width - 24, @window.height - 24)
      (0..7).each do |i|
        @window.draw_text(0, i * 24, @window.contents.width, 24, "Line #{i}")
      end
      @window.z = 9000

 
      App.set_icon('/home/eric/Pictures/arc-icon.png')
      # img = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Characters/001-Fighter01.png')
      # img = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Transitions/020-Flat01.png')

      path = '/home/eric/Pictures/screen.png'
      # path = '/home/eric/Pictures/RTP/XP/Graphics/Transitions/020-Flat01.png'
      @sprite = Sprite.new(@viewport, image: Image.from_file(path))
      # @sprite.alpha = 1

      # @window.alpha = 0.9

      fog = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Fogs/001-Fog01.png')
      @plane = Plane.new(nil, image: fog) 
      @plane.z = 100
      @plane.alpha = 0.75
      @plane.zoom_x = 0.5
      @plane.zoom_y = 0.5
 
    end

    def close
      @sprite.dispose(true) if @sprite
      @window.dispose if @window
      @plane.dispose(true) if @plane
      @viewport.dispose if @viewport
    end

    def update
      # @window.update
      @sprite.update if @sprite
      @window.update if @window

      if Input::Keyboard.trigger?(Key::T)

        Transition.random(60) { Game.goto(TestScene2) }

        return
      end

      if Input::Keyboard.trigger?(Input::Key::M)
        @window.move(32, 32, 8)



      end

      if Input::Keyboard.press?(Input::Key::O)
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

      # if Input.press?(:UP)
      #   @window.y -= 8
      # elsif Input.press?(:DOWN)
      #   @window.y += 8
      # end

      # if Input.press?(:LEFT)
      #   @window.x -= 8
      # elsif Input.press?(:RIGHT)
      #   @window.x += 8
      # end
    end

  end


  #################################################################
  # Example Main
  #################################################################

  # Create graphics with 640x480 internal resolution
  OpenRPG.create(640, 480, "OpenRPG #{VERSION}") 
  Game.back_color = Colors.green
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