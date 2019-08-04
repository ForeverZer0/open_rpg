module OpenRPG

  ##
  # A container for {Image} objects that determines how they are rendered onto the screen.
  class Sprite < Renderable

    attr_accessor :x

    attr_accessor :y

    attr_accessor :image

    attr_reader :viewport

    def initialize; end
  end
end