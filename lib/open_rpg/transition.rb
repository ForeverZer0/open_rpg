# frozen_string_literal: true

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
      Graphics.transition(load_shader('angular'), frames) do |shader|
        location = shader.locate("startingAngle")
        shader.uniformi(location, Integer(opts[:angle] || 90))
        block.call
      end
    end

    def self.bounce(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('bounce'), frames) do |shader|
        
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
      Graphics.transition(load_shader('bow-tie-h'), frames) do |shader|
        block.call
      end
    end

    def self.bow_tie_v(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('bow-tie-v'), frames) do |shader|
        block.call
      end
    end

    def self.burn(frames, color: Color.new(0.9, 0.4, 0.2), &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('burn'), frames) do |shader|
        location = shader.locate('color')
        shader.uniform_color(location, color)
        block.call
      end
    end

    def self.burnout(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('burnout'), frames) do |shader|
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.03)
        shader.uniform_vec2(shader.locate('center'), translate_coords(opts[:center]))
        c = opts[:color] || Colors.black
        shader.uniformf(shader.locate('color'), c.r, c.g, c.b)
        block.call
      end
    end

    def self.circle_open(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('circle-open'), frames) do |shader|

        location = shader.locate('smoothness')
        shader.uniformf(location, opts[:smoothness] || 0.3)

        location = shader.locate('opening')
        opening = opts.has_key?(:opening) ? opts[:opening] ? 1 : 0 : 1
        shader.uniformi(location, opening)

        block.call
      end
    end

    def self.circle(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('circle'), frames) do |shader|

        location = shader.locate("backColor")
        shader.uniform_color(location, opts[:color] || Colors.black)

        location = shader.locate("center")
        shader.uniform_vec2(location, translate_coords(opts[:center]))

        location = shader.locate("ratio")
        shader.uniformf(location, opts[:ration] || (Float(Graphics.width) / Graphics.height))

        block.call
      end
    end

    def self.color_phase(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('color-phase'), frames) do |shader|

        location = shader.locate('fromStep')
        shader.uniform_vec4(location, opts[:from] || Vector4.new(0.0, 0.2, 0.4, 0.0))

        location = shader.locate('toStep')
        shader.uniform_vec4(location, opts[:to] || Vector4.new(0.6, 0.8, 1.0, 1.0))

        block.call
      end
    end

    def self.color_distance(frames, power: 0.5, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('color-distance'), frames) do |shader|
        shader.uniformf(shader.locate('power'), power)
        block.call
      end
    end

    def self.crosshatch(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('crosshatch'), frames) do |shader|

        location = shader.locate('center')
        shader.uniform_vec2(location, translate_coords(opts[:center]))

        location = shader.locate('threshold')
        shader.uniformf(location, opts[:threshold] || 3.0)

        location = shader.locate('edge')
        shader.uniformf(location, opts[:edge] || 0.1)

        block.call
      end
    end

    def self.cross_warp(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('cross-warp'), frames) do |shader|
        block.call
      end
    end

    def self.cross_zoom(frames, strength: 0.4, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('cross-zoom'), frames) do |shader|

        location = shader.locate('strength')
        shader.uniformf(location, strength)

        block.call
      end
    end

    def self.cube(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('cube'), frames) do |shader|

        location = shader.locate('persp')
        shader.uniformf(location, opts[:perspective] || 0.7)

        location = shader.locate('unzoom')
        shader.uniformf(location, opts[:unzoom] || 0.3)

        location = shader.locate('reflection')
        shader.uniformf(location, opts[:reflection] || 0.4)

        location = shader.locate('floating')
        shader.uniformf(location, opts[:floating] || 3.0)

        block.call
      end
    end

    def self.curtain(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('curtain'), frames) do |shader|
        shader.uniformf(shader.locate('dots'), opts[:dots] || 20.0)

        center = translate_coords(opts[:center] || Vector2.new(0, Graphics.height))
        shader.uniform_vec2(shader.locate('center'), center)

        block.call
      end
    end

    def self.directional_warp(frames, direction: Vector2.new(-1.0, 1.0), &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('directional-warp'), frames) do |shader|
        shader.uniform_vec2(shader.locate('direction'), direction)
        block.call
      end
    end


    def self.directional_wipe(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('directional-wipe'), frames) do |shader|

        location = shader.locate('direction')
        shader.uniform_vec2(location, opts[:direction] || Vector2.new(1.0, -1.0))

        location = shader.locate('smoothness')
        shader.uniformf(location, opts[:smoothness] || 0.5)

        block.call
      end
    end

    def self.directional(frames, direction: Vector2.new(0.0, 1.0), &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('directional'), frames) do |shader|
        shader.uniform_vec2(shader.locate('direction'), direction)
        block.call
      end
    end

    # TODO: Displacement shader, need to bind another texture, create and stretch, etc
    # luma shader

    def self.doom(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('doom'), frames) do |shader|

        location = shader.locate('bars')
        shader.uniformi(location, opts[:bar] || 30)

        location = shader.locate('amplitude')
        shader.uniformf(location, opts[:amplitude] || 2.0)

        location = shader.locate('noise')
        shader.uniformf(location, opts[:noise] || 0.1)

        location = shader.locate('frequency')
        shader.uniformf(location, opts[:frequency] || 0.5)

        location = shader.locate('dripScale')
        shader.uniformf(location, opts[:drip] || 0.5)

        block.call
      end
    end

    def self.doorway(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('doorway'), frames) do |shader|

        location = shader.locate('reflection')
        shader.uniformf(location, opts[:reflection] || 0.4)

        location = shader.locate('prespective')
        shader.uniformf(location, opts[:perspective] || 0.4)

        location = shader.locate('depth')
        shader.uniformf(location, opts[:depth] || 3.0)

        block.call
      end
    end

    def self.dreamy(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('dreamy'), frames) do |shader|
        block.call
      end
    end

    def self.dreamy_zoom(frames, **opts, &block)
      # TODO: Remove this? Kinda below-average quality...
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('dreamy-zoom'), frames) do |shader|

        location = shader.locate('rotation')
        shader.uniformf(location, opts[:rotation] || 6.0)

        location = shader.locate('scale')
        shader.uniformf(location, opts[:scale] || 1.2)

        block.call
      end
    end

    def self.fade(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('fade'), frames) do |shader|
        block.call
      end
    end

    def self.fade_color(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('fade-color'), frames) do |shader|

        location = shader.locate('color')
        c = opts[:color] || Colors.black
        shader.uniformf(location, c.r, c.g, c.b)

        location = shader.locate('colorPhase')
        shader.uniformf(location, opts[:phase] || 0.4)

        block.call
      end
    end

    def self.fade_grayscale(frames, intensity: 0.3, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('fade-grayscale'), frames) do |shader|
        shader.uniformf(shader.locate('intensity'), intensity)
        block.call
      end
    end

    def self.film_burn(frames, seed: 2.31, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('film-burn'), frames) do |shader|
        shader.uniformf(shader.locate('Seed'), seed)
        block.call
      end
    end
  
    def self.flyeye(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('flyeye'), frames) do |shader|

        location = shader.locate('size')
        shader.uniformf(location, opts[:size] || 0.04)

        location = shader.locate('zoom')
        shader.uniformf(location, opts[:zoom] || 50.0)

        location = shader.locate('colorSeparation')
        shader.uniformf(location, opts[:separation] || 0.3)

        block.call
      end
    end

    def self.glitch_displace(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('glitch-displace'), frames) do |shader|
        block.call
      end
    end

    def self.glitch_memories(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('glitch-memories'), frames) do |shader|
        block.call
      end
    end

    def self.grid_flip(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('grid-flip'), frames) do |shader|

        location = shader.locate('size')
        size = opts[:size] || Size.new(4, 4)
        shader.uniformi(location, size.width, size.height)

        location = shader.locate('pause')
        shader.uniformf(location, opts[:pause] || 0.1)

        location = shader.locate('dividerWidth')
        shader.uniformf(location, opts[:divider] || 0.05)

        location = shader.locate('bgcolor')
        shader.uniform_color(location, opts[:background] || Colors.black)

        location = shader.locate('randomness')
        shader.uniformf(location, opts[:randomness] || 0.1)

        block.call
      end
    end

    def self.heart(frames, &block) 
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('heart'), frames) do |shader|
        block.call
      end
    end

    def self.hexagons(frames, **opts, &block)
      # FIXME: Not working
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('hexagonalize'), frames) do |shader|

        location = shader.locate('steps')
        shader.uniformi(location, opts[:steps] || 2)

        location = shader.locate('horizontalHexagons')
        shader.uniformf(location, opts[:horizontal] || 10.0)

        block.call
      end
    end

    def self.kaleidoscope(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('kaleidoscope'), frames) do |shader|

        location = shader.locate('speed')
        shader.uniformf(location, opts[:speed] || 1.0)

        location = shader.locate('angle')
        shader.uniformf(location, opts[:angle] || 1.0)

        location = shader.locate('power')
        shader.uniformf(location, opts[:power] || 1.5)

        block.call
      end
    end

    def self.linear_blur(frames, intensity: 0.1, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('kaleidoscope'), frames) do |shader|
        shader.uniformf(shader.locate('intensity'), intensity)
        block.call
      end
    end

    def self.luminance_melt(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('luminance-melt'), frames) do |shader|

        location = shader.locate('direction')
        d  = opts.has_key?(:direction) ? opts[:direction] ? 1 : 0 : 1
        shader.uniformi(location, d)

        location = shader.locate('threshold')
        shader.uniformf(location, opts[:threshold] || 0.8)

        location = shader.locate('above')
        shader.uniformi(location, opts[:above] || 0)

        block.call
      end
    end

    def self.morph(frames, strength: 0.1, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('morph'), frames) do |shader|
        shader.uniformf(shader.locate('strength'), strength)
        block.call
      end
    end

    def self.mosaic(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('mosaic'), frames) do |shader|

        location = shader.locate('endX')
        shader.uniformi(location, opts[:x] || 2)

        location = shader.locate('endY')
        shader.uniformi(location, opts[:y] || -1)

        block.call
      end
    end

    def self.multiply_blend(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('multiply-blend'), frames) do |shader|
        block.call
      end
    end

    def self.page_curl(frames, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('page-curl'), frames) do |shader|
        block.call
      end
    end

    def self.parametrics(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('parametrics'), frames) do |shader|

        location = shader.locate('a')
        shader.uniformf(location, opts[:a] || 4.0)

        location = shader.locate('b')
        shader.uniformf(location, opts[:b] || 1.0)

        location = shader.locate('amplitude')
        shader.uniformf(location, opts[:amplitude] || 120.0)

        location = shader.locate('smoothness')
        shader.uniformf(location, opts[:smoothness] || 0.1)

        block.call
      end
    end

    def self.perlin(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('perlin'), frames) do |shader|

        location = shader.locate('scale')
        shader.uniformf(location, opts[:scale] || 4.0)

        location = shader.locate('smoothness')
        shader.uniformf(location, opts[:smoothness] || 0.01)

        location = shader.locate('seed')
        shader.uniformf(location, opts[:seed] || 12.9898)

        block.call
      end
    end

    def self.pinwheel(frames, speed: 2.0, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('pinwheel'), frames) do |shader|
        shader.uniformf(shader.locate('speed'), speed)
        block.call
      end
    end

    def self.pixelize(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('pixelize'), frames) do |shader|

        location = shader.locate('squaresMin')
        x = opts[:x] || 20
        y = opts[:y] || 20
        shader.uniformi(location, x, y)

        location = shader.locate('steps')
        shader.uniformi(location, opts[:steps] || 50)

        block.call
      end
    end

    def self.polar(frames, segments: 5, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('polar'), frames) do |shader|
        shader.uniformi(shader.locate('segments'), segments)
        block.call
      end
    end

    def self.radial(frames, smoothness: 1.0, &block)
      # FIXME: Remove? Same as angular, but no setting for angle
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('radial'), frames) do |shader|
        shader.uniformf(shader.locate('smoothness'), smoothness)
        block.call
      end
    end


    def self.ripple(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('ripple'), frames) do |shader|
        shader.uniformf(shader.locate('amplitude'), opts[:amplitude] || 100.0)
        shader.uniformf(shader.locate('speed'), opts[:speed] || 50.0)
        block.call
      end
    end

    def self.rotate_scale_fade(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('rotate-scale-fade'), frames) do |shader|
        shader.uniform_vec2(shader.locate('center'), translate_coords(opts[:center]))
        shader.uniformf(shader.locate('rotations'), opts[:rotation] || 1.0)
        shader.uniformf(shader.locate('scale'), opts[:scale] || 8.0)
        shader.uniform_color(shader.locate('backColor'), opts[:color] || Color.new(0.15, 0.15, 0.15, 1.0))
        block.call
      end
    end

    def self.simple_zoom(frames, speed: 0.8, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('simple-zoom'), frames) do |shader|
        shader.uniformf(shader.locate('speed'), speed)
        block.call
      end
    end

    def self.squares(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('squares'), frames) do |shader|
        shader.uniformi(shader.locate('size'), opts[:x] || 10, opts[:y] || 10)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.5)
        block.call
      end
    end

    def self.squares_wire(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('squares-wire'), frames) do |shader|
        shader.uniformi(shader.locate('squares'), opts[:x] || 10, opts[:y] || 10)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 1.6)
        d = opts[:direction] || Vector2.new(1.0, -0.5)
        shader.uniformf(shader.locate('direction'), d.x, d.y)
        block.call
      end
    end

    def self.squeeze(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('squeeze'), frames) do |shader|
        shader.uniformf(shader.locate('separation'), opts[:separation] || 0.4)
        block.call
      end
    end

    def self.swap(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('swap'), frames) do |shader|
        shader.uniformf(shader.locate('reflection'), opts[:reflection] || 0.4)
        shader.uniformf(shader.locate('perspective'), opts[:perspective] || 0.2)
        shader.uniformf(shader.locate('depth'), opts[:depth] || 3.0)
        block.call
      end
    end

    def self.swirl(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('swirl'), frames) do |shader|
        block.call
      end
    end

    def self.static(frames, offset: 0.05, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('static'), frames) do |shader|
        shader.uniformf(shader.locate('offset'), offset)
        block.call
      end
    end
  
    def self.waterdrop(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('waterdrop'), frames) do |shader|
        shader.uniformf(shader.locate('amplitude'), opts[:amplitude] || 30.0)
        shader.uniformf(shader.locate('speed'), opts[:speed] || 30.0)
        block.call
      end
    end

    def self.wind(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('wind'), frames) do |shader|
        shader.uniformf(shader.locate('size'), opts[:size] || 0.2)
        block.call
      end
    end

    def self.windowblinds(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('windowblinds'), frames) do |shader|
        block.call
      end
    end

    def self.window_slice(frames, **opts, &block)
      raise LocalJumpError, 'block required' unless block_given?
      Graphics.transition(load_shader('window-slice'), frames) do |shader|
        shader.uniformf(shader.locate('count'), opts[:count] || 10.0)
        shader.uniformf(shader.locate('smoothness'), opts[:smoothness] || 0.5)
        block.call
      end
    end

    def self.wipe(frames, direction: Direction::RIGHT, &block)
      raise LocalJumpError, 'block required' unless block_given?
      program = case direction
      when Direction::LEFT then load_shader('wipe-left')
      when Direction::RIGHT then load_shader('wipe-right')
      when Direction::UP then load_shader('wipe-up')
      when Direction::DOWN then load_shader('wipe-down')
      else raise ArgumentError, 'invalid direction, only cardinal directions accepted'
      end
      Graphics.transition(program, frames) { |shader| block.call }
    end

  








    def self.test(frames, &block)
      # TODO: Remove
      last = (self.singleton_methods - [:test, :load_shader, :translate_coords, :set]).sort.last
      p last
      method(last).call(frames, &block)


    end

    # TODO: Get random transition, each method?

    private


    ##
    # @api private
    def self.load_shader(filename)
      key = filename.to_sym
      unless @cache[key]
        path = File.join(BASE_DIRECTORY, 'assets', 'transitions', filename + '.glsl')
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