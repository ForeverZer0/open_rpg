require_relative 'layer'
require_relative 'background'
require_relative 'overlay'
require_relative 'body'
require_relative 'frame'

module OpenRPG

  class Window < Viewport

    attr_accessor :windowskin

    attr_reader :background
    attr_reader :overlay
    attr_reader :frame
    attr_reader :body

    attr_accessor :active

    def initialize(*args)
      super(*args)

      @active = false
      @padding = 12

      @background = create_background
      @overlay = create_overlay
      @body = create_body
      @frame = create_frame
      draw
    end

 
    def draw_text(*args)
      @body.image.draw_text(*args)
    end

    def windowskin=(image)
      p image
      @windowskin = image
      draw
    end

    def contents
      @body.image
    end

    def contents=(image)
      @body.image = image
    end

    def dispose
      super
      @background.dispose if @background
      @overlay.dispose if @overlay
      @body.dispose if @body
      @frame.dispose if @frame
    end

    def inspect
      "<#{self.class.name.split('::').last}: x:#{x} y:#{y} width:#{width} height:#{height}>"
    end

    protected

    ##
    # Forces all the window layers to redraw themselves. This must be called when the certain propertys of 
    # the window such as size of windowskin are changed.
    #
    # @return [void]
    def draw
      @background.draw if @background
      @overlay.draw if @overlay
      @body.draw if @body
      @frame.draw if @frame
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
      Window::Body.new(self, @padding)
    end

    ##
    # @return [Layer?] a newly created layer used for the window frame, or `nil` if window should have no frame.
    def create_frame
      Window::Frame.new(self)
    end
  end
end