
module OpenRPG

  module Input

    class << self

      private_instance_methods :on_text

      # @!group Text Capturing

      ##
      # Begins capturing text received from the user, translatiing input into a string, accounting
      # for keyboard layour, modifier keys, etc.
      #
      # @param str [String?] Iniital string to start with, or `nil` to start with empty buffer.
      #
      # @overload begin_capture(str = nil, &block)
      #   When called with a block, yields to the block, automatically ending capture and returning
      #   the captured string when the block exits,
      #   @return [String] the captured string.
      #
      # @overload begin_capture(str = nil)
      #   When called without a block, begin capturing text until {end_capture} is called.
      #   @return [void]
      # 
      # @see end_capture
      def begin_capture(str = nil)
        return if @_capture
        @_capture = true   
        @_buffer = str ? str.codepoints : Array.new
        on_text { |cp| @_buffer << cp }
        if block_given?
          yield
          finish
        end
      end

      ##
      # Clears the capture buffer, while staying in capture mode.
      #
      # @return [void]
      def clear_capture
        return unless @_capture
        @_buffer.clear
      end
  
      ##
      # Removes the last captured character from the buffer when capturing user text.
      # Essentially acts as a "backspace" command.
      #
      # @return [Integer?] the removed codepoint, or `nil` if not capturing or buffer is empty.
      def capture_pop
        return unless @_capture
        @_buffer.pop
      end

      ##
      # @return [Boolean] `true` if currently capturing user text, otherwise `false`.
      def capturing?
        !!@_capture
      end
  
      ##
      # Ends text capturing, returning the captured string.
      #
      # @return [String?] the captured string, or `nil` if not in capturing mode.
      def end_capture
        return unless @_capture
        str = @_buffer.pack('U*')
        on_text
        @_buffer = nil
        @_capture = false
        str
      end

      # @!endgroup
    end
  end
end