
module OpenRPG

  class Window < Viewport

    ##
    # Default background layer for a window.
    #
    # By default, the background source image is stretched to fill the window bounds.
    class Background < Layer

      ##
      # (see Layer#initialize)
      def initialize(window)
        super(window)
        self.x = 4
        self.y = 4
        self.image = Image.new(@parent.width - 8, @parent.height - 8)
      end

      ##
      # @return [Rect] the source rectangle of the windowskin used to draw the background.
      def skin_rect
        @skin_rect ||= Rect.new(0, 0, 64, 64)
      end

      ##
      # (see Layer#draw)
      def draw
        super
        return unless self.image
        self.image.clear
        return unless @parent.windowskin
        self.image.blit(@parent.windowskin, skin_rect, self.image.rect)
      end
    end
  end
end