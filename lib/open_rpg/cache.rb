
module OpenRPG

  module Cache

    IMAGE_FORMATS = %w(.png .jpg .jpeg .bmp .gif .tga .psd)

    @cache = {}

    def self.map(method_name, directory)
      dir = File.expand_path(File.join(Dir.pwd, directory))
      define_singleton_method(method_name) do |name|
        p [name, dir]
        # self.image(name, dir)
      end
    end

    def self.image(path, directory)
      full = File.join(directory, path)
      key = full.to_sym
      return @cache[key] if @cache.has_key?(key)
      IMAGE_FORMATS.each do |ext|
        filename = full + ext
        next unless File.exist?(filename)
        return (@cache[key] = Image.load(filename))
      end
      raise Errno::ENOENT, path
    end

    def self.clear
      @cache.clear
    end

    def self.character(name)


    end

  end
end