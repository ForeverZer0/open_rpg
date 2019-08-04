module OpenRPG

  module Graphics

    ##
    # @note This method must be called prior to using ++ANY++ other class/method that relates to the native application 
    #   graphical object, or rendering (i.e. {Image}, {App}, {Sprite}, {Font}, etc.).
    #
    # Creates a native window on the system and initializes an OpenGL context for it.
    # 
    # @param width [Integer] the initial width of the rendering area, in pixels. Initially this is both the size of window's
    #   client area and the internal resolution of the game, but both can be changed independently of each other later.
    #
    # @param height [Integer] the initial width of the rendering area, in pixels. Initially this is both the size of window's
    #   client area and the internal resolution of the game, but both can be changed independently of each other later.
    #
    # @param caption [String?] The caption of the window, which will appear in the title bar of the window, the window's
    #   icons in the taskbar, etc. This value can be changed later.
    #
    # @param opts [Hash] The option's hash. 
    # @option opts [Boolean] :decorated (true) `true` if window should be decorated with titlebar, minimize/maximize/close widgets, etc., otherwise `false`.
    # @option opts [Boolean] :resizable (false) `true` if window should be resizable by the user, otherwise `false`. OpenRPG automatically stretches/shrinks and applies 
    #   pillars/letterbox to maintain the aspect ratio of the game's resolution, and a resized window will <strong>NOT</strong> effect the game's coordinate system.
    # @option opts [Boolean] :fullscreen (false) `true` if window should be started in fullscreen mode, otherwise `false`.
    # @option opts [Integer] :vsync (-1) Determines the sync between game rendering and the monitor's vertical refresh rate. 
    #    <ul>
    #    <li><code> -1</code>: Don't care, use system configured setting.</li>
    #    <li><code>  0</code>: Do not sync, render at maximum speed without regard to the monitor's refresh rate. Will give highest FPS, but increases chances of <a href="https://en.wikipedia.org/wiki/Screen_tearing">screen tearing</a>.</li>
    #    <li><code>>=1</code>: The number of frames to wait between monitor refreshes to render. A value of <code>1</code> is the same as being in sync, reducing chances of screen tearing, but limits FPS.</li>
    #    </ul>
    #    Note that these values are not guaranteed, and the system may override the specified behavior.
    #
    # @return [void]
    def self.create(width, height, caption = nil, **opts); end

    ##
    # @api private
    # 
    # Destroys the OpenGL context, cleaning up resources outside of the scope of Ruby's garbage collector.  
    #
    # @return [void]
    def destroy; end
  end
end