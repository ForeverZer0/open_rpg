module OpenRPG

  class Window < Viewport

    class Body < Layer

      def initialize(window, padding)
        super(window)
        @ox = 0
        @oy = 0
        @padding = padding
        self.x = padding
        self.y = padding
        self.z = Layer::BODY_Z
        @width = window.width - (padding * 2)
        @height = window.height - (padding * 2)
      end

      ##
      # (see Sprite#image=)
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

      ##
      # @return [Integer] bitwise combination of flags representing the 4 cardinal directions that the arrows should be display at.
      # @see Direction
      # @see Arrows#direction
      def scrollable_directions
        dir = Direction::NONE
        if @width < image.width
          if @ox > 0
            dir |= Direction::LEFT
          elsif @ox + @width < image.width
            dir |= Direction::RIGHT
          end
        end
        if @height < image.height
          if @oy > 0
            dir |= Direction::TOP
          elsif @oy + @height < image.height
            dir |= Direction::BOTTOM
          end
        end
        dir
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