#include <asset/manager.hpp>

#include <SFML/Graphics/Texture.hpp>

#include <mutex>
#include <unordered_map>

using namespace jrag::asset;

namespace
{
   struct asset_entry
   {
         std::weak_ptr<sf::Texture const> asset;
         manager::texture_future asset_future;
   };
}

struct manager::data
{
      class deleter
      {
         public:
            deleter(
               std::filesystem::path path, std::weak_ptr<data> data_weak_ptr)
               : m_path{std::move(path)}
               , m_data_weak_ptr{std::move(data_weak_ptr)}
            {}

            void operator()(sf::Texture const * asset) const;

         private:
            std::filesystem::path m_path;
            std::weak_ptr<data> m_data_weak_ptr;
      };

      std::mutex texture_map_mutex;
      std::unordered_map<std::filesystem::path, asset_entry> texture_map;
};

void manager::data::deleter::operator()(sf::Texture const * const asset) const
{
   std::unique_ptr<sf::Texture const> const asset_unique_ptr{asset};

   auto const data_shared_ptr{m_data_weak_ptr.lock()};
   if (!data_shared_ptr)
   {
      return; // Asset outlived manager. No data to cleanup.
   }

   using map_type = std::unordered_map<std::filesystem::path, asset_entry>;
   auto clean_entry = [](std::filesystem::path const & path,
                         std::mutex & map_mutex, map_type & map) -> void
   {
      std::lock_guard const lock{map_mutex};
      auto const asset_iterator{map.find(path)};

      if (asset_iterator == map.cend())
      {
         return; // Entry already removed
      }

      if (auto const & entry{asset_iterator->second};
          (!entry.asset.expired()) || entry.asset_future.valid())
      {
         return; // Entry has been re-activated
      }

      map.erase(asset_iterator);
   };

   clean_entry(
      m_path, data_shared_ptr->texture_map_mutex, data_shared_ptr->texture_map);
}

manager::manager()
   : m_data{std::make_unique<data>()}
{}

manager::~manager() = default;

auto manager::get_texture(
   std::filesystem::path const & path) -> expected_texture
{
   // TODO: Make generic version for fonts, music, etc...

   std::promise<expected_texture> asset_promise{};
   asset_entry * entry_ptr{nullptr};

   {
      std::unique_lock lock{m_data->texture_map_mutex};
      auto const [entry_iterator, was_inserted]{m_data->texture_map.try_emplace(
         path, std::weak_ptr<sf::Texture const>{}, asset_promise.get_future())};
      auto & entry{entry_iterator->second};
      entry_ptr = &entry;

      if (!was_inserted)
      {
         if (auto asset_shared_ptr{entry.asset.lock()})
         {
            lock.unlock();
            return asset_shared_ptr; // Asset already loaded
         }

         if (auto const & asset_future{entry.asset_future};
             asset_future.valid())
         {
            auto const asset_future_copy{asset_future};
            lock.unlock();
            return asset_future_copy.get(); // Asset already loading
         }

         // Asset is currently being deallocated and the asset deleter has not
         // removed the entry yet. Update entry to show it is being used again.
         entry.asset_future = asset_promise.get_future();
      }
   }

   auto asset_ptr{std::make_unique<sf::Texture>()};
   auto const was_loaded{asset_ptr->loadFromFile(path)};

   if (!was_loaded)
   {
      std::unexpected const error{error::not_found};
      asset_promise.set_value(error);

      {
         std::lock_guard const lock{m_data->texture_map_mutex};
         m_data->texture_map.erase(path);
      }

      return error;
   }

   std::shared_ptr<sf::Texture const> asset_shared_ptr{
      asset_ptr.release(), data::deleter{path, m_data}};
   asset_promise.set_value(asset_shared_ptr);

   {
      std::lock_guard const lock{m_data->texture_map_mutex};
      auto & entry{*entry_ptr};
      entry.asset = asset_shared_ptr;
      entry.asset_future = texture_future{};
   }

   return asset_shared_ptr;
}

auto manager::get_texture_async(
   std::filesystem::path const & path) -> texture_future
{
   std::unique_lock lock{m_data->texture_map_mutex};
   auto const [entry_iterator, was_inserted]{m_data->texture_map.try_emplace(
      path, std::weak_ptr<sf::Texture const>{}, texture_future{})};
   auto & entry{entry_iterator->second};

   if (!was_inserted)
   {
      if (auto asset_shared_ptr{entry.asset.lock()})
      {
         lock.unlock();
         std::promise<expected_texture> resolved_promise{};
         resolved_promise.set_value(std::move(asset_shared_ptr));
         return resolved_promise.get_future(); // Asset already loaded
      }

      if (auto const & asset_future{entry.asset_future}; asset_future.valid())
      {
         return asset_future; // Asset already loading
      }

      // Asset is currently being deallocated and the asset deleter has not
      // removed the entry yet. Asset must be loaded again.
   }

   auto load_texture =
      [](std::filesystem::path const & path, asset_entry & entry,
         std::shared_ptr<manager::data> const & data) -> expected_texture
   {
      auto asset_ptr{std::make_unique<sf::Texture>()};
      auto const was_loaded{asset_ptr->loadFromFile(path)};

      if (!was_loaded)
      {
         std::lock_guard const lock{data->texture_map_mutex};
         data->texture_map.erase(path);
         return std::unexpected{error::not_found};
      }

      std::shared_ptr<sf::Texture const> asset_shared_ptr{
         asset_ptr.release(), data::deleter{path, data}};

      {
         std::lock_guard const lock{data->texture_map_mutex};
         entry.asset = asset_shared_ptr;
         entry.asset_future = texture_future{};
      }

      return asset_shared_ptr;
   };

   entry.asset_future = std::async(load_texture, path, std::ref(entry), m_data);
   return entry.asset_future;
}
