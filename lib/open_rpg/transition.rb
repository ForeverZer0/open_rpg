
module OpenRPG

  module Transition

    include Numerics

    @cache = {}

    VERT = <<-EOS
      attribute vec2 _p;
      varying vec2 _uv;
  
      void main() {
        gl_Position = vec4(_p, 0.0, 1.0);
        _uv = vec2(0.5, 0.5) * (_p + vec2(1.0, 1.0));
      }
      EOS

    FRAG = <<-EOS
      precision highp float;
      varying vec2 _uv;
  
      uniform sampler2D from, to;
      uniform float progress, ratio, _fromR, _toR;
  
      vec4 getFromColor(vec2 uv) {
        return texture2D(from, uv);
      }
      
      vec4 getToColor(vec2 uv) {
        return texture2D(to, uv);
      }
  
      %s
  
      void main(){
        gl_FragColor = transition(_uv);
      }
      EOS
  
    def self.angular(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('angular.glsl', :angular), frames) do |shader|
        location = shader.locate("startingAngle")
        shader.uniformi(location, Integer(opts[:angle] || 90))
        block.call
      end
    end

    def self.bounce(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('bounce.glsl', :bounce), frames) do |shader|
        
        location = shader.locate('shadow_color')
        shader.uniform_vec4(location, opts[:color] || Vector4.new(0, 0, 0, 0.6))

        location = shader.locate('shadow_height')
        shader.uniformf(location, opts[:height] || 0.075)

        location = shader.locate('bounces')
        shader.uniformf(location, opts[:bounces] || 3.0)

        block.call
      end
    end

    def self.bow_tie_h(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('bow-tie-h.glsl', :bow_tie_h), frames) do |shader|
        block.call
      end
    end

    def self.bow_tie_v(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('bow-tie-v.glsl', :bow_tie_v), frames) do |shader|
        block.call
      end
    end

    def self.burn(frames, color: Color.new(0.9, 0.4, 0.2), &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('burn.glsl', :burn), frames) do |shader|
        location = shader.locate('color')
        shader.uniform_color(location, color)
        block.call
      end
    end

    def self.circle(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('circle.glsl', :circle), frames) do |shader|
        # Set color of effect
        location = shader.locate("backColor")
        shader.uniform_color(location, opts[:color] || Colors.black)
        # Set center point where circle closes to
        location = shader.locate("center")
        if opts[:center]
          center = opts[:center]
          x = Float(center.x) / Graphics.width
          y = Float(Graphics.height - center.y) / Graphics.height
          shader.uniformf(location, x, y)
        else
          shader.uniformf(location, 0.5, 0.5)
        end
        # Set screen ratio
        location = shader.locate("ratio")
        shader.uniformf(location, Float(Graphics.width) / Graphics.height)
        # Call the block to change current screen
        block.call
      end
    end

    # TODO: CircleCrop




    private

    def self.load_shader(filename, key)
      unless @cache[key]
        path = File.join(BASE_DIRECTORY, 'assets', 'transitions', filename)
        @cache[key] = Shader.new(VERT, FRAG % File.read(path))
      end
      @cache[key]
    end
  end
end