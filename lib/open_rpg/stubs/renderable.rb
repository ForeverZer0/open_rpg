module OpenRPG

  ##
  # Base class for objects that can be drawn on the screen, such as sprites, planes, viewports, etc.
  class Renderable

    ##
    # @return [Blend] the blending mode that will be applied when drawing this object.
    attr_accessor :blend

    ##
    # @note Unlike RPG Maker series, this is not an intensive operation, and is performed with a shader program.
    #
    # @return [Float] the amount of hue shifting to apply, with 360 degrees of displacement.
    attr_accessor :hue

    ##
    # @return [Vector2] the scaling to apply to the object on each axis, where 1.0 denotes actual pixel size.
    attr_accessor :scale

    ##
    # @return [Float] the scaling to apply to the object on the x-axis, where 1.0 denotes actual pixel size.
    attr_accessor :scale_x

    ##
    # @return [Float] the scaling to apply to the object on the y-axis, where 1.0 denotes actual pixel size.
    attr_accessor :scale_y

    ##
    # @return [Float] the angle, in degrees (`0..360`), the object will be rotated.
    attr_accessor :angle

    ##
    # @return [Point] the anchor around which the object rotates around.
    attr_accessor :anchor

    ##
    # The origin point of the object on the horizontal x-axis.
    # The effect this value has can vary with the class that implements it.
    #
    # @return [Integer]
    #
    # @see origin
    attr_accessor :ox

    ##
    # The origin point of the object on the vertical y-axis.
    # The effect this value has can vary with the class that implements it.
    #
    # @return [Integer]
    #
    # @see origin
    attr_accessor :oy

    ##
    # The origin point of the object.
    # The effect this value has can vary with the class that implements it.
    #
    # @return [Point]
    attr_accessor :origin

    ##
    # The objects location on the z-axis, which determines the order in which objects are drawn.
    #
    # The larger the value, the "closer" the object is to the player. When values are equal, the most recently created
    # instance will be displayed closer to the player.
    #
    # @return [Integer]
    attr_accessor :z

    ##
    # @return [Boolean] `true` if object is visible and will be drawn, otherwise `false`.
    attr_accessor :visible

    ##
    # @return [Color] the current flash color, or an empty color if not flashing.
    attr_reader :flash_color

    ##
    # @return [Integer] the current "tick" of the flash countdown, or `0` if not flashing.
    attr_reader :flash_duration

    ##
    # @return [Integer] the opacity the object is drawn at, between `0` (fully transparent) and `255` (fully opaque).
    attr_accessor :opacity

    ##
    # @return [Float] the opacity the object is drawn at, between `0.0` (fully transparent) and `1.0` (fully opaque).
    attr_accessor :alpha

    ##
    # @return [Color] the color to be blended with the object when drawn.
    #
    # @note Color blending uses linear interpolation of each pixel, where the alpha component determines the weight.
    attr_accessor :color

    ##
    # @return [Tone] the tone to be blended with the object when drawn.
    #
    # @note Tone blending uses basic +/- of each pixel on a component-wise basis. Grayscale is handled separately.
    attr_accessor :tone

    ##
    # Blends a color with the sprite for a specific duration.
    #
    # @param color [Color] the color to blend, or `nil` to cancel an existing flash effect.
    # @param duration [Integer] the number of frames to flash, between 1 and 255. Ignored if color is `nil`.
    #
    # @return [self]
    #
    # @note Color blending uses linear interpolation of each pixel, where the alpha component determines the weight.
    def flash(color, duration); end

    ##
    # @return [Boolean] `true` if object is currently flashing, otherwise `false`.
    def flashing?; end

    ##
    # Refreshes the flash effect. As a rule, this method is called once per frame.
    #
    # @return [self]
    #
    # @note It is not necessary to call this method if no flash effect is needed.
    def update; end

    ##
    # Frees all resouces that are outside the scope of Ruby's garbage collector.
    # Call this method when you are done using the object.
    #
    # @return [void]
    def dispose; end

    ##
    # @return [Boolean] `true` if object has been disposed, otherwise `false`.
    def disposed?; end
  end
end