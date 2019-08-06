module OpenRPG

  ##
  # Describes the padding between objects, an offset of units (typically pixels) in each cardinal direction.
  class Padding

    ##
    # @return [Integer] the number of units of padding on the left side.
    attr_accessor :left

    ##
    # @return [Integer] the number of units of padding on the right side.
    attr_accessor :right

    ##
    # @return [Integer] the number of units of padding on the top side.
    attr_accessor :top

    ##
    # @return [Integer] the number of units of padding on the bottom side.
    attr_accessor :bottom

    ##
    # @overload initialize
    #   Default padding with all values at `0`.
    #
    # @overload initialize(padding)
    #   @param padding [Integer] The uniform padding to apply to all sides.
    #
    # @overload initialize(horizontal, vertical)
    #   @param horizontal [Integer] The padding to apply to the left and right sides.
    #   @param vertical [Integer] The padding to apply to the top and bottom sides.
    #
    # @overload initialize(left, right, top, bottom)
    #   @param left [Integer] The amount of padding on the left side.
    #   @param right [Integer] The amount of padding on the right side.
    #   @param top [Integer] The amount of padding on the top side.
    #   @param bottom [Integer] The amount of padding on the bottom side.
    def initialize(*args)
      case args.size
      when 0
        set(0, 0, 0, 0)
      when 1
        v = args.first
        set(v, v, v, v)
      when 2
        lr = args.first
        tb = args.last
        set(lr, lr, tb, tb)
      when 4
        set(*args)
      else
        raise ArgumentError, "wrong number of arguments (given #{args.size}, expected 0, 1, 2, 4)"
      end
    end

    ##
    # Sets the padding on both the left and right sides.
    #
    # @param value [Integer] The padding value.
    #
    # @return [self]
    def set_horizontal(value)
      @left = value
      @right = value
      self
    end

    ##
    # Sets the padding on both the top and bottom sides.
    #
    # @param value [Integer] The padding value.
    #
    # @return [self]
    def set_vertical(value)
      @top = value
      @bottom = value
      self
    end

    ##
    # Sets all values of the {Padding} at once.
    # 
    # @param left [Integer] The amount of padding on the left side.
    # @param right [Integer] The amount of padding on the right side.
    # @param top [Integer] The amount of padding on the top side.
    # @param bottom [Integer] The amount of padding on the bottom side.
    #
    # @return [self]
    def set(left, right, top, bottom)
      @left = left
      @right = right
      @top = top
      @bottom = bottom
      self
    end

    ##
    # Determines if this {Padding} is equal to another object.
    #
    # @param other [Object] The object to compare for equality.
    #
    # @return [Boolean] `true` if objects are equal, otherwise `false`.
    def ==(other)
      return false unless other.is_a?(self.class)
      @top == other.top && @bottom == other.bottm && @left == other.left && @right == other.right
    end
  end
end