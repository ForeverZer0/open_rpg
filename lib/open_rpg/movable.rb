
module OpenRPG

  ##
  # Mixin for classes that can be represented in on-screen coordinates, and are
  # capable of being moved. 
  #
  # Implementors must define `x` and `y` attributes with read/write access.
  module Movable

    ##
    # @!attribute [rw] x
    #   @return [Integer] the location of the entity on the horizontal x-axis.

    ##
    # @!attribute [rw] y
    #   @return [Integer] the location of the entity on the vertical y-axis.

    ##
    # @!attribute [rw] location
    #   @return [Point] the location of the entity.

    ##
    # Sets the location of the entity.
    #
    # @param x [Integer] the location of the entity on the x-axis.
    # @param y [Integer] the location of the entity on the y-axis.
    #
    # @return [self]
    def move(x, y)
      self.x = Integer(x)
      self.y = Integer(y)
      self
    end

    def location
      Point.new(self.x, self.y)
    end

    def location=(point)
      self.x = point.x
      self.y = point.y
      point
    end
  end

  class Sprite < Renderable

    include Movable
  end

  class Viewport < Renderable

    include Movable
  end
end