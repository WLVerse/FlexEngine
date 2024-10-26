// inline functions for Scene class

// Steps:
// 1. Loop through each archetype and check if the archetype has the requested components
// 2. Get the entities from the archetype
template <typename... Ts>
std::vector<FlexEngine::FlexECS::Entity> FlexEngine::FlexECS::Scene::View()
{
  std::vector<Entity> entities;

  // Loop through each archetype and check if the archetype has the requested components
  for (auto& [archetype, archetype_storage] : ARCHETYPE_INDEX)
  {
    // 1. Variadic template unpacking which gives us some types which we need to convert to the ComponentID (std::string), 
    // then push all of it into a vector to get ComponentList.
    std::vector<ComponentID> component_list;
    (component_list.push_back(Reflection::TypeResolver<Ts>::Get()->name), ...);
    
    bool has_name_include{ false };
    for (auto& component : component_list)
    {
      if (component == Reflection::TypeResolver<uint64_t>::Get()->name)
      {
        has_name_include = true;
        break;
      }
    }

    if (!has_name_include)
    {
      component_list.push_back(Reflection::TypeResolver<uint64_t>::Get()->name); // Add the EntityID component
    }
    std::sort(component_list.begin(), component_list.end());

    // 2a. Check if ARCHETYPE_INDEX has an entry, else create archetype
    if (ARCHETYPE_INDEX.count(component_list) == 0)
    {
      // 2a. Create a new archetype with linkage
      Entity::Internal_CreateArchetype(component_list);
    }
    else
    {
      // 2b. If there was already an existing entry, copy its entities to the entities vector.
      entities.insert(entities.end(), ARCHETYPE_INDEX[component_list].entities.begin(), ARCHETYPE_INDEX[component_list].entities.end());
    }

    // 3. Check the edges of the archetype to see if there are any more archetypes obtainable by adding some other unrelated component.
    Archetype& archetype_to_check = ARCHETYPE_INDEX[component_list];
    for (auto& ar : archetype_to_check.edges)
    {
      if (ar.second.add != nullptr)
      {
        entities.insert(entities.end(), ar.second.add->entities.begin(), ar.second.add->entities.end());
      }
    }
  }

  return entities;
}
