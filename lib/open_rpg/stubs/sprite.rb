module OpenRPG

  ##
  # A container for {Image} objects that determines how they are rendered onto the screen.
  class Sprite

    include Disposable

    attr_accessor :x

    attr_accessor :y

    attr_accessor :y

    attr_accessor :scale

    attr_accessor :scale_x

    attr_accessor :scale_y

    attr_accessor :angle

    attr_accessor :anchor

    attr_reader :flash_color

    attr_reader :flash_duration

    attr_accessor :image

    attr_accessor :opacity

    attr_accessor :alpha

    attr_accessor :color

    attr_accessor :tone

    def flash(color, duration); end

    def dispose(image = false); end

    def disposed?; end

    def update; end

    def initialize; end
  end
end