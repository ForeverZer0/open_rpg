
module OpenRPG

  ##
  # Top-level namespace for all input releated functionality, including keyboard, mouse/touchpad, 
  # key bindings, and text capturing.
  #
  # Also contains modules that are analogs of a C-style enum for strongly-typed names of keys,
  # mouse buttons, etc.
  module Input

    ##
    # Creates a key/button binding, allowing to query a single symbol that checks for multiple
    # inputs.
    #
    # @param sym [Symbol] The symbol that will be used to query this binding.
    # @param keys [Array<Integer>] `0` or more {Key} constants the symbol is bound to.
    # @param buttons [Array<Integer>] `0` or more {Mouse} button constants the symbol is bound to.
    #
    # @example 
    #   # Binds movement to the arrow and WASD keys, allowing to query each with single symbol.
    #   Input.bind(:LEFT, [Input::Key::LEFT, Input::Key::A], nil)
    #   Input.bind(:RIGHT, [Input::Key::RIGHT, Input::Key::D], nil)
    #   Input.bind(:UP, [Input::Key::UP, Input::Key::W], nil)
    #   Input.bind(:DOWN, [Input::Key::DOWN, Input::Key::S], nil)
    #
    #   # Binds input to both keyboard and mouse input..
    #   Input.bind(:CONFIRM, [Input::Key::SPACE], [Input::Mouse::LEFT])
    #   Input.bind(:CANCEL, [Input::Key::Z], [Input::Mouse::RIGHT])
    #
    # @return [void]
    def self.bind(sym, keys, buttons); end

    ##
    # Removes a binding if it exists, or does nothing if not found.
    #
    # @param sym [Symbol] The symbol to unbind.
    #
    # @return [Boolean] `true` if symbol was bound, otherwise `false` if there was no binding.
    def self.unbind(sym); end

    ##
    # Enumerates through each current binding.
    #
    # @yield [sym,keys,buttons] Yields information about the binding to the block.
    # @yieldparam sym [Symbol] The symbol that will be used to query this binding.
    # @yieldparam keys [Array<Integer>] `0` or more {Key} constants the symbol is bound to.
    # @yieldparam buttons [Array<Integer>] `0` or more {Mouse} button constants the symbol is bound to.
    #
    # @return [void]
    def self.each_binding(&block); end

    ##
    # Checks the state of the input since the last update. If input was pressed, will return `true`, otherwise
    # `false`. Will only return `true` during for a single update, then its "triggered" state will be cleared,
    # even if input continues to be pressed.
    #
    # @param sym [Symbol] A previously bound symbol.
    #
    # @return [Boolean] `true` if input was triggered during this update, otherwise `false`.
    #
    # @see bind
    def self.trigger?(sym); end

    ##
    # Checks if the input is being held, and returns `true` at specific intervals. The interval between updates of
    # the repeat state are platform dependent, but commonly around every 25ms, with an initial delay around 500ms.
    #
    # @param sym [Symbol] A previously bound symbol.
    #
    # @return [Boolean] `true` if input was repeated during this update, otherwise `false`.
    #
    # @see bind
    def self.repeat?(sym); end

    ##
    # Checks if the input is being held, and returns `true` if it is. This method will return `true` from the moment a
    # input is first pressed until it is released.
    #
    # @param sym [Symbol] A previously bound symbol.
    #
    # @return [Boolean] `true` if input is being pressed, otherwise `false`.
    #
    # @see bind
    def self.press?(sym); end

    ##
    # Checks if the input has been released,and returns `true` if it was since the last update. The released
    # state will be cleared from this input with the next update.
    #
    # @param sym [Symbol] A previously bound symbol.
    #
    # @return [Boolean] `true` if input was released, otherwise `false`.
    #
    # @see bind
    def self.release?(sym); end

    ##
    # Provides an interface to keyboard input and related functions.
    module Keyboard

      ##
      # Checks the state of the key since the last update. If key was pressed, will return `true`, otherwise
      # `false`. Will only return `true` during for a single update, then its "triggered" state will be cleared,
      # even if key continues to be pressed.
      #
      # @param key [Integer] A key constant.
      #
      # @return [Boolean] `true` if key was triggered during this update, otherwise `false`.
      #
      # @see Key
      def self.trigger?(key); end

      ##
      # Checks if this key is being held, and returns `true` at specific intervals. The interval between updates of
      # the repeat state are platform dependent, but commonly around every 25ms, with an initial delay around 500ms.
      #
      # @param key [Integer] A key constant.
      #
      # @return [Boolean] `true` if key was repeated during this update, otherwise `false`.
      #
      # @see Key
      def self.repeat?(key); end

      ##
      # Checks if the key has been released,and returns `true` if it was since the last update. The released
      # state will be cleared from this key with the next update.
      #
      # @param key [Integer] A key constant.
      #
      # @return [Boolean] `true` if key was released, otherwise `false`.
      #
      # @see Key
      def self.release?(key); end

      ##
      # Checks if the key is being held, and returns `true` if it is. This method will return `true` from the moment a
      # key is first pressed until it is released.
      #
      # @param key [Integer] A key constant.
      #
      # @return [Boolean] `true` if key is being pressed, otherwise `false`.
      #
      # @see Key
      def self.press?(key); end

      ##
      # Query the keyboard layout dependent name of a printable key.
      #
      # @param key [Integer] A key constant.
      #
      # @return [String?] the key name, or `nil` if key has no printable name.
      def self.key_name(key); end

      ##
      # Query the keyboard layout dependent name of a printable scancode.
      #
      # @param scancode [Integer] A scancode.
      #
      # @return [String?] the scancode name, or `nil` if scancode has no printable name.
      def self.scancode_name(scancode); end

      ##
      # Returns the platform-specific scancode of the specified key. The scancode is unique
      # for every key, regardless of whether it has a key token. Scancodes are platform-specific
      # but consistent over time, so keys will have different scancodes depending on the platform
      # but they are safe to save to disk. 
      #
      # @param key [Integer] A key constant.
      #
      # @return [Integer] the scancode for the specified key.
      #
      # @see Key
      def self.scancode(key); end

      ##
      # @note This method is not synced to the game update, and can be called between frames.
      # @note Only a single callback may be linked to this method at a time.
      #
      # Sets a block to be called whenever keyboard input is received.
      #
      # @overload on_key(&block)
      #   When called with a block, sets a block of code that will be invoked each time the key input is
      #   recieved. 
      #   
      #   @yield [key, scancode,action,mods] Yields input information to the block.
      #   @yieldparam key [Integer] The key constant that raised the event.
      #   @yieldparam scancode [Integer] The corresponding scancode for the invoking key.
      #   @yieldparam action [Integer] The action of the key that caused the event: `0` if released, `1` if pressed, or `2` if repeated.
      #   @yieldparam mods [Integer] Any modifier keys that were present during the key action.
      #
      # @overload on_key
      #   When called without a block, removed any callback that may currently be set.
      #
      # @return [void]
      #
      # @see Key
      # @see ModKey
      def self.on_key(&block); end
    end

    ##
    # Provides an interface to mouse/touchpad input and releated functions.
    #
    # @note For the constants, it is highly recommended **not** to use the numerical values directly, and always 
    # reference by name in the event that the constant's value is ever changed in a future version.
    module Mouse

      BUTTON_1 = 0
			BUTTON_2 = 1
			BUTTON_3 = 2
			BUTTON_4 = 3
			BUTTON_5 = 4
			BUTTON_6 = 5
			BUTTON_7 = 6
      BUTTON_8 = 7

      ##
      # Same as {BUTTON_1}, represents the left-mouse button.
      LEFT = BUTTON_1

      ##
      # Same as {BUTTON_2}, represents the right-mouse button.
      RIGHT = BUTTON_2

      ##
      # Same as {BUTTON_3}, represents the middle-mouse button.
      MIDDLE = BUTTON_3
      
      ##
      # The regular arrow cursor shape.
      ARROW_CURSOR = 0x36001
      
      ##
      # The text input I-beam cursor shape.
      IBEAM_CURSOR = 0x36002
      
      ##
      # The crosshair shape.
      CROSSHAIR_CURSOR = 0x36003
      
      ##
      # The hand shape.
      HAND_CURSOR = 0x36004
      
      ##
      # The horizontal resize arrow shape.
      HRESIZE_CURSOR = 0x36005
      
      ##
      # The vertical resize arrow shape.
      VRESIZE_CURSOR = 0x360016

      ##
      # Checks the state of the mouse button since the last update. If button was pressed, will return `true`, otherwise
      # `false`. Will only return `true` during for a single update, then its "triggered" state will be cleared,
      # even if button continues to be pressed.
      #
      # @param button [Integer] A mouse button constant.
      #
      # @return [Boolean] `true` if button was triggered during this update, otherwise `false`.
      def self.trigger?(button); end

      ##
      # Checks if this mouse button is being held, and returns `true` at specific intervals. The interval between updates of
      # the repeat state are platform dependent, but commonly around every 25ms, with an initial delay around 500ms.
      #
      # @param button [Integer] A mouse button constant.
      #
      # @return [Boolean] `true` if button was repeated during this update, otherwise `false`.
      def self.repeat?(button); end

      ##
      # Checks if the mouse button has been released,and returns `true` if it was since the last update. The released
      # state will be cleared from this button with the next update.
      #
      # @param button [Integer] A mouse button constant.
      #
      # @return [Boolean] `true` if button was released, otherwise `false`.
      def self.release?(button); end

      ##
      # Checks if the mouse button is being held, and returns `true` if it is. This method will return `true` from the moment a
      # button is first pressed until it is released.
      #
      # @param button [Integer] A mouse button constant.
      #
      # @return [Boolean] `true` if button is being pressed, otherwise `false`.
      def self.press?(button); end

      class << self

        ##
        # Retrieves the location of the mouse cursor on the x-axis, in **game** coordinates. The value is
        # translated to account for differences in game resolution vs window size, strecthing/shrinking, and
        # letterbox/pillars that may be applied.
        #
        # @return [Integer] the location of the mouse cursor on the x-axis.
        #
        # @see location
        attr_reader :x

        ##
        # Retrieves the location of the mouse cursor on the y-axis, in **game** coordinates. The value is
        # translated to account for differences in game resolution vs window size, strecthing/shrinking, and
        # letterbox/pillars that may be applied.
        #
        # @return [Integer] the location of the mouse cursor on the y-axis.
        #
        # @see location
        attr_reader :y

        ##
        # Retrieves the location of the mouse cursor, in **game** coordinates. The value is translated to account
        # for differences in game resolution vs window size, strecthing/shrinking, and letterbox/pillars that
        # may be applied.
        #
        # @return [Point] the location of the mouse cursor.
        #
        # @see x
        # @see y
        attr_reader :location

        ##
        # @return [Vector2] the scroll offset of the mouse wheel.
        attr_reader :scroll

        ##
        # @return [Float] the scroll offset of the mouse wheel on the x-axis.
        attr_reader :scroll_x

        ##
        # @return [Float] the scroll offset of the mouse wheel on the y-axis.
        attr_reader :scroll_y

      end

      ##
      # Changes the appearance of the cursor used for the game window.
      #
      # @overload change_cursor(cursor, x_hot = 0, y_hot = 0)
      #   Changes the cursor to a user-defined image.
      # 
      #   @param cursor [Image|String] An {Image} or a path to an image file to use as the mouse cursor.
      #   @param x_hot [Integer] The location relative to the top-left corner of the cursor where the
      #     "click point" is on the x-axis.
      #   @param y_hot [Integer] The location relative to the top-left corner of the cursor where the
      #     "click point" is on the y-axis.
      #
      # @overload change_cursor(cursor)
      #   Changes the cursor to a system defined cursor.
      #
      #   @param cursor [Integer] A XXXX_CURSOR constant representing the desired cursor, or `nil` to not display
      #     a cursor over the window.
      #   @see ARROW_CURSOR
      #   @see IBEAM_CURSOR
      #   @see CROSSHAIR_CURSOR
      #   @see HAND_CURSOR
      #   @see HRESIZE_CURSOR
      #   @see VRESIZE_CURSOR 
      # 
      # @return [void]
      def self.change_cursor(*args); end

      ##
      # Sets the behavior of the cursor over the window.
      #
      # @param mode [Symbol] A symbol representing the desired cursor mode, valid values include:
      #   <ul>
      #   <li><strong>:normal</strong> The cursor is visible and behaving normally.</li>
      #   <li><strong>:hidden</strong> The cursor is invisible when it is over the content area of the window but does not restrict the cursor from leaving.</li>
      #   <li><strong>:disabled</strong> Hides and grabs the cursor, providing virtual and unlimited cursor movement.</li>
      #   </ul>
      # @return [void]
      def self.cursor_mode(mode = :normal); end

      # @!group Callbacks

      ##
      # @note This method is not synced to the game update, and can be called between frames.
      # @note Only a single callback may be linked to this method at a time.
      #
      # Sets a block to be called whenever mouse cursor is moved.
      #
      # @overload on_move(&block)
      #   When called with a block, sets a block of code that will be invoked each time mouse cursor is moved.
      #   
      #   @yield [x,y] Yields input information to the block.
      #   @yieldparam x [Integer] The mouse cursor location onn the x-axis, releative to the client area of the window.
      #   @yieldparam y [Integer] The mouse cursor location onn the y-axis, releative to the client area of the window.
      #
      # @overload on_button
      #   When called without a block, removed any callback that may currently be set.
      #
      # @return [void]
      def self.on_move(&block); end

      ##
      # @note This method is not synced to the game update, and can be called between frames.
      # @note Only a single callback may be linked to this method at a time.
      #
      # Sets a block to be called whenever mouse button input is received.
      #
      # @overload on_button(&block)
      #   When called with a block, sets a block of code that will be invoked each time mouse button input is
      #   recieved. 
      #   
      #   @yield [button,action,mods] Yields input information to the block.
      #   @yieldparam button [Integer] The mouse button constant that raised the event.
      #   @yieldparam action [Integer] The action of the button that caused the event: `0` if released, `1` if pressed, or `2` if repeated.
      #   @yieldparam mods [Integer] Any modifier keys that were present during the button action.
      #
      # @overload on_button
      #   When called without a block, removed any callback that may currently be set.
      #
      # @return [void]
      def self.on_button(&block); end

      ##
      # @note This method is not synced to the game update, and can be called between frames.
      # @note Only a single callback may be linked to this method at a time.
      #
      # Sets a block to be called whenever mouse cursor enters the client area of the window.
      #
      # @overload on_enter(&block)
      #   When called with a block, sets a block of code that will be invoked when the cursor enters the window.
      #   @yield No arguments are yielded to the block.
      #
      # @overload on_enter
      #   When called without a block, removed any callback that may currently be set.
      #
      # @return [void]
      def self.on_enter(&block); end

      ##
      # @note This method is not synced to the game update, and can be called between frames.
      # @note Only a single callback may be linked to this method at a time.
      #
      # Sets a block to be called whenever mouse cursor leaves the client area of the window.
      #
      # @overload on_enter(&block)
      #   When called with a block, sets a block of code that will be invoked when the cursor leaves the window.
      #   @yield No arguments are yielded to the block.
      #
      # @overload on_enter
      #   When called without a block, removed any callback that may currently be set.
      #
      # @return [void]
      def self.on_leave(&block); end

      # @!endgroup
    end

    ##
    # Contains strongly-typed constants representing modifer keys. The values are bit-flags that can be conbined to
    # represent multiple modifier keys in a single value.
    #
    # @example Set Multiple Modifer Keys
    #   ctrl_alt = ModKey::CONTROL | ModKey::ALT
    #
    # @example Check if Modifier Key is Set
    #   ctrl_set = (ctrl_alt & ModKey::Control).nonzero?
    #   #=> true
    #
    #   shift_set = (ctrl_alt & ModKey::SHIFT).nonzero?
    #   #=> false
    #
    # @note It is highly recommended **not** to use the numerical values directly, and always reference by name in the 
    #   event that the constant's value is ever changed in a future version.
    module ModKey
      NONE = 0
			SHIFT = 1
			CONTROL = 2
			ALT = 4
			SUPER = 8
			CAPS_LOCK = 16
			NUM_LOCK = 32
    end

    ##
    # Contains strongly-typed constants representing keyboard keys.
    #
    # @note It is highly recommended **not** to use the numerical values directly, and always reference by name in the 
    #   event that the constant's value is ever changed in a future version.
    module Key
			SPACE = 32
			APOSTROPHE = 39
			COMMA = 44
			MINUS = 45
			PERIOD = 46
			SLASH = 47
			NUM_0 = 48
			NUM_1 = 49
			NUM_2 = 50
			NUM_3 = 51
			NUM_4 = 52
			NUM_5 = 53
			NUM_6 = 54
			NUM_7 = 55
			NUM_8 = 56
			NUM_9 = 57
			SEMICOLON = 59
			EQUAL = 61
			A = 65
			B = 66
			C = 67
			D = 68
			E = 69
			F = 70
			G = 71
			H = 72
			I = 73
			J = 74
			K = 75
			L = 76
			M = 77
			N = 78
			O = 79
			P = 80
			Q = 81
			R = 82
			S = 83
			T = 84
			U = 85
			V = 86
			W = 87
			X = 88
			Y = 89
			Z = 90
			LEFT_BRACKET = 91
			BACKSLASH = 92
			RIGHT_BRACKET = 93
			GRAVE_ACCENT = 96
			WORLD_1 = 161
			WORLD_2 = 162
			ESCAPE = 256
			ENTER = 257
			TAB = 258
			BACKSPACE = 259
			INSERT = 260
			DELETE = 261
			RIGHT = 262
			LEFT = 263
			DOWN = 264
			UP = 265
			PAGE_UP = 266
			PAGE_DOWN = 267
      HOME = 268
      # Constant name is actually `END`, not `END_`, but documention engine parses `END` as the Ruby keyword.
      END_ = 269
			CAPS_LOCK = 280
			SCROLL_LOCK = 281
			NUM_LOCK = 282
			PRINT_SCREEN = 283
			PAUSE = 284
			F1 = 290
			F2 = 291
			F3 = 292
			F4 = 293
			F5 = 294
			F6 = 295
			F7 = 296
			F8 = 297
			F9 = 298
			F10 = 299
			F11 = 300
			F12 = 301
			F13 = 302
			F14 = 303
			F15 = 304
			F16 = 305
			F17 = 306
			F18 = 307
			F19 = 308
			F20 = 309
			F21 = 310
			F22 = 311
			F23 = 312
			F24 = 313
			F25 = 314
			KP_0 = 320
			KP_1 = 321
			KP_2 = 322
			KP_3 = 323
			KP_4 = 324
			KP_5 = 325
			KP_6 = 326
			KP_7 = 327
			KP_8 = 328
			KP_9 = 329
			KP_DECIMAL = 330
			KP_DIVIDE = 331
			KP_MULTIPLY = 332
			KP_SUBTRACT = 333
			KP_ADD = 334
			KP_ENTER = 335
			KP_EQUAL = 336
			LEFT_SHIFT = 340
			LEFT_CONTROL = 341
			LEFT_ALT = 342
			LEFT_SUPER = 343
			RIGHT_SHIFT = 344
			RIGHT_CONTROL = 345
			RIGHT_ALT = 346
			RIGHT_SUPER = 347
			MENU = 348
    end
  end
end