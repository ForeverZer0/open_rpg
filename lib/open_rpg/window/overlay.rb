
module OpenRPG
  
  class Window < Viewport

    ##
    # The source rectangle of the windowskin used to draw the background overlay.
    #
    # By default, the background source image is tiled to fill the window bounds.
    class Overlay < Layer

      ##
      # The source rectangle of the windowskin used to draw the background overlay.
      SRC_OVERLAY = Rect.new(0, 64, 64, 64)

      ##
      # (see Layer#initialize)
      def initialize(window)
        super(window)
        self.x = 2
        self.y = 2
        self.image = Image.new(@parent.width - 4, @parent.height - 4)
      end

      ##
      # (see Layer#draw)
      def draw
        super
        return unless self.image
        self.image.clear
        return unless @parent.windowskin
        x = 0
        while x < image.width 
          y = 0
          while y < image.height
            self.image.blit(@parent.windowskin, SRC_OVERLAY, Rect.new(x, y, 64, 64))
            y += 64
          end
          x += 64
        end
      end
    end
  end
end