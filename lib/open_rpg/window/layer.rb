module OpenRPG
  
  class Window < Viewport

    ##
    # Base class for sprites used to create a {Window}.
    class Layer < Sprite

      ##
      # Creates a new instance of the class.
      #
      # @param window [Window] A {Window} instance. Cannot be `nil`.
      def initialize(window)
        raise TypeError "expected Window (given #{window.class}" unless window.is_a?(Window)
        super(window)
        @parent = window
      end

      ##
      # Redraws the window layer.
      #
      # @return [void]
      def draw
        # Placeholder for override
      end

      ##
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