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
         auto const & [thinner_entity, wider_entity] {collider.size.x < other_collider.size.x ? std::pair{collider, other_collider} : std::pair{other_collider, collider}};
         std::pair const thinner_bound {thinner_entity.get_bounding_rect().first.x, thinner_entity.get_bounding_rect().second.x};
         std::pair const wider_bound {wider_entity.get_bounding_rect().first.x, wider_entity.get_bounding_rect().second.x};

         auto is_in_bounds = [] (auto const number, auto const bounds) -> bool
         {
            return number >= bounds.first && number <= bounds.second;
         };
         
         if (is_in_bounds(thinner_bound.first, wider_bound)
            || is_in_bounds(thinner_bound.second, wider_bound))
         {
            auto const & [shorter_entity, taller_entity] {collider.size.y < other_collider.size.y ? std::pair{collider, other_collider} : std::pair{other_collider, collider}};
            std::pair const shorter_bound {shorter_entity.get_bounding_rect().first.y, shorter_entity.get_bounding_rect().second.y};
            std::pair const taller_bound {taller_entity.get_bounding_rect().first.y, taller_entity.get_bounding_rect().second.y};

            if (is_in_bounds(shorter_bound.first, taller_bound)
               || is_in_bounds(shorter_bound.second, taller_bound))
            {
               std::println("collision!");
            }
         }
      }
   }
}
