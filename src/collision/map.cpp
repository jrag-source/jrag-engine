#include <cmath>
#include <collision/map.hpp>

#include <collision/collider.hpp>

#include <algorithm>
#include <print>
#include <ranges>
#include <cassert>

namespace
{
   auto get_collider(std::string_view const collider_id,
      std::span<std::string const> const collider_ids,
      std::span<jrag::collision::collider> const colliders) -> jrag::collision::collider &
   {
      auto const id_it {std::ranges::find(collider_ids, collider_id)};
      assert(id_it != collider_ids.cend());
      auto const index {std::distance(collider_ids.cbegin(), id_it)};
      return colliders[index];
   }
}

using map = jrag::collision::map;

auto map::create_collider(std::string uuid, vector2<float> position, vector2<float> size) -> void
{  
   m_colliders.emplace_back(position, size);
   m_collider_ids.emplace_back(std::move(uuid));
}

auto map::get_collider(std::string_view const collider_id) -> collider &
{
   return ::get_collider(collider_id, m_collider_ids, m_colliders);
}

auto map::handle_collisions() -> void
{
   auto entity_count{0Z};

   struct collision_data
   {
      std::reference_wrapper<jrag::collision::collider> collider_a;
      std::reference_wrapper<jrag::collision::collider> collider_b;
      jrag::math::rect<float> collision_rect;
   };
   
   std::vector<collision_data> collisions {};

   for (auto & collider : m_colliders)
   {
      entity_count += 1Z;

      for (auto & other_collider : m_colliders | std::views::drop(entity_count))
      {
         auto const intersection {collider.get_bounding_rect().get_intersection(other_collider.get_bounding_rect())};
         if (intersection.has_value())
         {
            collisions.emplace_back(collider, other_collider, *intersection);
         }
      }
   }

   std::ranges::sort(collisions, [] (collision_data const & collision_a, collision_data const & collision_b) -> bool
   {
      return collision_a.collision_rect.get_area() > collision_b.collision_rect.get_area();
   });

   for (auto const & collision : collisions)
   {
      auto & collider_a {collision.collider_a.get()};
      auto & collider_b {collision.collider_b.get()};
      auto const potential_collision {collider_a.get_bounding_rect().get_intersection(collider_b.get_bounding_rect())};

      if (!potential_collision.has_value()) { continue; } // no longer colliding

      auto const collision_rect {*potential_collision};

      // TODO: Handle case where needle collider is wedged into another collider (would result in wrong selection of resolve dimension e.g.: horizontal collision would be resolve vertically)

      if (collision_rect.size.x < collision_rect.size.y)
      {
         // Resolve across x-axis since it collides less
         auto const x_resolve {collider_a.get_bounding_rect().center().x > collider_b.get_bounding_rect().center().x ? collision_rect.size.x : -collision_rect.size.x};
         collider_a.set_center(collider_a.get_center() + jrag::math::vector2{x_resolve, 0.0F});
      }
      else
      {
         // Resolve across y-axis since it collides less
         auto const y_resolve {collider_a.get_bounding_rect().center().y > collider_b.get_bounding_rect().center().y ? collision_rect.size.y : -collision_rect.size.y};
         collider_a.set_center(collider_a.get_center() + jrag::math::vector2{0.0F, y_resolve});
      }
   }
}
