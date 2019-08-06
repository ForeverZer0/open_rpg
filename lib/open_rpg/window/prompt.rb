
module OpenRPG

  class Window < Viewport

    class Prompt < Layer

      def initialize(window)
        super(window)

        self.z = Layer::PROMPT_Z

      end
    end

  end
end