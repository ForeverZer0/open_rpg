module OpenRPG

  ##
  # A container for rendering sprites within a specific region of the screen, with its own clipping area and blending.
  #
  # Internally the {Viewport} is its own framebuffer and container that is rendered off-screen in its own batch before
  # being displayed to the user. Any child sprite's location on the z-axis is only relative to other sprites that share
  # the same viewport, and the viewport's z-coordinate is used for determining draw order among free objects not within a viewport.
  class Viewport < Renderable

    ##
    # @return [Rect] the bounds of the viewport.
    attr_reader :rect

    ##
    # @return [Size] the dimensions the {Viewport}, in pixels.
    attr_reader :size

    ##
    # @return [Integer] the horizontal dimension of the {Viewport}.
    attr_reader :width

    ##
    # @return [Integer] the vertical dimension of the {Viewport}.
    attr_reader :height
  end
end