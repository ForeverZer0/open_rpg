module OpenRPG
  
  ##
  # The base class for game scenes.
  # 
  # @abstract The class offers no functionality, only defines basic structure and placeholders for methods invoked by the {Game}
  #   module. These methods can be safely overridden without aliasing in derived classes.
  #
  # @see Game
  class Scene

    ##
    # Updates game logic for the current scene.
    #
    # @return [void]
    def update
    end

    ##
    # Indicates the scene is being resumed from a suspended state.
    #
    # This is called after the scene is made current, but before it begins recieving updates again.
    #
    # @return [void]
    def resume
    end

    ##
    # Indicates the scene is being suspended, but may be returned to.
    #
    # This can be used to retain the state of the scene, though it will not be updated.
    #
    #
    # @return [void]
    def suspend
    end

    ##
    # Closes the scene completely.
    #
    # @return [void]
    def close
    end
  end
end