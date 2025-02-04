/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// physicssystem.h
//
// Simple AABB Physics system
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "FlexEngine.h"
using namespace FlexEngine;

namespace FlexEngine
{
	/*
	Just runs the physics system.
	Needs: 
	> Rigidbody component on the entity
	> Position, Scale
	> Only supports BoundingBox2D "AABB collider" right now 
	
	- Updates positions based on rigidbody.velocity
	- Finds and resolves collisions
		- No elasticity or anything, just pushes back the 
			minimum amount to avoid overlap

	Note:
	Positions are still according to FlexEngine's top left (0,0)
	so our max min will be topleft botright
	*/
  class __FLX_API PhysicsSystem
  {
  public:
		static void UpdatePhysicsSystem();

  private:
		static void RecomputeBounds(FlexECS::Entity entity);
		static void UpdatePositions();
		static void UpdateBounds();
		static void FindCollisions();
		static void ResolveCollisions();

		static std::vector<std::pair<FlexECS::Entity, FlexECS::Entity>> collisions;
  };
}