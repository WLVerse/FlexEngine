// WLVERSE [https://wlverse.web.app]
// scene.inl
//
// Implementation of inline functions for the scene class. These are for querying
// entities of a certain component list. 
// YC : I could definitely optimize the vector to not be copied around so much.
// I'll look into it. 
//
// AUTHORS
// [50%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Query implementation
// [50%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//   - Cached query implementation, and comments because someone hates green, or something, lmao.
//
// Copyright (c) 2025 DigiPen, All rights reserved.

/*!
  \brief Performs a query that returns a list of entities that have the requested components, but assumes that you don't want to cache it as it is volatile.

         If you are going to perform the same query multiple times (ie in a for loop), it is better to use CachedQuery instead of Query.

  \param Ts... The packed component list of the components you want to query for
  \return A copy of the list of entities that have the requested components
*/
template <typename... Ts>
std::vector<FlexEngine::FlexECS::Entity> FlexEngine::FlexECS::Scene::Query()
{
  // Steps:
  // 1. Loop through each archetype and check if the archetype has the requested components
  // 2. Get the entities from the archetype
  std::vector<Entity> entities;

  // 1. Loop through each archetype and check if the archetype has the requested components
  for (auto& [archetype, archetype_storage] : ARCHETYPE_INDEX)
  {
    // find the type in the archetype
    bool has_requested_components = (
      (std::find(
        archetype_storage.type.begin(),
        archetype_storage.type.end(),
        Reflection::TypeResolver<Ts>::Get()->name
      ) != archetype_storage.type.end()) && ...
    );

    // check if the archetype has the requested components
    if (has_requested_components)
    {
      // 2. Get the entities from the archetype
      entities.insert(entities.end(), archetype_storage.entities.begin(), archetype_storage.entities.end());
    }
  }

  return entities;
}

/*
  \brief Performs a query that returns a pre-cached list of entities that have the requested components.

         If the query has not been performed before, it will perform the query and cache the result.

  \note Now you might say, why cache the query result? Well, if the query result isn't going to change, why keep querying?
        This only really happens if you add or remove components from the entities, resulting in a completely new archetype.
        This archetype, well maybe then it might not have the requested components, so the query result would be different, but that's rare, and a one time thing.

        So, if you are going to perform the same query multiple times, it is better to cache the result and return the cached result instead of
        performing the query multiple times.

        We could always create a archetype map that fully covers every connection, but computing that would literally run 10^15 times just for like 20+ components
        Clearly that isn't viable...
        So following the principles of pick 2 of the 3 to save - speed, memory and maintainability we choose to take speed and maintainability

  \param Ts... The packed component list of the components you want to query for
  \return A copy of the list of entities that have the requested components
*/
template <typename... Ts>
std::vector<FlexEngine::FlexECS::Entity> FlexEngine::FlexECS::Scene::CachedQuery()
{
  // Check if the query exists
  std::vector<std::string> component_id_list = { Reflection::TypeResolver<Ts>::Get()->name... };
  std::sort(component_id_list.begin(), component_id_list.end());

  if (query_cache.find(component_id_list) != query_cache.end())
  {
    return query_cache[component_id_list].Get();
  }

  // If no such query exists, perform archetype searching like Query, but store the archetype pointer instead
  std::vector<std::vector<FlexEngine::FlexECS::Entity>*> ptr_to_entities;
  for (auto& [archetype, archetype_storage] : ARCHETYPE_INDEX)
  {
    // find the type in the archetype
    bool has_requested_components = (
      (std::find(
        archetype_storage.type.begin(),
        archetype_storage.type.end(),
        Reflection::TypeResolver<Ts>::Get()->name
      ) != archetype_storage.type.end()) && ...
    );

    // check if the archetype has the requested components
    if (has_requested_components)
    {
      // 2. Capture pointer to archetype's entity vector
      std::vector<FlexEngine::FlexECS::Entity>* ptr = reinterpret_cast<std::vector<FlexEngine::FlexECS::Entity>*>(&archetype_storage.entities);
      ptr_to_entities.push_back(ptr);
    }
  }
  query_cache.emplace(component_id_list, ptr_to_entities);

  return query_cache[component_id_list].Get();
}