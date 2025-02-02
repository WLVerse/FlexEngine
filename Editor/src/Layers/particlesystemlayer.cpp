#include "Layers.h"

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

      // Emit new particles
      for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem>())
      {
          if (!elem.GetComponent<Transform>()->is_active) continue;

          auto ParticleSys = elem.GetComponent<ParticleSystem>();
          ParticleSys->duration -= deltaTime;
          if (!ParticleSys->is_looping)
          {
              elem.GetComponent<Transform>()->is_active = false;
              continue;
          }

          EmitParticles(deltaTime);
      }

      // Update existing particles
      for (auto it = m_ParticleEntities.begin(); it != m_ParticleEntities.end(); )
      {
          FlexECS::Entity entity = *it;
          auto& particle = entity.GetComponent<ParticleSystem>();

          // Update lifetime
          particle.Lifetime -= deltaTime;
          if (particle.Lifetime <= 0.0f)
          {
              // Remove the particle from the scene (and thus from ECS)
              entity.GetScene()->DestroyEntity(entity);
              it = m_ParticleEntities.erase(it);
              continue;
          }

          // Update particle position based on its velocity
          particle.Position += particle.Velocity * deltaTime;

          // Calculate normalized lifetime (0 = expired, 1 = just spawned)
          float normalizedLifetime = particle.Lifetime / particle.StartLifetime;
          float progress = 1.0f - normalizedLifetime; // 0 at spawn, 1 at death

          // Interpolate color and size
          glm::vec4 interpolatedColor = Lerp(particle.StartColor, particle.EndColor, progress);
          float interpolatedSize = Lerp(particle.StartSize, particle.EndSize, progress);

          // Update the corresponding sprite component for rendering
          auto& sprite = entity.GetComponent<Sprite>();
          sprite.position = particle.Position;
          sprite.color = interpolatedColor;
          sprite.size = interpolatedSize;

          // Optionally: add gravity or drag
          // particle.Velocity += gravity * deltaTime;

          ++it;
      }
  }

  void ParticleSystemLayer::EmitParticles(float deltaTime)
  {
      Particle particle{};
      particle.Position = glm::vec2(0.0f, 0.0f); // Emitter's position
      particle.Velocity = glm::vec2(RandomRange(-1.0f, 1.0f), RandomRange(0.5f, 2.0f));
      particle.StartLifetime = particle.Lifetime = 2.0f; // Lifetime of 2 seconds
      particle.StartColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
      particle.EndColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f); // Fade out to transparent
      particle.StartSize = 1.0f;
      particle.EndSize = 0.5f;

      // Create a new entity for the particle in the scene
      FlexECS::Entity entity = GetScene()->CreateEntity();
      entity.AddComponent<ParticleProperties>(particle);

      // Create and set up the sprite component
      Sprite sprite;
      sprite.sprite_handle = "particle_texture"; // Reference to a texture identifier
      sprite.center_aligned = true;
      sprite.position = particle.Position;
      sprite.color = particle.StartColor;
      sprite.size = particle.StartSize;
      entity.AddComponent<Sprite>(sprite);

      // Keep track of this particle for updating
      m_ParticleEntities.push_back(entity);
  }
}
