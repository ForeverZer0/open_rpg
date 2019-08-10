
module OpenRPG

  module Transition

    attr_reader :shader

    VERT = <<-EOS
    attribute vec2 _p;
    varying vec2 _uv;

    void main() {
      gl_Position = vec4(_p, 0.0, 1.0);
      _uv = vec2(0.5, 0.5) * (_p + vec2(1.0, 1.0));
    }
    EOS

    FRAG = <<-EOS
    // precision highp float;
    varying vec2 _uv;

    uniform sampler2D from, to;
    uniform float progress, ratio, _fromR, _toR;

    vec4 getFromColor(vec2 uv) {
      return texture2D(from, %s);
    }
    
    vec4 getToColor(vec2 uv) {
      return texture2D(to, %s);
    }

    %s

    void main(){
      gl_FragColor = transition(_uv);
    }
    EOS

    def self.resize_mode(uniform, mode = :stretch)

      return case mode
      when :cover then "0.5 + (uv - 0.5) * vec2(min(ratio / #{uniform}, 1.0), min(#{uniform} / ratio, 1.0))"
      when :contain then "0.5 + (uv - 0.5) * vec2(max(ratio / #{uniform}, 1.0), max(#{uniform} / ratio, 1.0))"
      else "uv"
      end
    end

    def self.create_shader(file)

      resize = %w(cover contain stretch)
      r = 'stretch'

      frag = File.read(file)

      from_r = "uv"
      to_r = "uv"

      File.open('test.glsl', 'wb') { |io| io.write format(FRAG, from_r, to_r, frag) }

      Shader.new(VERT, format(FRAG, from_r, to_r, frag))

    end

    ##
    # @!method initialize(shader, frames, &block)
    #

  end
end