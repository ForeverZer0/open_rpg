module OpenRPG

  ##
  # Module containing values to represent alignment, such as for text-rendering, etc.
  module Align
    NONE = 0x00
    TOP = 0x01
    CENTER_V = 0x02
    BOTTOM = 0x04
    LEFT = 0x08
    CENTER_H = 0x10
    RIGHT = 0x20
    TOP_LEFT = TOP | LEFT
    TOP_RIGHT = TOP | RIGHT
    TOP_CENTER = TOP | CENTER_H
    BOTTOM_LEFT = BOTTOM | LEFT
    BOTTOM_RIGHT = BOTTOM | RIGHT
    BOTTOM_CENTER = BOTTOM | CENTER_H
    CENTER_LEFT = CENTER_V | LEFT
    CENTER_RIGHT = CENTER_V | RIGHT
    CENTER = CENTER_V | CENTER_H
  end

  ##
  # Describes a basic two-dimensional graphics object for displaying on the screen via a {Sprite}.
  #
  # Internally this class contains an OpenGL texture stored on the GPU.
  class Image
    ##
    # Indicates that {Image} should be saved in PNG format.
    FORMAT_PNG = 0

    ##
    # Indicates that {Image} should be saved in JPEG format.
    FORMAT_JPG = 1

    ##
    # Indicates that {Image} should be saved in image format.
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
    # @note This value can be used with third-party OpenGL bindings.
    # @return [Integer] the internal value used by OpenGL for the framebuffer object used by this {Image}.
    attr_reader :fbo

    ##
    # @note This value can be used with third-party OpenGL bindings.
    # @return [Integer] the internal value used by OpenGL for the texture this {Image} represents.
    attr_reader :texture

    ##
    # @return [Integer] the width on the x-axis, in pixels, of the image.
    attr_reader :width

    ##
    # @return [Integer] the height on the y-axis, in pixels, of the image.
    attr_reader :height

    alias_method :columns, :width
    alias_method :rows, :height

    ##
    # Creates a new instance of the {Image} class.
    #
    # @param width [Integer] The horizontal size on the x-axis, in pixels.
    # @param height [Integer] The vertical size on the y-axis, in pixels.
    # @param color [Color] If speciifed, will initialize the image filled with a color. `nil` specifies a blank image with no color.
    #
    # @see load
    def initialize(width, height, color = nil); end

    ##
    # Creats a image from an  image stored in a file. 
    # 
    # OpenRPG supports the following formats out-of-the box:
    # <ul>
    # <li><strong>JPG</strong> (baseline & progressive, 12 bpc/arithmetic not supported, same as stock IJG lib)</li>
    # <li><strong>PNG</strong> (1/2/4/8/16-bit-per-channel)</li>
    # <li><strong>TGA</strong></li>
    # <li><strong>BMP</strong> (non-1bpp, non-RLE)</li>
    # <li><strong>PSD</strong> (composited view only, no extra channels, 8/16 bit-per-channel)</li>
    # <li><strong>GIF</strong></li>
    # <li><strong>HDR</strong> (radiance rgbE format)</li>
    # <li><strong>PIC</strong> (Softimage PIC)</li>
    # <li><strong>PNM</strong> (PPM and PGM binary only)</li>
    # </ul>
    #
    # @param filepath [String] The path to the image to be loaded.
    #
    # @return [Image]
    def self.load(filepath); end

    ##
    # Creates an image arbitrary binary data, useful if you using an alternate image loader (i.e. rmagick)
    #
    # @param width [Integer] The width of the image. (greater than `0`)
    # @param height [Integer] The height of the image. (greater than `0`)
    # @param blob [String] The binary data contained in a string. The stride of the data must be consistent with the format specified.
    # @param format {Integer} The supported formats of the pixel layout. Valid values include:
    # <ul>
    # <li>{TYPE_RGBA} (32-bit, red, green, blue, alpha order)</li>
    # <li>{TYPE_RGB} (24-bit, red, green, blue order, without and alpha component)</li>
    # <li>{TYPE_BGRA} (32-bit, blue, green, red, alpha order)</li>
    # <li>{TYPE_BGR} (24-bit, blue, green, red order, without and alpha component)</li>
    # </ul>
    #
    # @return [Image]
    def self.from_blob(width, height, blob, format = TYPE_RGBA); end

    ##
    # Creates a new image from a sub-section of this {Image}.
    #
    # @overload slice(x, y, width, height)
    #   @param x [Intger] The location of the top-left corner of the area to copy on the x-axis, in pixels.
    #   @param y [Intger] The location of the top-left corner of the area to copy on the y-axis, in pixels.
    #   @param width [Intger] The width of the area to copy, in pixels. (greater than `0`)
    #   @param height [Intger] The height of the area to copy, in pixels. (greater than `0`)
    #
    # @overload slice(point, size)
    #   @param point [Point] The location of the top-left corner of the area to copy, in pixels.
    #   @param size [Size] The area of the area to copy.
    #
    # @overload slice(rect)
    #   @param rect [Rect] The bounds of the area to copy.
    #
    # @raise [ArgumentError] Thrown when the specified dimensions fall outside the image bounds. 
    # 
    # @return [Image]
    def slice; end

    # TODO: Update!
    # blit (src, src_rect, dst_rect, alpha = 1.0)              3,4
    # blit (src, src_rect, dst_pnt, alpha = 1.0)               3,4
    # blit (src, src_rect, x, y, alpha = 1.0)                  4,5
    # blit (src, src_rect, x, y, width, height, alpha = 1.0)   6,7

    ##
    # Renders the specified image onto this image.
    #
    # @overload blit(src, dst_rect, alpha = 1.0)
    #   @param src [Image] The source image that will be drawn onto this image.
    #   @param dst_rect [Rect] The rectangle, relative to the this image, that will the source image will be drawn to.
    #   @param alpha [Integer|Float] The opacity to draw the source image at.
    #
    # @overload blit(src, x, y, alpha = 1.0)
    #   @param src [Image] The source image that will be drawn onto this image.
    #   @param x [Intger] The top-left corner on the x-axis where the source image will be copied to.
    #   @param y [Intger] The top-left corner on the y-axis where the source image will be copied to.
    #   @param alpha [Integer|Float] The opacity to draw the source image at.
    #
    # @overload blit(src, x, y, width, height, alpha = 1.0)
    #   @param src [Image] The source image that will be drawn onto this image.
    #   @param x [Intger] The top-left corner on the x-axis where the source image will be copied to.
    #   @param y [Intger] The top-left corner on the y-axis where the source image will be copied to.
    #   @param width [Intger] The width of the area to draw to. (greater than `0`)
    #   @param height [Intger] The height of the area to draw to. (greater than `0`)
    #   @param alpha [Integer|Float] The opacity to draw the source image at.
    # 
    # @note If the destination size is not equal to the size of the source image, it will be stretched/shrinked accordinly to fill the area,
    # @note For the alpha value, integer values will be assumed to be in the range of a `0` to `255`, while float values will be interpreted in range of `0.0` to `1.0`.
    # @return [self]
    def blit; end

    ##
    # Saves the image to a file in the specified format.
    # 
    # @param filepath [String] The path to the file where the image will be saved.
    # @param format [Integer] The image format to save the file as. Valid values include:
    #   <ul>
    #     <li>{FORMAT_PNG}</li>
    #     <li>{FORMAT_JPG}</li>
    #     <li>{FORMAT_BMP}</li>
    #   </ul>
    # @return [void]
    def save(filepath, format = FORMAT_PNG); end


  end
end