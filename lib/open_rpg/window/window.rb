require_relative 'layer'
require_relative 'background'
require_relative 'overlay'
require_relative 'body'
require_relative 'arrows'
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
      @arrow_offset = 4

      @layers = [
        (@background = create_background),
        (@overlay = create_overlay),
        (@body = create_body),
        (@arrows = create_arrows),
        (@frame = create_frame),
        (@selector = create_selector),
        (@prompt = create_prompt)
      ].compact
      
      draw
    end

 
    def draw_text(*args)
      @body.image.draw_text(*args)
    end

    def windowskin=(image)
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
      @layers.each { |layer| layer.dispose if layer }
      @layers.clear
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
      @layers.each { |layer| layer.draw if layer }
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

    ##
    # @return [Layer?] a newly created layer used for the scroll indicators, or `nil` if window should have no frame.
    def create_arrows
      Window::Arrows.new(self, @arrow_offset)
    end

    def create_selector

    end

    def create_prompt

    end
  end
end