#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class ParticleSystemLayer : public FlexEngine::Layer
  {
      // Container for active particle entities
      std::vector<FlexECS::Entity> m_ParticleEntities;
      //Particles should contain a sprite, and particle System component,
      //can have rigidbody and collision component attached if 
  public:
    ParticleSystemLayer() : Layer("Particle System Layer") {}
    ~ParticleSystemLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

    void EmitParticles(float deltaTime);
  };

}
