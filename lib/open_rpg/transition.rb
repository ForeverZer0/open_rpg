
module OpenRPG

  module Transition

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

    def self.circle(frames = 40, **opts, &block)
      # Create shader unless cached
      unless @cache[:circle]
        path = File.join(BASE_DIRECTORY, 'assets', 'transitions', 'circle.glsl')
        @cache[:circle] = Shader.new(VERT, FRAG % File.read(path))
      end
      # Perform transition
      Graphics.transition(@cache[:circle], frames) do |shader|
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
  
    def self.angular(frames, **opts, &block)
      # Create shader unless cached
      unless @cache[:angular]
        path = File.join(BASE_DIRECTORY, 'assets', 'transitions', 'angular.glsl')
        @cache[:angular] = Shader.new(VERT, FRAG % File.read(path))
      end
      Graphics.transition(@cache[:angular], frames) do |shader|
        location = shader.locate("startingAngle")
        shader.uniformi(location, Integer(opts[:angle] || 90))
        block.call
      end
    end




  end
end