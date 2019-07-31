module OpenRPG

  ##
  # Describes a basic two-dimensional graphics object for displaying on the screen via a {Sprite}.
  #
  # Internally this class contains an OpenGL texture stored on the GPU.
  class Bitmap

    ALIGN_TOP_LEFT = 0
    ALIGN_TOP_RIGHT = 1
    ALIGN_TOP_CENTER = 2
    ALIGN_BOTTOM_LEFT = 3
    ALIGN_BOTTOM_RIGHT = 4
    ALIGN_BOTTOM_CENTER = 5
    ALIGN_CENTER_LEFT = 6
    ALIGN_CENTER_RIGHT = 7
    ALIGN_CENTER = 8

    ##
    # Indicates that {Bitmap} should be saved in PNG format.
    FORMAT_PNG = 0

    ##
    # Indicates that {Bitmap} should be saved in JPEG format.
    FORMAT_JPG = 1

    ##
    # Indicates that {Bitmap} should be saved in bitmap format.
    FORMAT_BMP = 2

    ##
    # Indicates each pixel is in RGBA byte order.
    TYPE_RGBA = 0x1908

    ##
    # Indicates each pixel is in RGB byte order, without alpha component.
    TYPE_RGB  = 0x1907

    ##
    # Indicates each pixel is in BGRA byte order.
    TYPE_BGRA = 0x80E1

    ##
    # Indicates each pixel is in BGR byte order, without alpha component.
    TYPE_BGR  = 0x80E0

    ##
    # @return [Integer] the width on the x-axis, in pixels, of the bitmap.
    def width; end

    ##
    # @return [Integer] the height on the y-axis, in pixels, of the bitmap.
    def height; end

    alias_method :columns, :width
    alias_method :rows, :height

    def initialize; end

    ##
    # Creates a new bitmap by copying a sub-section of this {Bitmap}.
    #
    # @overload slice(x, y, width, height)
    #
    # @overload slice(point, size)
    #
    # @overload slice(rect)
    #
    def slice; end

    ##
    #
    # @overload blit(src, src_rect, dst_rect, alpha = 1.0)
    #
    # @overload blit(src, src_rect, x, y, alpha = 1.0)
    #
    def blit; end

    def save(filepath, format = FORMAT_PNG); end

  end
end