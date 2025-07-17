#pragma once

#include "collider.hpp"

#include <math/vector2.hpp>

#include <string>
#include <vector>

namespace jrag::collision
{
   class map
   {
      template<typename NumType>
      using vector2 = math::vector2<NumType>;
      
      public:
         auto create_stationary_collider(std::string uuid, vector2<float> position, vector2<float> size) -> void;
         auto create_moving_collider(std::string uuid, vector2<float> position, vector2<float> size) -> void;
         [[nodiscard]] auto get_collider(std::string_view collider_id) -> collider &;
         auto handle_collisions() -> void;

      private:
         std::vector<collider> m_moving_colliders;
         std::vector<std::string> m_moving_collider_ids;

         std::vector<collider> m_stationary_colliders;
         std::vector<std::string> m_stationary_collider_ids;
   };
}