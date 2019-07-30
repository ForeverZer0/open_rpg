module OpenRPG

  ##
  # Mixin for classes that contain resources that must be freed manually.
  #
  # Implementors must define a `dispose` and `disposed?` method.
  module Disposable

    ##
    # Releases unmanaged resources that are outside of the  garbage collector's scope.
    #
    # @return [void]
    def dispose; end

    ##
    # @return [Boolean] `true` if the objects resources have been released, otherwise `false`.
    def disposed?; end
  end

end