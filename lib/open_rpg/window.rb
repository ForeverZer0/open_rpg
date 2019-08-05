module OpenRPG

  class Window < Viewport

    class Layer < Sprite

      def initialize(window)
        raise TypeError "expected Window (given #{window.class}" unless window.is_a?(Window)
        super(window)
        @window = window
      end

      def refresh
        # Placeholder for override
      end

      def dispose
        @window = Qnil
        self.image.dispose if self.image
      end
    end

    class BackgroundLayer < Layer

      SRC_RECT = Rect.new(0, 0, 64, 64)

      def initialize(window)
        super(window)
        self.x = 2
        self.y = 2
        self.image = Image.new(@window.width - 4, @window.height - 4)
      end

      def refresh
        super
        return unless self.image
        self.image.clear
        return unless @window.windowskin
        self.image.blit(@window.windowskin, SRC_RECT, self.image.rect)
      end
    end

    class FrameLayer < Layer

      SRC_TL = Rect.new(64, 0, 16, 16)
      SRC_TR = Rect.new(112, 0, 16, 16)
      SRC_BL = Rect.new(64, 48, 16, 16)
      SRC_BR = Rect.new(112, 48, 16, 16)

      SRC_L = Rect.new(64, 16, 16, 32)
      SRC_R = Rect.new(112, 16, 16, 32)
      SRC_T = Rect.new(80, 0, 32, 16)
      SRC_B = Rect.new(80, 48, 32, 16)

      def initialize(window)
        super(window)
        self.image = Image.new(window.width, window.height)
        self.z = 100
      end

      def refresh
        super
        return unless self.image
        self.image.clear
        return unless @window.windowskin
        # Corners
        self.image.blit(@window.windowskin, SRC_TL, Rect.new(0, 0, 16, 16))
        self.image.blit(@window.windowskin, SRC_TR, Rect.new(self.image.width - 16, 0, 16, 16))
        self.image.blit(@window.windowskin, SRC_BL, Rect.new(0, self.image.height - 16, 16, 16))
        self.image.blit(@window.windowskin, SRC_BR, Rect.new(self.image.width - 16, self.image.height - 16, 16, 16))
        # Sides
        # TODO: Stretch/Tile option

        self.image.blit(@window.windowskin, SRC_L, Rect.new(0, 16, 16, self.image.height - 32))
        self.image.blit(@window.windowskin, SRC_R, Rect.new(self.image.width - 16, 16, 16, self.image.height - 32))
        self.image.blit(@window.windowskin, SRC_T, Rect.new(16, 0, self.image.width - 32, 16))
        self.image.blit(@window.windowskin, SRC_B, Rect.new(16, self.image.height - 16, self.image.width - 32, 16))
      end

    end

    attr_accessor :windowskin

    def initialize(*args)
      super(*args)
      @layers = []
      create_layers
      refresh
    end

    def windowskin=(image)
      @windowskin = image
      refresh
    end

    def refresh
      @background.refresh
      @frame.refresh
    end

    def create_layers
     @background = BackgroundLayer.new(self)
     @frame = FrameLayer.new(self)
    end

    def draw_layers

    end
  end
end