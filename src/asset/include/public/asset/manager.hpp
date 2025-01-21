#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <future>
#include <memory>

namespace sf
{
   class Texture;
}

namespace jrag::asset
{
   class manager final
   {
      public:
         manager();
         ~manager();

         manager(manager const &) = delete;
         auto operator=(manager const &) -> manager & = delete;

         enum class error : std::uint8_t
         {
            not_found,
         };

         using expected_texture =
            std::expected<std::shared_ptr<sf::Texture const>, manager::error>;
         [[nodiscard]] auto get_texture(
            std::filesystem::path const & path) -> expected_texture;

         using texture_future = std::shared_future<expected_texture>;
         [[nodiscard]] auto get_texture_async(
            std::filesystem::path const & path) -> texture_future;

      private:
         struct data;
         std::shared_ptr<data> m_data;
   };
}