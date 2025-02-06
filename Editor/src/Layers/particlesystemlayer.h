#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class ParticleSystemLayer : public FlexEngine::Layer
  {
      // Container for active particle entities
      std::vector<FlexECS::Entity> m_ParticleEntities;

      void CreateNewParticles(FlexECS::Entity& emitterEntity);
      void PoolOldParticles(FlexECS::Entity& emitterEntity, FlexECS::Entity& particleEntity);
  public:
    ParticleSystemLayer() : Layer("Particle System Layer") {}
    ~ParticleSystemLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

    void SpawnParticles(FlexECS::Entity& emitterEntity);

    Vector3 CalculateInitialVelocity(const int shape);
  };

}
