
module OpenRPG

  class Window < Viewport

    class Selector < Layer

      def initialize(window)
        super(window)

        self.z = Layer::SELECTOR_Z
      end
    end

  end
end