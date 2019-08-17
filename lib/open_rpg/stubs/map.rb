
module OpenRPG
  module Map

    def self.clear_cache; end

    def self.cache_tileset(path); end

    def self.cache_template(path); end

    class Tilemap

      ##
      # Loads a Tiled tilemap file (*.tmx).
      #
      # @param path [String] The path to the file to load.
      #
      # @return [Tilemap] the loaded tilemap.
      #
      # @note The tilemap is cached internally for subsequent loads.
      def self.from_file(path); end

      ##
      # @return [Integer] the width of the map, in pixels.
      def width; end

      ##
      # @return [Integer] the height of the map, in pixels.
      def height; end

      ##
      # @return [Size] the size of the map, in pixels.
      def size; end

      ##
      # @return [Integer] the width of the tiles in the map, in pixels.
      def tile_width; end

      ##
      # @return [Integer] the height of the tiles in the map, in pixels.
      def tile_height; end

      ##
      # @return [Integer] the size of the tiles in the map, in pixels.
      def tile_size; end

      def back_color; end

      def orientation; end

      def stagger_axis; end

      def stagger_index; end

      def render_order; end

      def tile_count; end

      def side_length; end

      def tilesets; end

      def layers; end

      def each_tileset; end

      def each_layer; end

      def properties; end

      def property(key); end

      def [](index); end

      alias_method :tile, :[]
    end

    class Tileset
      def tile_count; end
      def offset_x; end
      def offset_y; end
      def offset; end
      def tile_width; end
      def tile_height; end
      def tile_size; end
      def spacing; end
      def margin; end
      def name; end
      def inspect; end

      def image
        def properties; end
        def property(key); end
        def [](index); end

        alias_method :tile, :[]
      end
    end

    class Layer
      def inspect; end
      def name; end
      def alpha; end
      def opacity; end
      def visible?; end
      def offset_x; end
      def offset_y; end
      def offset; end
      def type; end
      def contents; end
      def next_layer; end
      def properties; end
      def property(key); end
    end

    class Tile
      def tileset; end
      def inspect; end
      def id; end
      def x; end
      def y; end
      def animated?; end
      def frame_count; end
      def frames; end
      def each_frame; end
      def image; end
      def type; end
      def collision; end
      def properties; end
      def property(key); end

      class Frame
        def tile_id; end
        def duration; end
        def inspect; end
      end
    end

    class ImageInfo
      def source; end
      def width; end
      def height; end
      def transparent_color; end
      def image; end
    end

    class Group
      include Enumerable

      def color; end
      def draw_order; end
      def objects; end
      def each; end
      def [](index); end

      alias_method :object, :[]
    end

    class MapObject
      def id; end
      def x; end
      def y; end
      def width; end
      def height; end
      def visible?; end
      def rotation; end
      def type; end
      def template; end
      def gid; end
      def properties; end
      def property(key); end
    end

    class Text < MapObject
      def wrap?; end
      def bold?; end
      def italic?; end
      def underline?; end
      def strikeout?; end
      def kerning?; end
      def value; end
      def font_family; end
      def size; end
      def align_h; end
      def align_v; end
      def color; end
    end

    class Shape < MapObject
      def kind; end
      def length; end
      def each_point; end
      def points; end

      alias_method :size, :length
    end

    class Template
      def tileset; end
      def object; end
    end
  end
end
