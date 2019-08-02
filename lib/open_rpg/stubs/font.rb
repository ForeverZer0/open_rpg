
module OpenRPG

  class Font

    class << self

      attr_accessor :default

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