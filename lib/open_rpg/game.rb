
module OpenRPG

  ##
  # Provides methods to control transitioning between game scenes.
  #
  # The {update) method is invoked from the C API at the specified interval defined by the frame rate.
  # Having the timing implemented in C allows for higher precision, and significantly less overhead.
  #
  # @see Scene
  module Game

    @scene = nil
    @history = []
    @started = false

    ##
    # Starts the first scene, and enters the main game loop.
    #
    # @param first_scene [Class] The class of the iniial game scene.
    # @param args [Array<Object>] Arguments to pass to the `initialize` method of the specified scene.
    #
    # @return [void]
    def self.start(first_scene, *args)
      raise RPGError 'game is already running' if @started
      begin
        goto(first_scene, *args)
        main
      ensure
        terminate
        @started = false
      end
    end

    ##
    # @api private
    #
    # Destroys the game window, and cleans up all resources used by the game.
    #
    # @note The game cannot be recovered from this state, and the Ruby VM will need to be relaunched to create another window.
    # @return [void]
    def self.terminate
      Font.finalize # TODO: Do internally

      OpenRPG.destroy
      
    end

    ##
    # @return [Boolean] `true` if game has been initialized and running, otherwise `false`.
    def self.running?
      @started 
    end

    ##
    # !@accessor [rw] tps
    #   @return [Integer] the "ticks-per-second" the {update} method will be invoked at.

    ##
    # !@accessor [rw] tick
    #   @return [Integer] the current number of elapsed game ticks.


    ##
    # @return [Scene] the current active scene that is receiving updates.
    def self.scene
      @scene
    end

    ##
    # Clears the scene history, closing each suspended scene before discarding.
    #
    # The scenes are closed in reverse order that they were created.
    #
    # @return [void]
    def self.clear
      @history.reverse.each { |s| scene.close }
      @history.clear
      nil
    end

    ##
    # Closes the current scene, and then creates and begins executing the specified scene. The current scene will be
    # discarded and ++NOT++ retained in the history.
    #
    # @param scene_class [Class] A class type that inherits from {Scene}.
    # @param args [Array<Object>] Optional arguments to be passed to the `initialize` method when creating the scene.
    #
    # @return [Scene] the created scene instance.
    def self.goto(scene_class, *args)
      if @scene
        @scene.close
      end
      raise TypeError, "#{scene_class} is not a Scene" unless scene_class < Scene
      @scene = scene_class.new(*args)
    end

    ##
    # Suspends the current scene, and then creates and begins executing the specified scene. The current scene will
    # remain suspended in the history.
    #
    # @param scene_class [Class] A class type that inherits from {Scene}.
    # @param args [Array<Object>] Optional arguments to be passed to the `initialize` method when creating the scene.
    #
    # @return [Scene] the created scene instance.
    def self.call(scene_class, *args)
      if @scene
        @scene.suspend
        @history.push(@scene)
      end
      raise TypeError, "#{scene_class} is not a Scene" unless scene_class < Scene
      @scene = scene_class.new(*args)
    end

    ##
    # Updates the current scene.
    #
    # @return [void]
    def self.update
      @scene.update if @scene
    end

    ##
    # Closes the current scene, and resumes updates on the previous scene in the history if one exists.
    #
    # @return [Scene] the created scene instance.
    def self.return
      if @scene 
        @scene.close
      end
      @scene = @history.pop
      @scene.resume
      @scene
    end
  end
end