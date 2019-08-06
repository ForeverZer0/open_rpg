module OpenRPG
  
  class Window < Viewport

    ##
    # @abstract This class offers no functionality without being overriden.
    # Base class for sprites used to create a {Window}.
    class Layer < Sprite

      BACKGROUND_Z = 0
      OVERLAY_Z = 100
      BODY_Z = 200
      FAZE_IMAGE_Z = 300
      FACE_FRAME_Z = 400
      SELECTOR_Z = 500
      FRAME_Z = 600
      ARROWS_Z = 700
      PROMPT_Z = 800

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