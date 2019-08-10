require_relative 'layer'
require_relative 'background'
require_relative 'overlay'
require_relative 'body'
require_relative 'arrows'
require_relative 'frame'
require_relative 'face'
require_relative 'face_frame'

module OpenRPG

  ##
  # @abstract This class is not intended to be used directly, and should be sub-classed.
  #
  # Base class for all game windows. Unlike RPG Maker windows, OpenRPG uses a "layer" approach, where each layer
  # is specialized child sprite responsible for displaying a specific aspect of the window. Each sprite can be
  # overridden and/or altered to provide any non-standard behavior that is desired with ease, or add/remove
  # parts of the window.
  #
  # While this modular approach [single responsibility principle](https://en.wikipedia.org/wiki/Single_responsibility_principle)
  # implementation may seem somewhat foreign to those coming from the RPG Maker series, it offers a drastic increase in
  # extensiblility, and the capability to easily customize behavior with minimal effort, while offering the same core
  # functionality and interface.
  class Window < Viewport

    ##
    # @return [Boolean] value indicating if window is active, used to control cursor blink, etc.
    attr_accessor :active

    ##
    # @return [Background?] the background layer of the window, or `nil` if none has been defined.
    attr_reader :background

    ##
    # @return [Overlay?] the overlay layer of the window, or `nil` if none has been defined.
    attr_reader :overlay

    ##
    # @return [Frame?] the frame layer of the window, or `nil` if none has been defined.
    attr_reader :frame

    ##
    # @return [Body?] the contents layer of the window, or `nil` if none has been defined.
    attr_reader :body

    ##
    # Creates a new instance of the {Window} class. 
    #
    # @overload initialize
    #   Creates a new {Window} that fills the screen.
    #
    # @overload initialize(rect)
    #   Creates a new {Window} that fills the specified rect.
    #   @param rect [Rect] A rectangle specifying the location and size of the window.
    #
    # @overload initialize(locatiom, size)
    #   Creates a new {Window} at the specified location and size.
    #   @param location [Point] The location of the top-left corner of the window.
    #   @param size [Size] The size, in pixels, of the window.
    #
    # @overload initialize(x, y, width, height)
    #   Creates a new {Window} with the specified dimensions.
    #   @param x [Integer] The location of the top-left corner of the window on the x-axis.
    #   @param y [Integer] The location of the top-left corner of the window on the y-axis.
    #   @param width [Integer] The size of the window on the x-axis, in pixels.
    #   @param height [Integer] The size of the window on the y-axis, in pixels.
    #
    def initialize(*args)
      # TODO: Check size, padding, etc.
      super(*args)
      @active = false
      @arrow_offset = 4

      @layers = create_layers.compact
      draw
    end

    ##
    # Convenience method for drawing text to the image of the {Body} layer.
    #
    # @return [void]
    #
    # @see Image#draw_text
    def draw_text(*args)
      @body.image.draw_text(*args)
    end

    ##
    # Clears the image of the {Body} layer if it exists, otherwise does nothing.
    #
    # @return [void]
    def clear
      if @body && @body.image
        @body.image.clear
      end
    end

    ##
    # @return [String] the {Window} as a human readaoble string.
    def inspect
      "<#{self.class.name.split('::').last}: x:#{x} y:#{y} width:#{width} height:#{height}>"
    end

    ##
    # Updates the window and all of its layers.
    # This provides update to flash effects, or whatever behavior that may be defined by each layer.
    #
    # @return [self]
    def update
      super
      @layers.each { |layer| layer.update } 
      self
    end

    ##
    # Disposes the window and all of its layers, free resources outside the scope of the Ruby garbage collector.
    #
    # @return [void]
    def dispose
      @layers.each { |layer| layer.dispose(true) }
      @layers.clear
      super
    end

    ##
    # @!attribute [rw] windowskin
    #   @return [Image?] the source image used for creating the windows graphics, or `nil` to not use.

    ##
    # @!attribute [rw] contents
    #   @return [Image?] the image used for drawing the window contents, a shortcut to the {Body} layer image.

    ##
    # @!attribute [rw] padding
    #   @return [OpenRPG::Padding] the padding to be applied between the window's edge and its contents on each cardinal direction.

    def contents
      @body.image
    end

    def contents=(image)
      @body.image = image
    end

    attr_reader :windowskin

    def windowskin=(image)
      @windowskin = image
      draw
    end

    def padding
      @padding ||= Padding.new(12, 12, 12, 12)
    end

    def padding=(padding)
      @padding = padding
    end

    protected

    # @group Protected Methods (override in sub-classes)

    ##
    # Creates the window layers, returning them win an array.
    #
    # By default, only the background, overlay, body, and frame are created.
    #
    # @return [Array<Layer>] the array of layers used to supply automatic refreshing and disposing.
    def create_layers
      return [
        (@background = create_background),
        (@overlay = create_overlay),
        (@body = create_body),
        (@frame = create_frame),
      ]
    end

    ##
    # Forces all the window layers to redraw themselves. This must be called when certain propertys of 
    # the window, such as the size or windowskin, are changed, as well as when the {Window} is first created.
    #
    # @return [void]
    def draw
      @layers.compact!
      @layers.each { |layer| layer.draw }
    end

    ##
    # @return [Layer?] a newly created layer used for the window background, or `nil` if window should have no background.
    def create_background
      Window::Background.new(self)
    end

    ##
    # @return [Layer?] a newly created layer used for the window overlay, or `nil` if window should have no overlay.
    def create_overlay
      Window::Overlay.new(self)
    end

    ##
    # @return [Layer?] a newly created layer used for the window contents, or `nil` if window should have no contents.
    def create_body
      Window::Body.new(self, padding)
    end

    ##
    # @return [Layer?] a newly created layer used for the window frame, or `nil` if window should have no frame.
    def create_frame
      Window::Frame.new(self)
    end

    ##
    # @return [Layer?] a newly created layer used for the scroll indicators, or `nil` if window should not use scroll arrows.
    def create_arrows
      Window::Arrows.new(self, @arrow_offset)
    end

    ##
    # @return [Layer?] a newly created layer used for the selector, or `nil` if window should have not use a selector.
    def create_selector
      Window::Selector.new(self)
    end

    ##
    # @return [Layer?] a newly created layer used for the input prompt indicator, or `nil` if window should not use an input prompt.
    def create_prompt
      Window::Prompt.new(self)
    end

    # @endgroup
  end
end