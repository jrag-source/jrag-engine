#pragma once

#include <math/vector2.hpp>

#include <chrono>
#include <vector>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace jrag::asset::sprite_sheet
{
   class view final
   {
         using vector2u = jrag::math::vector2u;

      public:
         template<vector2u Dimensions, vector2u CellPosition,
            vector2u Offset = {0U, 0U}>
         [[nodiscard]] static auto get_sprite(
            sf::Texture const & sheet) noexcept -> sf::Sprite
         {
            static_assert(Dimensions.x > 0U && Dimensions.y > 0U,
               "Sprite Sheet dimensions must be greater than 0");
            static_assert(
               CellPosition.x < Dimensions.x && CellPosition.y < Dimensions.y,
               "Invalid cell requested from sprite sheet");

            return get_sprite_impl(sheet, Dimensions, CellPosition, Offset);
         }

      private:
         [[nodiscard]] static auto get_sprite_impl(sf::Texture const & sheet,
            vector2u dimensions, vector2u cell_position,
            vector2u offset) noexcept -> sf::Sprite;
   };

   class animation final
   {
         using vector2u = jrag::math::vector2u;
         using duration = std::chrono::duration<float>;
         template<vector2u Dimensions>
         struct frame_validator;

      public:
         template<vector2u Dimensions, bool Loop = true,
            vector2u Offset = {0U, 0U}>
         [[nodiscard]] static auto make(sf::Texture const & sheet,
            frame_validator<Dimensions> const validator) -> animation
         {
            return {sheet, Dimensions, Loop, Offset, validator.m_frames};
         }

         ~animation() = default;

         animation(animation const &) = delete;
         auto operator=(animation const &) = delete;

         auto advance(duration delta_time) noexcept -> void;

         [[nodiscard]] auto get_frame() const noexcept -> sf::Sprite;

         struct frame_data
         {
               vector2u m_cell_position;
               duration m_duration;
         };

      private:
         sf::Texture const & m_sheet;
         vector2u m_dimensions;
         bool m_loop;
         vector2u m_offset;
         std::vector<frame_data> m_frames;
         vector2u m_cell_size;
         std::size_t m_frame_index;
         duration m_elapsed_frame_time;

         animation(sf::Texture const & sheet, vector2u dimensions, bool loop,
            vector2u offset, std::vector<frame_data> frames) noexcept;

         template<vector2u Dimensions>
         struct frame_validator
         {
               consteval frame_validator(
                  std::initializer_list<frame_data> const & frames)
                  : m_frames{frames}
               {
                  if (m_frames.size() == 0) { throw; }

                  for (auto const & frame : m_frames)
                  {
                     auto const has_valid_cell{
                        frame.m_cell_position.x < Dimensions.x &&
                        frame.m_cell_position.y < Dimensions.y};
                     if (!has_valid_cell) { throw; }

                     using namespace std::chrono_literals;
                     auto const has_valid_duration{frame.m_duration > 0s};
                     if (!has_valid_duration) { throw; }
                  }
               }

               ~frame_validator() = default;

               frame_validator(frame_validator const &) = delete;
               auto operator=(frame_validator const &) = delete;

               std::initializer_list<frame_data> const & m_frames;
         };
   };
}