
module OpenRPG
  class Window < Viewport

    class Arrows < Layer

      SRC_LEFT = Rect.new(80, 24, 8, 16)
      SRC_RIGHT = Rect.new(104, 24, 8, 16)
      SRC_TOP = Rect.new(88, 16, 16, 8)
      SRC_BOTTOM = Rect.new(88, 40, 16, 8)

      attr_reader :direction
      attr_reader :offset

      def initialize(window, offset)
        super(window)
        self.visible = false
        @direction = Direction::NONE
        @offset = offset
        self.x = offset
        self.y = offset
        self.z = 300
        self.image = Image.new(window.width - (offset * 2), window.height - (offset * 2))
      end

      def direction=(value)
        if @direction != value
          @direction = value
          draw
        end
        value
      end

      def offset=(value)
        if @offset != value
          @offset = value
          draw
        end
        value
      end

      def draw
        return unless image
        image.clear
        return unless @parent.windowskin && @direction != Direction::NONE
        if (@direction & Direction::LEFT) != 0
          y = (image.height - SRC_LEFT.height) / 2
          image.blit(@parent.windowskin, SRC_LEFT, Rect.new(0, y, SRC_LEFT.width, SRC_LEFT.height))
        end
        if (@direction & Direction::RIGHT) != 0
          x = image.width - SRC_RIGHT.width
          y = (image.height - SRC_RIGHT.height) / 2
          image.blit(@parent.windowskin, SRC_RIGHT, Rect.new(x, y, SRC_RIGHT.width, SRC_RIGHT.height))
        end
        if (@direction & Direction::TOP) != 0
          x = (image.width - SRC_TOP.width) / 2
          image.blit(@parent.windowskin, SRC_TOP, Rect.new(x, 0, SRC_TOP.width, SRC_TOP.height))
        end
        if (@direction & Direction::BOTTOM) != 0
          x = (image.width - SRC_BOTTOM.width) / 2
          y = image.height - SRC_BOTTOM.height
          image.blit(@parent.windowskin, SRC_BOTTOM, Rect.new(x, y, SRC_BOTTOM.width, SRC_BOTTOM.height))
        end
      end
    end
  end
end