

module OpenRPG

  ##
  # Represents a font that can be rendered onto an {Image}.
  #
  # Internally, OpenRPG uses the [FreeType](https://www.freetype.org/freetype2/docs/) library for loading fonts and generating glyphs. 
  # FreeType supports many popular font formats, including the most common TrueType (.ttf) and OpenType (.otf) formats.
  #
  # Due to neither Windows, Linux, or macOSX offering a consistent and predictable way of locating installed fonts, or even
  # the location of the base font directories, OpenRPG does not use fonts installed on the system by default. Font loading
  # is done by supplying a path to a supported file, and any fonts the game uses will likely need distributed with it, or the
  # paths will need to be resolved at runtime manually.
  class Font

    ##
    # @return [String] the name of the font's family.
    attr_reader :name

    ##
    # @return [Integer] the size, in pixels, of the font.
    # @note Must be greater than 0.
    attr_accessor :size

    ##
    # @return [Color] the color used to render the font.
    attr_accessor :color

    ##
    # @return [Boolean] `true` if font is a bold style, otherwise `false`.
    attr_reader :bold

    ##
    # @return [Boolean] `true` if font is an italic style, otherwise `false`.
    attr_reader :italic

    ##
    # @return [Size] the size, in pixels, required to rendered the font.
    def measure(string); end

    ##
    # Retrieves the {Glyph} of this font used to render the specified codepoint. 
    # 
    # @param codepoint [Integer] A Unicode codepoint.
    #
    # @return [Glyph?] the glyph for the codepoint, or `nil` if none exists.
    def glyph(codepoint); end

    ##
    # @overload each_glyph(string)
    #   When called wihout a block, returns an enumerator for each glyph used to render the string.
    #   @param string [String] The string to retrieve glyphs for.
    #   @return [Enumerator]
    #
    # @overload each_glyph(string, &block)
    #   When called with a block, yields each glyph used to render the string to the block before returning self.
    #   @param string [String] The string to retrieve glyphs for.
    #   @yield [glyph] Yields a glyph to the block.
    #   @yieldparam glyph [Glyph] The glyph for the current character in the string.
    #   @return [self]
    #
    # @note Only UTF-8 strings are accepted (Ruby's default encoding since 2.0).
    def each_glyph(string, &block); end

    class << self

      ##
      # @return [Font] the default font, which will be used as the prototype of newly created fonts and {Image} objects without a font specified.
      attr_accessor :default

      ##
      # Loads a font from the specified path.
      #
      # @param path [String] The path to a font file. Supports all font types supported by FreeType (i.e. .ttf, otf, etc)
      # @param size [Integer] The size, in pixels, for this font. If `nil`, the size of the {Font.default} will be used.
      # @param options [Hash] Options hash.
      # @option options [Color] :color The color used to render the font. If not specified, the color of the {Font.default} font is used.
      def from_file(path, size = nil, **options); end

      ##
      # @api private
      # @note Must be called prior to {Graphics.destroy}.
      #
      # Frees up all resources outside the scope of the Ruby garbage collector, unloading the FreeType 
      # library and all cached fonts and glyph textures.
      #
      # @return [void]
      def finalize; end
    end

    ##
    # Contains basic information and metrics of an individual character of a font and size.
    # 
    # See [FreeType's documentation](https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html) on how these metrics are used.
    class Glyph

      ##
      # @return [Integer] the UTF-8 codepoint for the character this glyph represents.
      attr_reader :codepoint

      ##
      # @return [Size] the dimensions of the glyph, in pixels, required to render it.
      attr_reader :size

      ##
      # @return [Point] the horizontal and vertical offset of the glyph.
      attr_reader :bearing

      ##
      # @note This is a generic value without advanced kerning techniques.
      # @return [Integer] the number of pixels to advance from this pixel to draw the next glyph.
      attr_reader :advance

      ##
      #
      # @return [Integer] the ID of the glyph's texture used by OpenGL.
      attr_reader :texture

    end
  end
end