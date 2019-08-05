module OpenRPG

  class Window < Viewport

    class Body < Layer

      def initialize(window, padding)
        super(window)
        self.z = 100
        @ox = 0
        @oy = 0
        @padding = padding
        self.x = padding
        self.y = padding
        @width = window.width - (padding * 2)
        @height = window.height - (padding * 2)
      end

      def image=(value)
        super(value)
        if value
          @ox = [[value.width, @ox].min, 0].max
          @oy = [[value.height, @oy].min, 0].max
          @width = [[value.width, @parent.width - (@padding * 2)].min, 0].max
          @height = [[value.height, @parent.height - (@padding * 2)].min, 0].max
          self.src_rect = Rect.new(@ox, @oy, @width, @height)
        end
      end

      def ox=(ox)
        @ox = ox
        src_rect = Rect.new(ox, @oy, @width, @height)
      end

      def oy=(oy)
        @oy = oy
        src_rect = Rect.new(@ox, oy, @width, @height)
      end
   
      def draw
        super
        image;clear if image
      end
    end
  end
end