#include "Layers.h"
#include "editor.h"
namespace Editor
{

  void ParticleSystemLayer::OnAttach()
  {
	  // Initialize resources if needed (e.g., load textures)
  }

  void ParticleSystemLayer::OnDetach()
  {
	  // Cleanup: destroy particle entities if necessary
	  m_ParticleEntities.clear();
  }

  void ParticleSystemLayer::Update()
  {
      float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

      // --- EMITTER UPDATE: Process all ParticleSystem (emitter) components ---
      for (auto& emitterEntity : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem>())
      {
          // Only process active emitters (for example, check the associated Transform)
          auto transform = emitterEntity.GetComponent<Transform>();
          if (!transform || !transform->is_active)
              continue;

          // Access the ParticleSystem (emitter) component
          auto emitter = emitterEntity.GetComponent<ParticleSystem>();

          // Update the emitter’s duration timer
          emitter->duration -= deltaTime;
          if (!emitter->is_looping && emitter->duration <= 0.0f)
          {
              transform->is_active = false;
              continue;
          }

          // --- Emission Rate Handling ---
          // Increase the accumulator based on rate_over_time.
          emitter->emissionAccumulator += deltaTime * emitter->particleEmissionRate.rate_over_time;
          int particlesToEmit = static_cast<int>(emitter->emissionAccumulator);
          // Subtract the integer count from the accumulator.
          emitter->emissionAccumulator -= particlesToEmit;

          // Emit the required number of particles.
          for (int i = 0; i < particlesToEmit; ++i)
          {
              SpawnParticles(emitterEntity);
          }
      }

      // --- PARTICLE UPDATE: Process all Particle components (individual particles) ---
      // Note: We iterate over particle entities, and if a particle’s lifetime expires,
      // we remove it from the scene.
      std::vector<FlexECS::EntityID> m_entities_to_delete;
      for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem::Particle>())
      {
          auto particle = elem.GetComponent<ParticleSystem::Particle>();

          // Update lifetime.
          particle->currentLifetime -= deltaTime;
          if (particle->currentLifetime < -20.0f)
          {
              m_entities_to_delete.push_back(elem.Get());
              continue;
          }
          else if (particle->currentLifetime <= 0.0f) 
          {
              elem.GetComponent<Transform>()->is_active = false;
              continue;
          }


          // Update particle position based on its velocity
          elem.GetComponent<Position>()->position += static_cast<Vector3>(elem.GetComponent<Rigidbody>()->velocity) * deltaTime;

          // Calculate normalized lifetime (0 = expired, 1 = just spawned) -> meant for interpolation of other values
          float normalizedTime = 1.0f - (particle->currentLifetime / particle->totalLifetime);
          //float progress = 1.0f - normalizedTime; // 0 at spawn, 1 at death

          // Interpolate particle properties based on its lifetime.
          particle->currentSpeed = FlexMath::Lerp(particle->start_speed, particle->end_speed, normalizedTime);
          particle->currentSize = FlexMath::Lerp(particle->start_size, particle->end_size, normalizedTime);
          particle->currentColor = Lerp(particle->start_color, particle->end_color, normalizedTime);

          // Update the corresponding sprite component for rendering
          //auto sprite = elem.GetComponent<Sprite>();
          //sprite.color = interpolatedColor;
          elem.GetComponent<Scale>()->scale = Vector3(particle->currentSize, particle->currentSize, particle->currentSize);

          // Optionally: add gravity or drag
          // particle.Velocity += gravity * deltaTime;
      }

