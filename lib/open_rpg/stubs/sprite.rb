module OpenRPG

  ##
  # A container for {Bitmap} objects that determines how they are rendered onto the screen.
  class Sprite < Renderable

    attr_accessor :x

    attr_accessor :y

    attr_accessor :scale

    attr_accessor :scale_x

    attr_accessor :scale_y

    attr_accessor :angle

    attr_accessor :anchor

    attr_accessor :bitmap

    def initialize; end
  end
end