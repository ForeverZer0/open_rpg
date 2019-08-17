
module OpenRPG

  ##
  # Represents a time interval, with fluid conversion between time and game ticks.
  class TimeSpan

    include Comparable

    ##
    # The maximum possible {TimeSpan} value.
    MAX = TimeSpan.from_milliseconds(0xFFFFFFFFFFFFFFFF)

    ##
    # The minumum possible {TimeSpan} value.
    ZERO = TimeSpan.from_milliseconds(0)

    ##
    # @param ms [Integer] The millisecond component of the interval.
    # @param sec [Integer] The second component of the interval.
    # @param min [Integer] The minute component of the interval.
    # @param hour [Integer] The hour component of the interval.
    # @param day [Integer] The day component of the interval.
    # @param year [Integer] The year component of the interval.
    def initialize(ms, sec = 0, min = 0, hour = 0, day = 0, year = 0); end

    ##
    # @note The result is dependent on current the {Game.frame_rate}.
    # @return [Float] the total value of the {TimeSpan} interval expressed in game ticks.
    def frames; end

    ##
    # @return [Integer] the millisecond component of the {TimeSpan} interval.
    def milliseconds; end

    ##
    # @return [Integer] the second component of the {TimeSpan} interval.
    def seconds; end

    ##
    # @return [Integer] the minute component of the {TimeSpan} interval.
    def minutes; end

    ##
    # @return [Integer] the hour component of the {TimeSpan} interval.
    def hours; end

    ##
    # @return [Integer] the day component of the {TimeSpan} interval.
    def days; end

    ##
    # @return [Integer] the year component of the {TimeSpan} interval.
    def years; end

    ##
    # @return [Float] the total value of the {TimeSpan} interval expressed in milliseconds.
    def total_milliseconds; end

    ##
    # @return [Float] the total value of the {TimeSpan} interval expressed in seconds.
    def total_seconds; end

    ##
    # @return [Float] the total value of the {TimeSpan} interval expressed in minutes.
    def total_minutes; end

    ##
    # @return [Float] the total value of the {TimeSpan} interval expressed in hours.
    def total_hours; end

    ##
    # @return [Float] the total value of the {TimeSpan} interval expressed in days.
    def total_days; end

    ##
    # @return [Float] the total value of the {TimeSpan} interval expressed in years.
    def total_years; end

    ##
    # @note The result is dependent on current the {Game.frame_rate}.
    #
    # Creates a new {TimeSpan} from the specified number of frames.
    #
    # @param value [Integer] The number of frames to create the span from.
    #
    # @return [TimeSpan]
    def self.from_frames(value); end

    ##
    # Creates a new {TimeSpan} from the specified number of milliseconds.
    #
    # @param value [Integer] The number of milliseconds to create the span from.
    #
    # @return [TimeSpan]
    def self.from_milliseconds(value); end

    ##
    # Creates a new {TimeSpan} from the specified number of seconds.
    #
    # @param value [Integer] The number of seconds to create the span from.
    #
    # @return [TimeSpan]
    def self.from_seconds(value); end

    ##
    # Creates a new {TimeSpan} from the specified number of minutes.
    #
    # @param value [Integer] The number of minutes to create the span from.
    #
    # @return [TimeSpan]
    def self.from_minutes(value); end

    ##
    # Creates a new {TimeSpan} from the specified number of hours.
    #
    # @param value [Integer] The number of hours to create the span from.
    #
    # @return [TimeSpan]
    def self.from_hours(value); end

    ##
    # Creates a new {TimeSpan} from the specified number of days.
    #
    # @param value [Integer] The number of days to create the span from.
    #
    # @return [TimeSpan]
    def self.from_days(value); end

    ##
    # Creates a new {TimeSpan} from the specified number of years.
    #
    # @param value [Integer] The number of years to create the span from.
    #
    # @return [TimeSpan]
    def self.from_years(value); end
  
    ##
    # Adds the values of two {TimeSpan} objects.
    #
    # @param other [TimeSpan] The interval to add to this instance.
    #
    # @return [TimeSpan] a new {TimeSpan}
    def +(other); end

    ##
    # Returns the interval between two {TimeSpan} objects.
    #
    # @param other [TimeSpan] The {TimeSpan} to measure from.
    #
    # @return [TimeSpan] a new {TimeSpan} equal to the distance between the two intervals.
    def -(other); end

    ##
    # Scalar multiplication.
    #
    # @param scalar [Numeric] The scalar amount to multiply this instance by.
    #
    # @return [TimeSpan] the result of the operation.
    def *(scalar); end

    ##
    # Scalar division.
    #
    # @param scalar [Numeric] The scalar amount to divide this instance by.
    #
    # @return [TimeSpan] the result of the operation.
    #
    # @raise [ZeroDivisionError]
    def /(scalar); end

    ##
    # @return [String] a well-formatted string represenation of this instance.
    def to_s; end

    ##
    # @return [String] an informative string representation of this instance.
    def inspect; end

    ##
    # @return [TimeSpan] a copy of this {TimeSpan} instance.
    def dup; end

    ##
    # @return [Hash<Symbol,Integer>] the hash representation of this instance.
    def to_h; end

    alias_method :add, :+
    alias_method :subtract, :-
    alias_method :multiply, :*
    alias_method :divide, :/
    alias_method :ticks, :frames
  end
end
