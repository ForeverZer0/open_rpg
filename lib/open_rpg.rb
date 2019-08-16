require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'
require_relative 'open_rpg/movable'

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

      path = '/home/eric/Music/flora cash - I Wasted You (Audio)-0-kennkvJLE.mp3'
      sound = Sound.from_file path
      ms = sound.duration
      p sound
      Audio.play(sound)
      sound.dispose

      p ms
      p ms / 1000
      p (ms / 1000) / 60

      t = Map::Tilemap.from_file('/home/eric/Desktop/sample/island.tmx')

    
      @viewport = Viewport.new(0, 0, 640, 480)
      @viewport.z = 10
      path = '/home/eric/Pictures/RTP/XP/Graphics/Characters/001-Fighter01.png'
      @sprite = SpriteSheet.new(@viewport, Image.from_file(path), 32, 48)


      # @window = Window.new(0, 480 - 192, 640, 192)
      # @window.windowskin = Image.from_file('/home/eric/Pictures/Window.png')
      # @window.contents = Image.new(@window.width - 24, @window.height - 24)
      # (0..7).each do |i|
      #   @window.draw_text(0, i * 24, @window.contents.width, 24, "Line #{i}")
      # end
      # @window.z = 9000

 
      App.set_icon('/home/eric/Pictures/arc-icon.png')


 
      path = '/home/eric/Pictures/screen.png'
      @background = Sprite.new(@viewport, image: Image.from_file(path))

      fog = Image.from_file('/home/eric/Pictures/RTP/XP/Graphics/Fogs/001-Fog01.png')
      @plane = Plane.new(nil, image: fog) 
      @plane.z = 100
      @plane.alpha = 0.4
      @plane.zoom_x = 0.5
      @plane.zoom_y = 0.5
 
    end

    def close
      @sprite.dispose(true) if @sprite
      @background.dispose(true) if @background
      @window.dispose if @window
      @plane.dispose(true) if @plane
      @viewport.dispose if @viewport
    end

    def update
      @sprite.update if @sprite
      @window.update if @window

      if Input.trigger?(:CONFIRM)
        


      end


      cell = @sprite.selected

      if Input.press?(:UP)
        @sprite.y -= 4
        @sprite.select((cell.x - 1) % 4, 3) if Game.frame_count % 4 == 0
      elsif Input.press?(:DOWN)
        @sprite.y += 4
        @sprite.select((cell.x + 1) % 4, 0) if Game.frame_count % 4 == 0
      end
      if Input.press?(:LEFT)
        @sprite.x -= 4
        @sprite.select((cell.x - 1) % 4, 1) if Game.frame_count % 4 == 0
      elsif Input.press?(:RIGHT)
        @sprite.x += 4
        @sprite.select((cell.x + 1) % 4, 2) if Game.frame_count % 4 == 0
      end





      if Input::Keyboard.trigger?(Key::T)

        Transition.random(60) { Game.goto(TestScene2) }

        return
      end

      if Input::Keyboard.trigger?(Input::Key::M)
        @window.move(32, 32, 8)



      end

      if @plane
        @plane.oy += 1
        @plane.ox += 2
      end

    end

  end


  #################################################################
  # Example Main
  #################################################################

  # Create graphics with 640x480 internal resolution
  OpenRPG.create(640, 480, "OpenRPG #{VERSION}") 

  # Set window size to 800x600
  App.client_size = Size.new(800, 600)

  # Bind input keys
  Input.bind(:UP, [Input::Key::UP, Input::Key::W], nil)
  Input.bind(:LEFT, [Input::Key::LEFT, Input::Key::A], nil)
  Input.bind(:DOWN, [Input::Key::DOWN, Input::Key::S], nil)
  Input.bind(:RIGHT, [Input::Key::RIGHT, Input::Key::D], nil)

  Input.bind(:CONFIRM, [Input::Key::SPACE], [Input::Mouse::LEFT])
  Input.bind(:CANCEL, [Input::Key::LEFT_CONTROL], [Input::Mouse::RIGHT])


  # Start the first scene, entering main game loop
  Game.start(TestScene)

end