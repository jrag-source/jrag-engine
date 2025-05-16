#pragma once

#include <functional>
#include <vector>

namespace jrag::collision
{
   struct collider;
   
   class map
   {
      public:
         auto add_collider(collider & collider) -> void;
         auto handle_collisions() -> void;

      private:
         std::vector<std::reference_wrapper<collider>> m_colliders;
   };
}