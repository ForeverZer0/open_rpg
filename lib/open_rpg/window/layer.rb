module OpenRPG
  
  class Window < Viewport

    ##
    # @abstract This class offers no functionality without being overriden.
    #
    # Base class for sprites used to create a {Window}.
    class Layer < Sprite

      ##
      # Retrieves the initial {#z} value to set for a layer, which determines teh order
      # in which layers are drawn.
      #
      # @param layer [Layer] An instance of a {Layer}.
      #
      # @return [Integer] the 
      def self.layer_z(layer)
        return case layer
        when Window::Face then 500       # Sub-class of Background, so muct come before in switch
        when Window::FaceFrame then 600  # Sub-class of Frame, so muct come before in switch
        when Window::Background then 0
        when Window::Overlay then 100
        when Window::Body then 200
        when Window::Frame then 300
        when Window::Selector then 400
        when Window::Arrows then 700
        when Window::Prompt then 800
        else 0
        end
      end

      ##
      # Creates a new instance of the class.
      #
      # @param window [Window] A {Window} instance. Cannot be `nil`.
      def initialize(window)
        raise TypeError "expected Window (given #{window.class}" unless window.is_a?(Window)
        super(window)
        @parent = window
        self.z = Layer.layer_z(self)
      end

      ##
      # Redraws the window layer.
      #
      # @return [void]
      def draw
        # Placeholder for override
      end

      ##
      # @note Unlike a normal sprite, the layers's {#image} is also disposed, therefore it is important not use a cached/shared
      #   {Image} for this property without overriding the {#dispose} method to alter this behavior.
      #
      # Disposes the sprite **and its image**. 
      #
      # @return [void]
      def dispose
        super
        @parent = nil
        self.image.dispose if self.image
      end
    end
  end
end