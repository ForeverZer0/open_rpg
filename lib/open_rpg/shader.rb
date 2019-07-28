
module OpenRPG

  class Shader

    def self.from_file(vertex, fragment, geometry = nil)
      vert_src = IO.read(vertex)
      frag_src = IO.read(fragment)
      geo_src = geometry ? IO.read(geometry) : nil
      return new(vert_src, frag_src, geo_src)
    end
  end
end