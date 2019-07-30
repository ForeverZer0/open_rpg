require_relative 'open_rpg/version'
require_relative 'open_rpg/open_rpg'

require_relative 'open_rpg/colors'


module OpenRPG

  # class TestGame < Game

  #   include Input

  #   def update
  #     if $TEST
  #       $TEST.update
  #     end

  #     if Keyboard.press?(Key::UP)
  #       $TEST.y -= 8
  #     end
  #     if Keyboard.press?(Key::DOWN)
  #       $TEST.y += 8
  #     end
  #     if Keyboard.press?(Key::LEFT)
  #       $TEST.x -= 8
  #     end
  #     if Keyboard.press?(Key::RIGHT)
  #       $TEST.x += 8
  #     end
  #     if Keyboard.trigger?(Key::F)
  #       $TEST.flash(Colors.green, 8)
  #     end
  #     if Keyboard.trigger?(Key::G)
  #       $TEST.tone = Tone.new(0, 0, 0, 255)
  #       p $TEST.tone
  #     end
  #   end

  # end


  Graphics.create(800, 600, "OpenRPG #{VERSION}") do |gfx|
    gfx.bg_color = Colors.cornflower_blue


    bmp = Bitmap.new('/home/eric/Pictures/character.png')
    sprite = Sprite.new(nil, bmp)


    p sprite.visible
    p sprite.alpha
    
    gfx.main

    p 'bye'
    sprite.dispose true


  end

  

  # Graphics.main
  # Graphics.dispose


#   TestGame.new(800, 600, "OpenRPG #{VERSION}") do |game|
#     $TEST = Sprite.new(nil)

#     image = 
#     red  = Bitmap.new(64, 64, Colors.red)
    
#     image.blt(0, 0, red, Rect.new(0, 0, 32, 64))

# $TEST.bitmap = image
    
#     $TEST.z = 8000
#     game.bg_color = Colors.cornflower_blue
#     game.main

 
    # font.dispose

    # $TEST.dispose(true)
    # $TEST = nil
    

  # end
end


