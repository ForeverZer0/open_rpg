
module OpenRPG

  ##
  # Provides a 
  module Cache

    ##
    # File extensions for supported image formats.
    IMAGE_FORMATS = %w(.png .jpg .jpeg .bmp .gif .tga .psd .hdr)

    @cache = {}

    ##
    # Defines a method to search a specific directory for images. Once mapped,
    # the method can be used to search and load image files from that directory by
    # providing only the base filename, without directory or (optional) file extension.
    #
    # @example
    #   # Example Directory Tree
    #   # |-(working directory)
    #   # |   |-Assets
    #   # |   |   |-Characters
    #   # |   |   |   |-hero.png
    #
    #   Cache.map(:character, "./Assets/Characters")
    #   hero_image = Cache.character('hero')
    #
    # @param method_name [Symbol|String] The name of the method to create.
    # @param directory [String] The relative or absolute path to the directory the created
    #   method will search for files.
    #
    # @return [void]
    def self.map_directory(method_name, directory)
      dir = File.absolute_path(directory)
      define_singleton_method(method_name) do |name|
        self.load_image(name, dir)
      end
    end

    ##
    # @api private
    #
    # Loads an image from a file, caching it so once loaded once, it remains in memory for subsequent usage.
    #
    # @param filename [String] The name of the file, with or without file extension. If no extension is provided,
    #   a file matching with any of the supported extensions will be searched.
    # @param directory [String] A relative or absolute path to the directory where the image is located.
    #
    # @return [Image] The loaded image.
    #
    # @raise [Errno::ENOENT] Thrown when no suitable file can be found.
    # @raise [RPGError] Thrown image loading the fails, such as unsupported file format.
    def self.load_image(filename, directory)
      full = File.join(directory, filename)
      key = full.to_sym
      return @cache[key] if @cache.has_key?(key)
      return (@cache[key] = Image.from_file(full)) if File.exist?(full)
      IMAGE_FORMATS.each do |ext|
        path = full + ext
        next unless File.exist?(path)
        return (@cache[key] = Image.from_file(path))
      end
      raise Errno::ENOENT, path
    end

    ##
    # Disposes and clears all cached images.
    #
    # @return [void]
    def self.clear
      @cache.each_value { |img| img.dispose }
      @cache.clear
    end
  end
end