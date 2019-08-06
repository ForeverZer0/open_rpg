module OpenRPG

  class Window < Viewport

    ##
    # Default frame layer for a window.
    class Frame < Layer

      ##
      # Source rectangle of the windowskin of the frame's top-left corner.
      SRC_TL = Rect.new(64, 0, 16, 16)

      ##
      # Source rectangle of the windowskin of the frame's top-right corner.
      SRC_TR = Rect.new(112, 0, 16, 16)

      ##
      # Source rectangle of the windowskin of the frame's bottom-left corner.
      SRC_BL = Rect.new(64, 48, 16, 16)

      ##
      # Source rectangle of the windowskin of the frame's bottom-right corner.
      SRC_BR = Rect.new(112, 48, 16, 16)

      ##
      # Source rectangle of the windowskin of the frame's left side.
      SRC_L = Rect.new(64, 16, 16, 32)

      ##
      # Source rectangle of the windowskin of the frame's right side.
      SRC_R = Rect.new(112, 16, 16, 32)

      ##
      # Source rectangle of the windowskin of the frame's top side.
      SRC_T = Rect.new(80, 0, 32, 16)

      ##
      # Source rectangle of the windowskin of the frame's bottom side.
      SRC_B = Rect.new(80, 48, 32, 16)

      ##
      # (see Layer#initialize)
      def initialize(window)
        super(window)
        self.image = Image.new(window.width, window.height)
      end

      ##
      # (see Layer#draw)
      def draw
        super
        return unless self.image
        self.image.clear
        return unless @parent.windowskin
        # Corners
        self.image.blit(@parent.windowskin, SRC_TL, Rect.new(0, 0, 16, 16))
        self.image.blit(@parent.windowskin, SRC_TR, Rect.new(self.image.width - 16, 0, 16, 16))
        self.image.blit(@parent.windowskin, SRC_BL, Rect.new(0, self.image.height - 16, 16, 16))
        self.image.blit(@parent.windowskin, SRC_BR, Rect.new(self.image.width - 16, self.image.height - 16, 16, 16))
        # Sides
        self.image.blit(@parent.windowskin, SRC_L, Rect.new(0, 16, 16, self.image.height - 32))
        self.image.blit(@parent.windowskin, SRC_R, Rect.new(self.image.width - 16, 16, 16, self.image.height - 32))
        self.image.blit(@parent.windowskin, SRC_T, Rect.new(16, 0, self.image.width - 32, 16))
        self.image.blit(@parent.windowskin, SRC_B, Rect.new(16, self.image.height - 16, self.image.width - 32, 16))
      end
    end
  end
end