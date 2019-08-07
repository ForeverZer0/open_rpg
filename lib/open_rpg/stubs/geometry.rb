module OpenRPG

  ##
  # Describes a location in Euclidean space.
  class Point

    ##
    # @return [Integer] the location on the horizontal x-axis.
    attr_accessor :x

    ##
    # @return [Integer] the location on the vertical y-axis.
    attr_accessor :y
  end

  ##
  # Describes the dimensions of an 2D object in Euclidean space.
  class Size

    ##
    # @return [Integer] the dimension on the horizontal x-axis.
    attr_accessor :width

    ##
    # @return [Integer] the dimension on the vertical y-axis.
    attr_accessor :height

  end

  ##
  # Represents a two-dimensional shape with four sides at 90° offsets.
  class Rect
  
    ##
    # @return [Integer] the location of the top-left corner on the horizontal x-axis.
    attr_accessor :x

    ##
    # @return [Integer] the location of the top-left corner on the vertical y-axis.
    attr_accessor :y

    ##
    # @return [Integer] the dimension on the horizontal x-axis.
    attr_accessor :width

    ##
    # @return [Integer] the dimension on the vertical y-axis.
    attr_accessor :height

    ##
    # @return [Point] the location of top-left corner of the rectangle.
    attr_accessor :location

    ##
    # @return [Size] the dimensions of the rectangle.
    attr_accessor :size
  end
end 