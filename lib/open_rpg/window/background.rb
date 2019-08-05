
module OpenRPG

  class Window < Viewport

    ##
    # Default background layer for a window.
    #
    # By default, the background source image is stretched to fill the window bounds.
    class Background < Layer

      ##
      # The source rectangle of the windowskin used to draw the background.
      SRC_BACKGROUND = Rect.new(0, 0, 64, 64)

      ##
      # (see Layer#initialize)
      def initialize(window)
        super(window)
        self.x = 4
        self.y = 4
        self.image = Image.new(@parent.width - 8, @parent.height - 8)
      end

      ##
      # (see Layer#draw)
      def draw
        super
        return unless self.image
        self.image.clear
        return unless @parent.windowskin
        self.image.blit(@parent.windowskin, SRC_BACKGROUND, self.image.rect)
      end
    end
  end
end