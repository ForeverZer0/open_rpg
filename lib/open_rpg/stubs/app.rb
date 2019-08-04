module OpenRPG

  ##
  # Provides an interface to the native application window of the game.
  #
  # @note The methods within this class are **NOT** synced to occur within the main game loop, and may occur between updates.
  module App

    class << self

      ##
      # @return [Size] the size of the client area of the window, in **screen** coordinates.
      attr_reader :client_size

      ##
      # @return [Integer] the size of the client area of the window on the x-axis, in **screen** coordinates.
      attr_reader :client_width

      ##
      # @return [Integer] the size of the client area of the window on the y-axis, in **screen** coordinates.
      attr_reader :client_height

      ##
      # @return [String?] the system clipboard string, or `nil`.
      attr_accessor :clipboard

      ##
      # @return [Float] the opacity of the window, a value between `0.0` (fully transparent) and `1.0` (fully opaque).
      attr_accessor :opacity
    end

    ##
    # Sets the application icon(s), which is displayed in the window titlebar, taskbar, etc.
    #
    # @param icon [Image|String]
    def self.set_icon(*icon); end

    ##
    # Sets the close flag for the window, which issues a callback that can be intercepted and canceled with {on_close}.
    #
    # @param close [Boolean] `true` to close window, otherwise `false` to cancel the action.
    #
    # @return [void]
    #
    # @see on_close
    def self.close(close = true); end

    ##
    # @return [Boolean] `true` if the window close flag is set, otherwise `false`. 
    def self.closing?; end

    ##
    # Sets input focus to the game window.
    #
    # @return [void]
    def self.focus; end

    ##
    # @return [Boolean] `true` if window is focused and has input focus, otherwise `false`.
    def self.focused?; end

    ##
    # Minimizes the game window. If window is already minimized, does nothing.
    # 
    # @return [void]
    #
    # @see restore
    # @see on_minimize_change
    def self.minimize; end

    ##
    # Maximizes the game window. If window is already maximized, does nothing.
    # 
    # @return [void]
    #
    # @see restore
    # @see on_maximize_change
    def self.maximize; end

    ##
    # @return [Boolean] `true` if window is minimized, otherwise `false`.
    def self.minimized?; end

    ##
    # @return [Boolean] `true` if window is maximized, otherwise `false`.
    def self.maximized?; end

    ##
    # Restores the window from either a minimized or maximized state.
    #
    # @return [void]
    #
    # @see maximize
    # @see minimize
    # @see on_minimize_change
    # @see on_maximize_change
    def self.restore; end

    ##
    # Makes the specified window visible if it was previously hidden. If the window is already visible or is in full screen mode, this method does nothing.
    #
    # @return [void]
    #
    # @see hide
    def self.show; end

    ##
    # Hides the specified window if it was previously visible. If the window is already hidden or is in full screen mode, this method does nothing.
    #
    # @return [void]
    #
    # @see show
    def self.hide; end

    ##
    # Requests user attention to the specified window. On platforms where this is not supported, attention is requested to the application as a whole.
    # Once the user has given attention, usually by focusing the window or application, the system will end the request automatically.
    #
    # @return [void]
    def self.request_attention; end

    ##
    # Sets the position, in screen coordinates, of the upper-left corner of the content area of the window. If in fullscreen mode, does nothing.
    #
    # @overload move(location)
    #   @param location [Point] The new location of the window to set.
    #
    # @overload move(x, y)
    #   @param x [Integer] The new location of the window on the x-axis to set.
    #   @param y [Integer] The new location of the window on the y-axis to set.
    #
    # @return [void]
    #
    # @see on_move
    def self.move(*args); end

    ##
    # Returns a hash containing the dimensions of window frame, with 4 keys: `:left`, `:top`, `:right`, and `:bottom`.
    #
    # @return [Hash{Symbol=>Integer}] the hash containing the dimensions.
    def self.frame_size; end

    ##
    # Sets a callback when the the window focus state is changed.
    #
    # @overload on_focus_change(&block)
    #   When called with a block, sets the block that will be called when the application window gains or loses input focus.
    #   @yield [focued] Yields the new focus state to the block.
    #   @yieldparam focused [Boolean] `true` if window is being focused, otherwise `false` if losing focus.
    #
    # @overload on_focus_change
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_focus_change; end

    ##
    # Sets a callback when the the window position is changed.
    #
    # @overload on_move(&block)
    #   When called with a block, sets the block that will be called when the application window is moved.
    #   @yield [x, y] Yields the new window coordinates to the block.
    #   @yieldparam x [Integer] The new x-coordinate, in screen coordinates, of the upper-left corner of the content area of the window.
    #   @yieldparam y [Integer] The new y-coordinate, in screen coordinates, of the upper-left corner of the content area of the window.
    #
    # @overload on_move
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_move; end

    ##
    # Sets a callback when files are dragged-dropped onto the game window.
    #
    # @overload on_file_drop(&block)
    #   When called with a block, sets the block that will be called when the application has files dropped onto it.
    #   @yield [files] Yields the paths of the dropped files.
    #   @yieldparam files [Array<String>] An array containing the absolute paths of the files dropped on the window.
    #
    # @overload on_file_drop
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_file_drop; end

    ##
    # Sets a callback when the the window minimize state is changed.
    #
    # @overload on_minimize_change(&block)
    #   When called with a block, sets the block that will be called when the application window's minimize state is changed.
    #   @yield [minimized] Yields the minimize state to the block.
    #   @yieldparam minimized [Boolean] `true` if window is being minimized, otherwise `false` if being restored.
    #
    # @overload on_minimize_change
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_minimize_change; end

    ##
    # Sets a callback when the the window maximize state is changed.
    #
    # @overload on_maximize_change(&block)
    #   When called with a block, sets the block that will be called when the application window's maximize state is changed.
    #   @yield [maximized] Yields the maximize state to the block.
    #   @yieldparam maximized [Boolean] `true` if window is being maximized, otherwise `false` if losing maximized state.
    #
    # @overload on_maximize_change
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_maximize_change; end

    ##
    # Sets a callback when the the window is being closed.
    #
    # @overload on_close(&block)
    #   When called with a block, sets the block that will be called when the application window is about to close. This
    #   action can be cancelled by calling `App.close(false)`.
    #   @yield No arguments are passed to the block.
    #
    # @overload on_close
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_close; end

    ##
    # Sets a callback when the the window size is changed.
    #
    # @overload on_size_change(&block)
    #   When called with a block, sets the block that will be called when the application window size is changed.
    #   @yield [width, height] Yields the new dimensions to the block.
    #   @yieldparam width [Integer] The new width of the application window client area, in **screen** coordinates.
    #   @yieldparam height [Integer] The new height of the application window client area, in **screen** coordinates.
    #
    # @overload on_size_change
    #   When called without a block, clears any callback that may be set.
    #
    # @return [void]
    def self.on_size_change; end
  end
end