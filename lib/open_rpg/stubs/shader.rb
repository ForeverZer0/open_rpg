
module OpenRPG

  ##
  # Basic interface around a shader program that uses <a href="https://www.khronos.org/opengl/wiki/Core_Language_(GLSL)">GLSL</a>. 
  #
  # Using this class requires general knowledge of OpenGL shaders, and their stage in the graphics pipeline. 
  # Note that the context OpenRPG creates is version 3.3, core profile, and any shader used should adhere to that standard. 
  class Shader

    ##
    # Disposes the shader, freeing its resources used internally.
    #
    # @return [void]
    def dispose; end

    # @return [Boolean] `true` if shader has been disposed, otherwise `false`.
    def disposed?; end

    ##
    # @overload use
    #   Activates the shader, making it current.
    #   @return [self]
    #
    # @overload use(&block)
    #   Activates the shader within the context of the given block, automatically restoring the current active shader when the block exits.
    #   @yield [shader] Yields the shader to the block.
    #   @yieldparam shader [self] This shader. 
    #   @return [void]
    def use; end

    ##
    # @example
    #   # INCORRECT
    #   shader.use unless shader.current?
    #
    #   # CORRECT
    #   shader.use
    # @return [Boolean] `true` if this is the current active shader, otherwise `false`.
    #
    # @note This is is not intended to be used to check if a shader should be activated before use, it is more efficient to just activate it.
    def current?; end;

    ##
    # Locates the numerical location used by OpenGL of the uniform with the specified name.
    #
    # @param name [String] The name of the uniform to locate.
    #
    # @return [Integer] The location of the uniform, or `0` if not found. It is recommended to store the result if the location will be reused frequently.
    def locate(name); end

    ##
    # Sets a uniform that consists of floating point values.
    #  
    # @overload uniformf(location, *args)
    #   @example 
    #     shader.uniformf(location, 0.0, 0.0, 0.0)
    #   @param location [Integer] The uniform location to change, returned by {#locate}. 
    #   @param args [Array<Float>] One or more Float values.
    #
    # @overload uniformf(location, array)
    #   @example
    #     ary = [2.0, 4.0, 6.0, 0.0, 1.0] 
    #     shader.uniformf(location, ary)
    #   @param location [Integer] The uniform location to change, returned by {#locate}. 
    #   @param array [Array<Float>] An array containing one or more Float values.
    #
    # @return [self]
    def uniformf(location, *args); end

    ##
    # Sets a uniform that consists of integer values.
    #  
    # @overload uniformi(location, *args)
    #   @example 
    #     shader.uniformi(location, 0, 89, 14)
    #   @param location [Integer] The uniform location to change, returned by {#locate}. 
    #   @param args [Array<Integer>] One or more Integer values.
    #
    # @overload uniformi(location, array)
    #   @example
    #     ary = [2, 4, 6, 0, 1] 
    #     shader.uniformi(location, ary)
    #   @param location [Integer] The uniform location to change, returned by {#locate}. 
    #   @param array [Array<Integer>] An array containing one or more Integer values.
    #
    # @return [self]
    def uniformi(location, *args); end

    ##
    # Sets a uniform that contains a {Color} value (`vec4` in GLSL)
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param color [Color] The color value to set.
    #
    # @return [self]
    def uniform_color(location, color); end

    ##
    # Sets a uniform that contains a {Tone} value (`vec4` in GLSL)
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param tone [Tone] The tone value to set.
    #
    # @return [self]
    def uniform_tone(location, tone); end

    ##
    # Sets a uniform that contains a vector value;
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param vector [Vector2] The vector value to set.
    #
    # @return [self]
    def uniform_vec2(location, vector); end

    ##
    # Sets a uniform that contains a vector value;
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param vector [Vector3] The vector value to set.
    #
    # @return [self]
    def uniform_vec3(location, vector); end

    ##
    # Sets a uniform that contains a vector value;
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param vector [Vector4] The vector value to set.
    #
    # @return [self]
    def uniform_vec4(location, vector); end

    ##
    # Sets a uniform that contains a {Point} value (`ivec2` in GLSL);
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param point [Point] The value to set.
    #
    # @return [self]
    def uniform_point(location, point); end

    ##
    # Sets a uniform that contains a {Size} value (`ivec2` in GLSL);
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param size [Size] The value to set.
    #
    # @return [self]
    def uniform_size(location, size); end

    ##
    # Sets a uniform that contains a {Rect} value (`ivec4` in GLSL);
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param rect [Rect] The value to set.
    #
    # @return [self]
    def uniform_rect(location, rect); end

    ##
    # Sets a uniform that contains a {Matrix4x4} value (`mat4` in GLSL);
    # 
    # @param location [Integer] The uniform location to change, returned by {#locate}. 
    # @param matrix [Matrix4x4] The value to set.
    # @param transpose [Boolean] `true` if the matrix should be transposed, otherwise `false`.
    #
    # @return [self]
    def uniform_mat4(location, matrix, transpose = false); end
  end
end