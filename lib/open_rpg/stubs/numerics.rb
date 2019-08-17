
module OpenRPG

  module Numerics
    
    class Vector2
      attr_accessor :x
      attr_accessor :y
    end
  
    class Vector3
      attr_accessor :x
      attr_accessor :y
      attr_accessor :z
    end
  
    class Vector4
      attr_accessor :x
      attr_accessor :y
      attr_accessor :z
      attr_accessor :w
    end

    class Matrix3x2
      attr_accessor :m11
      attr_accessor :m12
      attr_accessor :m13
      attr_accessor :m21
      attr_accessor :m22
      attr_accessor :m23
      attr_accessor :m31
      attr_accessor :m32
      attr_accessor :m33
    end
  
    class Matrix4x4
      attr_accessor :m11
      attr_accessor :m12
      attr_accessor :m13
      attr_accessor :m14
      attr_accessor :m21
      attr_accessor :m22
      attr_accessor :m23
      attr_accessor :m24
      attr_accessor :m31
      attr_accessor :m32
      attr_accessor :m33
      attr_accessor :m34
      attr_accessor :m41
      attr_accessor :m42
      attr_accessor :m43
      attr_accessor :m44
    end
  end
end