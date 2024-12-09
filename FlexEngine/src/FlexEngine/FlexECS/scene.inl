// inline functions for Scene class

// Steps:
// 1. Loop through each archetype and check if the archetype has the requested components
// 2. Get the entities from the archetype
template <typename... Ts>
std::vector<FlexEngine::FlexECS::Entity> FlexEngine::FlexECS::Scene::View()
{
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