      // Destroy All expired particles
      auto scene = FlexECS::Scene::GetActiveScene();
      for (auto entity : m_entities_to_delete)
      {
          scene->DestroyEntity(entity);
          Editor::GetInstance().GetSystem<SelectionSystem>()->DeselectEntity(entity);
      }
  }

  /// Spawns a particle using a pooled inactive particle if available; otherwise, creates a new one.
  void ParticleSystemLayer::SpawnParticles(FlexECS::Entity& emitterEntity) 
  {
      FlexECS::Entity particleEntity;
      bool foundInactive = false;

      for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem::Particle>())
      {
          if (!elem.GetComponent<Transform>()->is_active)
          {
              particleEntity = elem;
              foundInactive = true;
              break;
          }
      }

      if (foundInactive) 
      {
          PoolOldParticles(emitterEntity, particleEntity);
      }
      else 
      {
          CreateNewParticles(emitterEntity);
      }
  }

  void ParticleSystemLayer::CreateNewParticles(FlexECS::Entity& emitterEntity)
  {
      auto emitter = emitterEntity.GetComponent<ParticleSystem>();

      FlexECS::Entity particleEntity = FlexECS::Scene::GetActiveScene()->CreateEntity();
      particleEntity.AddComponent<Position>({ emitterEntity.GetComponent<Position>()->position });
      particleEntity.AddComponent<Rotation>({ Vector3{0,0,0} });
      particleEntity.AddComponent<Scale>({ {emitter->start_size,emitter->start_size,emitter->start_size} });
      particleEntity.AddComponent<Transform>({});
      particleEntity.AddComponent<Sprite>({ emitter->particlesprite_handle, -1 });

      // --- Calculate the initial velocity ---
      Vector3 direction = CalculateInitialVelocity(emitter->particleEmissionShapeIndex);
      if (auto rotationComp = emitterEntity.GetComponent<Rotation>())
      {
          direction = RotateVector(direction, rotationComp->rotation);
      }
      Vector3 velocity = direction * emitter->start_speed;
      particleEntity.AddComponent<Rigidbody>({ velocity, emitter->is_static });

      if (emitter->is_collidable) //PENDING ON PROPER COLLISION COMPONENTS
      {
          particleEntity.AddComponent<BoundingBox2D>({});
          particleEntity.AddComponent<AABB>({});
      }

      // --- Initialize the Particle (runtime state) component ---
      ParticleSystem::Particle particle;
      particle.totalLifetime = emitter->lifetime;
      particle.currentLifetime = emitter->lifetime;
      particle.start_speed = emitter->start_speed;
      particle.end_speed = emitter->end_speed;
      particle.start_size = emitter->start_size;
      particle.end_size = emitter->end_size;
      particle.start_color = emitter->start_color;
      particle.end_color = emitter->end_color;
      // Set initial rendering properties matching the starting state.
      particle.currentSpeed = particle.start_speed;
      particle.currentSize = particle.start_size;
      particle.currentColor = particle.start_color;
      // Add the Particle component to the new entity.
      particleEntity.AddComponent<ParticleSystem::Particle>(particle);
  }

  void ParticleSystemLayer::PoolOldParticles(FlexECS::Entity& emitterEntity, FlexECS::Entity& particleEntity)
  {
      auto emitter = emitterEntity.GetComponent<ParticleSystem>();

      particleEntity.GetComponent<Position>()->position = { emitterEntity.GetComponent<Position>()->position };
      particleEntity.GetComponent<Rotation>()->rotation = { Vector3{0,0,0} };
      particleEntity.GetComponent<Scale>()->scale = Vector3{ emitter->start_size, emitter->start_size, emitter->start_size };
      particleEntity.GetComponent<Transform>()->is_active = true;
      particleEntity.GetComponent<Sprite>()->sprite_handle = emitter->particlesprite_handle;

      // --- Calculate the initial velocity ---
      Vector3 direction = CalculateInitialVelocity(emitter->particleEmissionShapeIndex);
      if (auto rotationComp = emitterEntity.GetComponent<Rotation>())
      {
          direction = RotateVector(direction, rotationComp->rotation);
      }
      Vector3 velocity = direction * emitter->start_speed;
      if (particleEntity.HasComponent<Rigidbody>())
      {
          particleEntity.GetComponent<Rigidbody>()->velocity = velocity;
          particleEntity.GetComponent<Rigidbody>()->is_static = emitter->is_static;
      }
      else
          particleEntity.AddComponent<Rigidbody>({ velocity, emitter->is_static });

      //TODO
      //if (emitter->is_collidable) //PENDING ON PROPER COLLISION COMPONENTS
      //{
      //    particleEntity.AddComponent<BoundingBox2D>({});
      //    particleEntity.AddComponent<AABB>({});
      //}

      // --- Initialize the Particle (runtime state) component ---
      ParticleSystem::Particle particle;
      particleEntity.GetComponent<ParticleSystem::Particle>()->totalLifetime = emitter->lifetime;
      particleEntity.GetComponent<ParticleSystem::Particle>()->currentLifetime = emitter->lifetime;
      particleEntity.GetComponent<ParticleSystem::Particle>()->start_speed = emitter->start_speed;
      particleEntity.GetComponent<ParticleSystem::Particle>()->end_speed = emitter->end_speed;
      particleEntity.GetComponent<ParticleSystem::Particle>()->start_size = emitter->start_size;
      particleEntity.GetComponent<ParticleSystem::Particle>()->end_size = emitter->end_size;
      particleEntity.GetComponent<ParticleSystem::Particle>()->start_color = emitter->start_color;
      particleEntity.GetComponent<ParticleSystem::Particle>()->end_color = emitter->end_color;
      // Set initial rendering properties matching the starting state.
      particleEntity.GetComponent<ParticleSystem::Particle>()->currentSpeed = particle.start_speed;
      particleEntity.GetComponent<ParticleSystem::Particle>()->currentSize = particle.start_size;
      particleEntity.GetComponent<ParticleSystem::Particle>()->currentColor = particle.start_color;
  }

  Vector3  ParticleSystemLayer::CalculateInitialVelocity(const int shape)
  {
      switch (shape)
      {
      case ParticleEmitShape::Sphere:
          // Random direction in any direction.
          return RandomUnitVector();

      case ParticleEmitShape::Hemisphere:
      {
          // Use the sphere generator then restrict to one hemisphere.
          Vector3 v = RandomUnitVector();
          // For example, assume the hemisphere is in the positive Y direction:
          if (v.y < 0.0f)
              v.y = -v.y;
          return v.Normalize();
      }

      case ParticleEmitShape::Cone:
          // For cone shape, assume a fixed cone half-angle (e.g., 30 degrees).
          return RandomUnitVectorCone(30.0f);

      case ParticleEmitShape::Box:
          // For a box, generate a random direction from the cube.
          return RandomUnitVectorCube();

      default:
          // Fallback: default upward direction.
          return Vector3(0.0f, 1.0f, 0.0f);
      }
  }

}
