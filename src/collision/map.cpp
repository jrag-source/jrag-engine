#include <collision/map.hpp>

#include <collision/collider.hpp>

#include <print>
#include <ranges>
#include <utility>

using map = jrag::collision::map;

auto map::add_collider(collider & collider) -> void
{
   m_colliders.emplace_back(collider);
}

auto map::handle_collisions() -> void
{
   auto entity_count{0Z};

   auto get_ref = [] (auto wrapper) -> auto &
   {
      return wrapper.get();
   };

   for (auto & collider : m_colliders | std::views::transform(get_ref))
   {
      entity_count += 1Z;

      for (auto & other_collider : m_colliders | std::views::drop(entity_count) | std::views::transform(get_ref))
      {
         if (collider.is_intersecting(other_collider))
         {
            std::println("collision!");
         }
      }
   }
}
