#pragma once

#include <string>
#include <vector>

#include <math/vector2.hpp>

#include "collider.hpp"

namespace jrag::collision
{  
   class map
   {
      template<typename NumType>
      using vector2 = math::vector2<NumType>;
      
      public:
         auto create_collider(std::string uuid, vector2<float> position, vector2<float> size) -> void;
         [[nodiscard]] auto get_collider(std::string_view collider_id) -> collider &;
         auto handle_collisions() -> void;

      private:
         std::vector<collider> m_colliders;
         std::vector<std::string> m_collider_ids;
   };
}