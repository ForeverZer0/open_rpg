
module OpenRPG

  ##
  # Provides methods to control transitioning between game scenes.
  #
  # The {#update} method is invoked from the C API at the specified interval defined by the frame rate.
  #
  # @see {Scene}
  module Game

    @scene = nil
    @history = []

    ##
    # @!accessor [rw] tps
    #   @return [Integer] the "ticks-per-second" the {update} method will be invoked at.

    ##
    # @!accessor [rw] tick
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