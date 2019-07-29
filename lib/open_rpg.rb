require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/colors'
require_relative 'open_rpg/shader'

module OpenRPG

  class TestGame < Game

    include Input

    def update
      if $TEST
        $TEST.update
      end

      if Keyboard.press?(Key::UP)
        $TEST.y -= 8
      end
      if Keyboard.press?(Key::DOWN)
        $TEST.y += 8
      end
      if Keyboard.press?(Key::LEFT)
        $TEST.x -= 8
      end
      if Keyboard.press?(Key::RIGHT)
        $TEST.x += 8
      end
      if Keyboard.trigger?(Key::F)
        $TEST.flash(Colors.green, 8)
      end
      if Keyboard.trigger?(Key::G)
        $TEST.tone = Tone.new(0, 0, 0, 255)
        p $TEST.tone
      end
    end

  end


  TestGame.new(800, 600, "OpenRPG #{VERSION}") do |game|

    image = Image.new('/home/eric/Pictures/character.png')

    image.draw_text(0, 0, image.width, image.height, "Hello World!")


    $TEST = Sprite.new(image)
    $TEST.z = 8000
    
    game.bg_color = Colors.cornflower_blue
    game.main

 
    # font.dispose

    $TEST.dispose(true)
    $TEST = nil
    

  end
end


