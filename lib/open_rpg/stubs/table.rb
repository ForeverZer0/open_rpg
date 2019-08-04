module OpenRPG

  ##
  # A multi-dimensional array class implementing in C for storing a large number of integers.
  # Internally, each value is a 16-bit signed integer in the range of `-32768` to `32767`.
  class Table

    include Enumerable

    ##
    # @return [Integer] the number of elements on the x-axis.
    attr_reader :width

    ##
    # @return [Integer] the number of elements on the y-axis, or `0` if table has only one dimension.
    attr_reader :height

    ##
    # @return [Integer] the number of elements on the z-axis, or `0` if table has less than 3 dimensions.
    attr_reader :depth

    ##
    # @return [Integer] the number of dimensions in this {Table}.
    attr_reader :dimensions

    ##
    # @return [Integer] the total number of elements in this {Table}.
    attr_reader :length

    alias_method :xsize, :width
    alias_method :ysize, :height
    alias_method :zsize, :depth
    alias_method :size, :length

    ##
    # @overload initialize(width)
    #   Creates a new instance of a {Table} with 1 dimension.
    #   @param width [Integer] The size of the table on the x-axis.
    #
    # @overload initialize(width, height)
    #   Creates a new instance of a {Table} with 2 dimensions.
    #   @param width [Integer] The size of the table on the x-axis.
    #   @param height [Integer] The size of the table on the y-axis. Must be greater than `0` if specified.
    #
    # @overload initialize(width, height, depth)
    #   Creates a new instance of a {Table} with 3 dimensions.
    #   @param width [Integer] The size of the table on the x-axis.
    #   @param height [Integer] The size of the table on the y-axis. Must be greater than `0` if specified.
    #   @param depth [Integer] The size of the table on the z-axis. Must be greater than `0` if specified.
    #
    def initialize(*args); end

    ##
    # Enumerates through each value in the {Table}.
    # @overload each(&block)
    #   When called with a block, yields each value to the block before returning self. Values are yielded
    #   in order along the x-axis, then the y-axis, and then finally the z-axis.
    #   @yield [value] Yields a value to the block.
    #   @yieldparam value [Integer] The current value, an integer in the range of `-32768` to `32767`.
    #   @return [self]
    #
    # @overload each
    #   When called without a block, returns an Enumerator object for the {Table}.
    #   @return [Enumerator]
    #
    def each(&block); end

    ##
    # Retrieves a value from the {Table}.
    #
    # @overload [](x)
    #   @param x [Integer] The location of the element on the x-axis.
    # 
    # @overload [](x, y)
    #   @param x [Integer] The location of the element on the x-axis.
    #   @param y [Integer] The location of the element on the y-axis.
    #
    # @overload [](x, y, z)
    #   @param x [Integer] The location of the element on the x-axis.
    #   @param y [Integer] The location of the element on the y-axis.
    #   @param z [Integer] The location of the element on the z-axis.
    #
    # @raise [ArgumentError] Thrown when the number of indices provided is not equal to the dimensions of the {Table}.
    # @raise [RangeError] Thrown when an index is outside the bounds of the specified dimension.
    #
    # @return [Integer] the value at the specified location.
    def [](*args); end

    ##
    # Sets a value within the {Table}. Internally the values of the table are 16-bit signed integers, in
    # the range of `-32768` to `32767`.
    #
    # @overload []=(x, value)
    #   @param x [Integer] The location of the element on the x-axis.
    #   @param value [Integer] The value to set, a value between `-32768` and `32767`.
    # 
    # @overload []=(x, y, value)
    #   @param x [Integer] The location of the element on the x-axis.
    #   @param y [Integer] The location of the element on the y-axis.
    #   @param value [Integer] The value to set, a value between `-32768` and `32767`.
    #
    # @overload []=(x, y, z, value)
    #   @param x [Integer] The location of the element on the x-axis.
    #   @param y [Integer] The location of the element on the y-axis.
    #   @param z [Integer] The location of the element on the z-axis.
    #   @param value [Integer] The value to set, a value between `-32768` and `32767`.
    #
    # @raise [ArgumentError] Thrown when the number of indices provided is not equal to the dimensions of the {Table}.
    # @raise [RangeError] Thrown when an index is outside the bounds of the specified dimension.
    #
    # @return [Integer] the value at the specified location.
    def []=(*args); end

    ##
    # Allows changing the size of the table, including adding or removing dimensions. When a dimension is added, all
    # values on the new axis will be initialized as `0`. If dimensions are less, values outside of the new range will be truncated.
    #
    # @overload resize(width)
    #   @param width [Integer] The size of the table on the x-axis.
    #
    # @overload resize(width, height)
    #   @param width [Integer] The size of the table on the x-axis. 
    #   @param height [Integer] The size of the table on the y-axis. Must be greater than `0` if specified.
    #
    # @overload resize(width, height, depth)
    #   @param width [Integer] The size of the table on the x-axis.
    #   @param height [Integer] The size of the table on the y-axis. Must be greater than `0` if specified.
    #   @param depth [Integer] The size of the table on the z-axis. Must be greater than `0` if specified.
    #
    # @return [self]
    def resize(*args); end

    ##
    # Creates a deep-copy of this {Table}.
    #
    # @return [Table] a newly created {Table} equal to this instance.
    def dup; end

    ##
    # Clears all values in the {Table}, setting them all to `0`.
    #
    # @return [void]
    def clear; end
  end
end