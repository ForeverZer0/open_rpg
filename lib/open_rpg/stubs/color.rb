
module OpenRPG

  class Tone

    ##
    # @return [Float] the red component value in a value between `-1.0` and `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :r

    ##
    # @return [Float] the green component value in a value between `-1.0` and `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :g

    ##
    # @return [Float] the blue component value in a value between `-1.0` and `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :b

    ##
    # @return [Float] the gray component value in a value between `0.0` (full color) and `1.0` (full grayscale).
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :gr

    ##
    # @return [Integer] the red component value in a value between `-255` and `255`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :red

    ##
    # @return [Integer] the green component value in a value between `-255` and `255`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :green

    ##
    # @return [Integer] the blue component value in a value between `-255` and `255`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :blue

    ##
    # @return [Integer] the grayscale component value in a value between `0` (full color) and `255` (full grayscale).
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :gray
  end

  ##
  # Describes a color in RGBA, HSV/HSB, and HSL color-spaces.
  # 
  # Internally the color is a structure encapsulating four 32-bit floating point values in RGBA order.
  class Color

    ##
    # @return [Float] the red component value in a value between `0.0` and `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :r

    ##
    # @return [Float] the green component value in a value between `0.0` and `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :g

    ##
    # @return [Float] the blue component value in a value between `0.0` and `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :b

    ##
    # @return [Float] the alpha component value in a value between `0.0` (fully transparent) and `1.0` (fully opaque).
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :a

    ##
    # @return [Integer] the red component value in a value between `0` and `255`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :red

    ##
    # @return [Integer] the green component value in a value between `0` and `255`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :green

    ##
    # @return [Integer] the blue component value in a value between `0` and `255`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :blue

    ##
    # @return [Integer] the alpha component value in a value between `0` (fully transparent) and `255` (fully opaque).
    # @note Out of range values will be automatically clamped to nearest valid value.
    attr_accessor :alpha

    ##
    # Creates a new Color instance.
    #
    # @param red [Float|Integer] The value of the red component.
    # @param green [Float|Integer] The value of the green component.
    # @param blue [Float|Integer] The value of the blue component.
    # @param alpha [Float|Integer] The value of the alpha component.
    # @note Out of range values will be automatically clamped to nearest valid value.
    # @note Integer values will be assumed to be in the range of a `0` to `255`, while float values will be interpreted in range of `0.0` to `1.0`.
    def initialize(red, green, blue, alpha = 1.0); end

    ##
    # Sets all values of the color at once.
    #
    # @param red [Float|Integer] The value of the red component.
    # @param green [Float|Integer] The value of the green component.
    # @param blue [Float|Integer] The value of the blue component.
    # @param alpha [Float|Integer] The value of the alpha component, or `nil` to not change.
    # @note Out of range values will be automatically clamped to nearest valid value.
    # @note Integer values will be assumed to be in the range of a `0` to `255`, while float values will be interpreted in range of `0.0` to `1.0`.
    def set(red, green, blue, alpha = nil); end

    ##
    # @return [Vector3] the RGB values of the color in the range of `0.0` to `1.0`.
    def rgb; end

      ##
    # @return [Vector4] the RGBA values of the color in the range of `0.0` to `1.0`.
    def rgb; end

    ##
    # @return [Vector3] the BGR values of the color in the range of `0.0` to `1.0`.
    def bgr; end

    ##
    # @return [Vector4] the BGRA values of the color in the range of `0.0` to `1.0`.
    def bgra; end

    ##
    # @return [Vector3] the hue, saturation, and value of the color.
    # @note This is exactly the same as {#hsb}.
    def hsv; end;

    ##
    # @return [Vector3] the hue, saturation, and brightness of the color.
    # @note This is exactly the same as {#hsv}.
    def hsb; end

    ##
    # @return [Vector3] the hue, saturation, and lightness of the color.
    def hsl; end

    ##
    # @return [String] the color as an HTML/CSS hex string.
    def html; end

    ##
    # Creates a new {Color} from values in the HSB colorspace
    # 
    # @param hue [Float] the hue component, in the range of `0.0` to `360.0`
    # @param saturation [Float] the saturation component, in the range of `0.0` to `1.0`
    # @param brightness [Float] the brightness component, in the range of `0.0` to `1.0`
    # @param alpha [Float] the alpha component, in the range of `0.0` to `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    # @note This is exactly the same as {from_hsv}.
    def self.from_hsb(hue, saturation, brightness, alpha = 1.0); end

    ##
    # Creates a new {Color} from values in the HSV colorspace
    # 
    # @param hue [Float] the hue component, in the range of `0.0` to `360.0`
    # @param saturation [Float] the saturation component, in the range of `0.0` to `1.0`
    # @param value [Float] the value component, in the range of `0.0` to `1.0`
    # @param alpha [Float] the alpha component, in the range of `0.0` to `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    # @note This is exactly the same as {from_hsb}.
    def self.from_hsv(hue, saturation, value, alpha = 1.0); end

    ##
    # Creates a new {Color} from values in the HSL colorspace
    # 
    # @param hue [Float] the hue component, in the range of `0.0` to `360.0`
    # @param saturation [Float] the saturation component, in the range of `0.0` to `1.0`
    # @param lightness [Float] the lightness component, in the range of `0.0` to `1.0`
    # @param alpha [Float] the alpha component, in the range of `0.0` to `1.0`.
    # @note Out of range values will be automatically clamped to nearest valid value.
    def self.from_hsl(hue, saturation, lightness, alpha = 1.0); end

    ##
    # Creates a new {Color} from the values of the given integer.
    #
    # @param int [Integer] The integer value, equivalent to a 32-bit unsigned integer in RRGGBBAA byte order.
    def self.unpack(int); end

    ##
    # @return [Integer] the color packed into a 32-bit unsigned integer in RRGGBBAA byte order.
    def to_i; end

    ##
    # Creates a new {Color} from this instance with the each component inverted.
    #
    # @param alpha [Boolean] `true` to invert alpha component, otherwise `false` to leave the same.
    #
    # @return [Color] the inverted color.
    def invert(alpha = false); end

    ##
    # Inverts each component of this color.
    #
    # @param alpha [Boolean] `true` to invert alpha component, otherwise `false` to leave the same.
    #
    # @return [self]
    def invert!(alpha = false); end

    ##
    # @return [Color] a copy of this color as a new instance.
    def dup; end
  end
end