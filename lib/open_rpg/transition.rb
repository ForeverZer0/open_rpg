# frozen_string_literal: true

module OpenRPG

  module Transition

    include Numerics

    @cache = {}

    VERT = <<-EOS
      attribute vec2 _p;
      varying vec2 _uv;

      void main() {
          gl_Position = vec4(_p.xy + 0.0, 0.0, 1.0);
          _uv = vec2(0.5, 0.5) * (_p + vec2(1.0, 1.0));
      }
      EOS

    FRAG = <<-EOS
      precision highp float;
      varying vec2 _uv;
  
      uniform sampler2D from, to;
      uniform float progress, ratio;
  
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
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('angular'), frames) do |shader|
        shader.uniformi(shader.locate("startingAngle"), Integer(opts[:angle] || 90))
        block.call
      end
    end

    def self.bounce(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('bounce'), frames) do |shader|
        shader.uniform_vec4(shader.locate('shadow_color'), opts[:color] || Vector4.new(0, 0, 0, 0.6))
        shader.uniformf(shader.locate('shadow_height'), opts[:height] || 0.075)
        shader.uniformf(shader.locate('bounces'), opts[:bounces] || 3.0)
        block.call
      end
    end

    def self.bow_tie_h(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('bow-tie-h'), frames) do |shader|
        block.call
      end
    end

    def self.bow_tie_v(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('bow-tie-v'), frames) do |shader|
        block.call
      end
    end

    def self.burn(frames, color: Color.new(0.9, 0.4, 0.2), &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('burn'), frames) do |shader|
        shader.uniform_color(shader.locate('color'), color)
        block.call
      end
    end

    def self.burnout(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('burnout'), frames) do |shader|
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.03)
        shader.uniform_vec2(shader.locate('center'), translate_coords(opts[:center]))
        c = opts[:color] || Colors.black
        shader.uniformf(shader.locate('color'), c.r, c.g, c.b)
        block.call
      end
    end

    def self.circle_open(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('circle-open'), frames) do |shader|
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.3)
        opening = opts.has_key?(:opening) ? opts[:opening] ? 1 : 0 : 1
        shader.uniformi(shader.locate('opening'), opening)
        block.call
      end
    end

    def self.circle(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('circle'), frames) do |shader|
        shader.uniform_color(shader.locate("backColor"), opts[:color] || Colors.black)
        shader.uniform_vec2(shader.locate("center"), translate_coords(opts[:center]))
        shader.uniformf(shader.locate("ratio"), opts[:ration] || (Float(Graphics.width) / Graphics.height))
        block.call
      end
    end

    def self.color_phase(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('color-phase'), frames) do |shader|
        shader.uniform_vec4(shader.locate('fromStep'), opts[:from] || Vector4.new(0.0, 0.2, 0.4, 0.0))
        shader.uniform_vec4(shader.locate('toStep'), opts[:to] || Vector4.new(0.6, 0.8, 1.0, 1.0))
        block.call
      end
    end

    def self.color_distance(frames, power: 0.5, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('color-distance'), frames) do |shader|
        shader.uniformf(shader.locate('power'), power)
        block.call
      end
    end

    def self.crosshatch(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('crosshatch'), frames) do |shader|
        shader.uniform_vec2(shader.locate('center'), translate_coords(opts[:center]))
        shader.uniformf(shader.locate('threshold'), opts[:threshold] || 3.0)
        shader.uniformf(shader.locate('edge'), opts[:edge] || 0.1)
        block.call
      end
    end

    def self.cross_warp(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('cross-warp'), frames) do |shader|
        block.call
      end
    end

    def self.cross_zoom(frames, strength: 0.4, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('cross-zoom'), frames) do |shader|
        shader.uniformf(shader.locate('strength'), strength)
        block.call
      end
    end

    def self.cube(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('cube'), frames) do |shader|
        shader.uniformf(shader.locate('persp'), opts[:perspective] || 0.7)
        shader.uniformf(shader.locate('unzoom'), opts[:unzoom] || 0.3)
        shader.uniformf(shader.locate('reflection'), opts[:reflection] || 0.4)
        shader.uniformf(shader.locate('floating'), opts[:floating] || 3.0)
        block.call
      end
    end

    def self.curtain(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('curtain'), frames) do |shader|
        shader.uniformf(shader.locate('dots'), opts[:dots] || 20.0)
        center = translate_coords(opts[:center] || Vector2.new(0, Graphics.height))
        shader.uniform_vec2(shader.locate('center'), center)
        block.call
      end
    end

    def self.directional_warp(frames, direction: Vector2.new(-1.0, 1.0), &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('directional-warp'), frames) do |shader|
        shader.uniform_vec2(shader.locate('direction'), direction)
        block.call
      end
    end


    def self.directional_wipe(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('directional-wipe'), frames) do |shader|
        shader.uniform_vec2(shader.locate('direction'), opts[:direction] || Vector2.new(1.0, -1.0))
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.5)
        block.call
      end
    end

    def self.directional(frames, direction: Vector2.new(0.0, 1.0), &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('directional'), frames) do |shader|
        shader.uniform_vec2(shader.locate('direction'), direction)
        block.call
      end
    end

    def self.displace(frames, **opts, &block)
      if opts[:map].nil?
        block.call
        return
      end
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('displacement'), frames) do |shader|
        shader.uniformf(shader.locate('strength'), opts[:strength] || 0.5)
        block.call
        shader.bind(shader.locate('map'), opts[:map], 2)
      end
    end

    def self.luma(frames, map: nil, &block)
      if map.nil?
        block.call
        return
      end
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('luma'), frames) do |shader|
        block.call
        shader.bind(shader.locate('luma'), map, 2)
      end
    end

    def self.doom(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('doom'), frames) do |shader| 
        shader.uniformi(shader.locate('bars'), opts[:bar] || 30)
        shader.uniformf(shader.locate('amplitude'), opts[:amplitude] || 2.0)
        shader.uniformf(shader.locate('noise'), opts[:noise] || 0.1)
        shader.uniformf(shader.locate('frequency'), opts[:frequency] || 0.5)
        shader.uniformf(shader.locate('dripScale'), opts[:drip] || 0.5)
        block.call
      end
    end

    def self.doorway(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('doorway'), frames) do |shader|
        shader.uniformf(shader.locate('reflection'), opts[:reflection] || 0.4)
        shader.uniformf(shader.locate('prespective'), opts[:perspective] || 0.4)
        shader.uniformf(shader.locate('depth'), opts[:depth] || 3.0)
        block.call
      end
    end

    def self.dreamy(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('dreamy'), frames) do |shader|
        block.call
      end
    end

    def self.dreamy_zoom(frames, **opts, &block)
      # TODO: Remove this? Kinda below-average quality...
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('dreamy-zoom'), frames) do |shader|
        shader.uniformf(shader.locate('rotation'), opts[:rotation] || 6.0)
        shader.uniformf(shader.locate('scale'), opts[:scale] || 1.2)
        block.call
      end
    end

    def self.fade(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('fade'), frames) do |shader|
        block.call
      end
    end

    def self.fade_color(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('fade-color'), frames) do |shader|
        c = opts[:color] || Colors.black
        shader.uniformf(shader.locate('color'), c.r, c.g, c.b)
        shader.uniformf(shader.locate('colorPhase'), opts[:phase] || 0.4)
        block.call
      end
    end

    def self.fade_grayscale(frames, intensity: 0.3, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('fade-grayscale'), frames) do |shader|
        shader.uniformf(shader.locate('intensity'), intensity)
        block.call
      end
    end

    def self.film_burn(frames, seed: 2.31, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('film-burn'), frames) do |shader|
        shader.uniformf(shader.locate('Seed'), seed)
        block.call
      end
    end
  
    def self.flyeye(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('flyeye'), frames) do |shader|
        shader.uniformf(shader.locate('size'), opts[:size] || 0.04)
        shader.uniformf(shader.locate('zoom'), opts[:zoom] || 50.0)
        shader.uniformf(shader.locate('colorSeparation'), opts[:separation] || 0.3)
        block.call
      end
    end

    def self.glitch_displace(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('glitch-displace'), frames) do |shader|
        block.call
      end
    end

    def self.glitch_memories(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('glitch-memories'), frames) do |shader|
        block.call
      end
    end

    def self.grid_flip(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('grid-flip'), frames) do |shader|
        size = opts[:size] || Size.new(4, 4)
        shader.uniformi(shader.locate('size'), size.width, size.height)
        shader.uniformf(shader.locate('pause'), opts[:pause] || 0.1)
        shader.uniformf(shader.locate('dividerWidth'), opts[:divider] || 0.05)
        shader.uniform_color(shader.locate('bgcolor'), opts[:background] || Colors.black)
        shader.uniformf(shader.locate('randomness'), opts[:randomness] || 0.1)
        block.call
      end
    end

    def self.heart(frames, &block) 
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('heart'), frames) do |shader|
        block.call
      end
    end

    def self.hexagons(frames, **opts, &block)
      # FIXME: Not working
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('hexagonalize'), frames) do |shader|
        shader.uniformi(shader.locate('steps'), opts[:steps] || 2)
        shader.uniformf(shader.locate('horizontalHexagons'), opts[:horizontal] || 10.0)
        block.call
      end
    end

    def self.kaleidoscope(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('kaleidoscope'), frames) do |shader| 
        shader.uniformf(shader.locate('speed'), opts[:speed] || 1.0)
        shader.uniformf(shader.locate('angle'), opts[:angle] || 1.0)
        shader.uniformf(shader.locate('power'), opts[:power] || 1.5)
        block.call
      end
    end

    def self.linear_blur(frames, intensity: 0.1, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('kaleidoscope'), frames) do |shader|
        shader.uniformf(shader.locate('intensity'), intensity)
        block.call
      end
    end

    def self.luminance_melt(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('luminance-melt'), frames) do |shader|
        d  = opts.has_key?(:direction) ? opts[:direction] ? 1 : 0 : 1
        shader.uniformi(shader.locate('direction'), d)
        shader.uniformf(shader.locate('threshold'), opts[:threshold] || 0.8)
        shader.uniformi(shader.locate('above'), opts[:above] || 0)
        block.call
      end
    end

    def self.morph(frames, strength: 0.1, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('morph'), frames) do |shader|
        shader.uniformf(shader.locate('strength'), strength)
        block.call
      end
    end

    def self.mosaic(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('mosaic'), frames) do |shader|
        shader.uniformi(shader.locate('endX'), opts[:x] || 2)
        shader.uniformi(shader.locate('endY'), opts[:y] || -1)
        block.call
      end
    end

    def self.multiply_blend(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('multiply-blend'), frames) do |shader|
        block.call
      end
    end

    def self.page_curl(frames, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('page-curl'), frames) do |shader|
        block.call
      end
    end

    def self.parametrics(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('parametrics'), frames) do |shader|
        shader.uniformf(shader.locate('a'), opts[:a] || 4.0)
        shader.uniformf(shader.locate('b'), opts[:b] || 1.0)
        shader.uniformf(shader.locate('amplitude'), opts[:amplitude] || 120.0)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.1)
        block.call
      end
    end

    def self.perlin(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('perlin'), frames) do |shader|
        shader.uniformf(shader.locate('scale'), opts[:scale] || 4.0)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.01)
        shader.uniformf(shader.locate('seed'), opts[:seed] || 12.9898)
        block.call
      end
    end

    def self.pinwheel(frames, speed: 2.0, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('pinwheel'), frames) do |shader|
        shader.uniformf(shader.locate('speed'), speed)
        block.call
      end
    end

    def self.pixelize(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('pixelize'), frames) do |shader|
        x = opts[:x] || 20
        y = opts[:y] || 20
        shader.uniformi(shader.locate('squaresMin'), x, y)
        shader.uniformi(shader.locate('steps'), opts[:steps] || 50)
        block.call
      end
    end

    def self.polar(frames, segments: 5, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('polar'), frames) do |shader|
        shader.uniformi(shader.locate('segments'), segments)
        block.call
      end
    end

    def self.radial(frames, smoothness: 1.0, &block)
      # FIXME: Remove? Same as angular, but no setting for angle
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('radial'), frames) do |shader|
        shader.uniformf(shader.locate('smoothness'), smoothness)
        block.call
      end
    end


    def self.ripple(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('ripple'), frames) do |shader|
        shader.uniformf(shader.locate('amplitude'), opts[:amplitude] || 100.0)
        shader.uniformf(shader.locate('speed'), opts[:speed] || 50.0)
        block.call
      end
    end

    def self.rotate_scale_fade(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('rotate-scale-fade'), frames) do |shader|
        shader.uniform_vec2(shader.locate('center'), translate_coords(opts[:center]))
        shader.uniformf(shader.locate('rotations'), opts[:rotation] || 1.0)
        shader.uniformf(shader.locate('scale'), opts[:scale] || 8.0)
        shader.uniform_color(shader.locate('backColor'), opts[:color] || Color.new(0.15, 0.15, 0.15, 1.0))
        block.call
      end
    end

    def self.simple_zoom(frames, speed: 0.8, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('simple-zoom'), frames) do |shader|
        shader.uniformf(shader.locate('speed'), speed)
        block.call
      end
    end

    def self.squares(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('squares'), frames) do |shader|
        shader.uniformi(shader.locate('size'), opts[:x] || 10, opts[:y] || 10)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.5)
        block.call
      end
    end

    def self.squares_wire(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('squares-wire'), frames) do |shader|
        shader.uniformi(shader.locate('squares'), opts[:x] || 10, opts[:y] || 10)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 1.6)
        d = opts[:direction] || Vector2.new(1.0, -0.5)
        shader.uniformf(shader.locate('direction'), d.x, d.y)
        block.call
      end
    end

    def self.squeeze(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('squeeze'), frames) do |shader|
        shader.uniformf(shader.locate('separation'), opts[:separation] || 0.4)
        block.call
      end
    end

    def self.swap(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('swap'), frames) do |shader|
        shader.uniformf(shader.locate('reflection'), opts[:reflection] || 0.4)
        shader.uniformf(shader.locate('perspective'), opts[:perspective] || 0.2)
        shader.uniformf(shader.locate('depth'), opts[:depth] || 3.0)
        block.call
      end
    end

    def self.swirl(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('swirl'), frames) do |shader|
        block.call
      end
    end

    def self.static(frames, offset: 0.05, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('static'), frames) do |shader|
        shader.uniformf(shader.locate('offset'), offset)
        block.call
      end
    end
  
    def self.waterdrop(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('waterdrop'), frames) do |shader|
        shader.uniformf(shader.locate('amplitude'), opts[:amplitude] || 30.0)
        shader.uniformf(shader.locate('speed'), opts[:speed] || 30.0)
        block.call
      end
    end

    def self.wind(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('wind'), frames) do |shader|
        shader.uniformf(shader.locate('size'), opts[:size] || 0.2)
        block.call
      end
    end

    def self.windowblinds(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('windowblinds'), frames) do |shader|
        block.call
      end
    end

    def self.window_slice(frames, **opts, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      Graphics.transition(load_shader('window-slice'), frames) do |shader|
        shader.uniformf(shader.locate('count'), opts[:count] || 10.0)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.5)
        block.call
      end
    end

    def self.wipe(frames, direction: Direction::RIGHT, &block)
      raise LocalJumpError, ERROR_MESSAGE unless block_given?
      program = case direction
      when Direction::LEFT then load_shader('wipe-left')
      when Direction::RIGHT then load_shader('wipe-right')
      when Direction::UP then load_shader('wipe-up')
      when Direction::DOWN then load_shader('wipe-down')
      else raise ArgumentError, 'invalid direction, only cardinal directions accepted'
      end
      Graphics.transition(program, frames) { |shader| block.call }
    end

  


    ##
    # Performs a random transition using the default arguments.
    #
    # @param frames [Integer] The number of game updates the transition will take to perform. If `0` or less, 
    #   the method will return instantly without any effects being performed.
    # @yield Yields a block with no parameters, within which all logic to change the on-screen graphics must be performed.
    #  
    def self.random(frames, &block)
      methods = self.singleton_methods - [:load_shader, :translate_coords, :random]
      loop do
        meth = method(methods.sample)
        params = meth.parameters
        next unless 1 == params.count { |a| a.first == :req }
        next unless params.any? { |a| a.first == :block }
        return meth.call(frames, &block)
      end
    end

    private

    SHADER_EXTENSION = '.glsl'
    ERROR_MESSAGE = 'block required'

    private_constant :SHADER_EXTENSION, :ERROR_MESSAGE
    
    ##
    # @api private
    def self.load_shader(filename)
      key = filename.to_sym
      unless @cache[key]
        path = File.join(BASE_DIRECTORY, 'assets', 'transitions', filename + SHADER_EXTENSION)
        @cache[key] = Shader.new(VERT, FRAG % File.read(path))
      end
      @cache[key]
    end

    ##
    # @api private
    def self.translate_coords(point)
      return Vector2.new(0.5, 0.5) unless point
      x = Float(point.x) / Graphics.width
      y = Float(Graphics.height - point.y) / Graphics.height
      Vector2.new(x, y)
    end

    
  end
end