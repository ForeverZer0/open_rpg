module OpenRPG


  ##
  # A container for rendering sprites within a specific region of the screen, with its own clipping area and blending.
  #
  # Internally the {Viewport} is its own framebuffer and container that is rendered off-screen in its own batch before
  # being displayed to the user. Any child sprite's location on the z-axis is only relative to other sprites that share
  # the viewport, and the viewport's z-coordinate is used for determining draw order among sprites without a viewport.
  class Viewport < Renderable

    ##
    # @return [Rect] the bounds of the viewport.
    attr_reader :rect

    ##
    # @return [Size] the location of the top-left corner of the {Viewport}.
    attr_reader :location

    ##
    # @return [Size] the dimensions the {Viewport}, in pixels.
    attr_reader :size

  end
end