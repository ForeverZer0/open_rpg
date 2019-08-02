

module OpenRPG

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

    end


    class Glyph

      attr_reader :codepoint

      attr_reader :size

      attr_reader :bearing

      attr_reader :advance

      attr_reader :texture

    end
  end

end