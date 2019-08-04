module OpenRPG

  ##
  # Defines strongly-typed names for directions, as both left, top, right, bottom, and north, east, south, west.
  module Direction

    NONE = 0x00
    
    LEFT = 0x01
    TOP = 0x02
    RIGHT = 0x04
    BOTTOM = 0x08
    TOP_LEFT = TOP | LEFT
    TOP_RIGHT = TOP | RIGHT
    BOTTOM_LEFT = BOTTOM | LEFT
    BOTTOM_RIGHT = BOTTOM | RIGHT

    WEST = LEFT
    NORTH = TOP
    EAST = RIGHT
    SOUTH = BOTTOM
    NORTH_WEST = TOP_LEFT
    NORTH_EAST = TOP_RIGHT
    SOUTH_WEST = BOTTOM_LEFT
    SOUTH_EAST = BOTTOM_RIGHT

    ALL = 0xFF
  end
end