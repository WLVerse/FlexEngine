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

#include "physicssystem.h"
#include "Components/physics.h"
#include "Components/rendering.h"	//position, scale, rotate still over here

#include <FlexEngine.h>

#include <vector>
#include <cmath>
#include <algorithm>


namespace ChronoDrift
{
	std::vector<std::pair<FlexECS::Entity, FlexECS::Entity>> collisions {};


	/*!***************************************************************************
	* @brief
	* Recalculates the AABB bounding box for an entity.
	* @param entity
	* The entity to recalculate for.
	******************************************************************************/
	void RecomputeBounds(FlexECS::Entity entity) 
	{
		auto& position = entity.GetComponent<Position>()->position;
		auto& scale = entity.GetComponent<Scale>()->scale;
		auto& size = entity.GetComponent<BoundingBox2D>()->size;
		entity.GetComponent<BoundingBox2D>()->max.x = position.x + scale.x / 2 * size.x;
		entity.GetComponent<BoundingBox2D>()->max.y = position.y + scale.y / 2 * size.y;
		entity.GetComponent<BoundingBox2D>()->min.x = position.x - scale.x / 2 * size.x;
		entity.GetComponent<BoundingBox2D>()->min.y = position.y - scale.y / 2 * size.y;
	}


	/*!***************************************************************************
	* @brief
	* Updates Positions of all rigidbodies based on their velocity.
	******************************************************************************/
	void UpdatePositions() 
	{
		float dt = FlexEngine::Application::GetCurrentWindow()->GetDeltaTime();
		for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Rigidbody>())
		{
			auto& velocity = entity.GetComponent<Rigidbody>()->velocity;
			auto& position = entity.GetComponent<Position>()->position;
			entity.GetComponent<Transform>()->is_dirty = true;

			position.x += velocity.x * dt;
			position.y += velocity.y * dt;
		}

		// Reset collision data from the previous frame
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<BoundingBox2D>())
    {
      entity.GetComponent<BoundingBox2D>()->isColliding = false;
    }
	}
	
	/*!***************************************************************************
	* @brief
	* Recalculates the AABB bounding box for all entities.
	******************************************************************************/
	void UpdateBounds()
	{
		for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Rigidbody, BoundingBox2D>())
		{
			auto& position = entity.GetComponent<Position>()->position;
			auto& scale = entity.GetComponent<Scale>()->scale;
			auto& size = entity.GetComponent<BoundingBox2D>()->size;
			entity.GetComponent<BoundingBox2D>()->max.x = position.x + scale.x / 2 * size.x;
			entity.GetComponent<BoundingBox2D>()->max.y = position.y + scale.y / 2 * size.y;
			entity.GetComponent<BoundingBox2D>()->min.x = position.x - scale.x / 2 * size.x;
			entity.GetComponent<BoundingBox2D>()->min.y = position.y - scale.y / 2 * size.y;
		}
	}

	/*!***************************************************************************
	* @brief
	* Finds all collisions between rigidbodies.
	* If Rigidbody is static, skips the check.
	******************************************************************************/
	void FindCollisions() 
	{
		collisions.clear();
		
		for (auto& entity_a : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Rigidbody, BoundingBox2D>())
		{
			if(entity_a.GetComponent<Rigidbody>()->is_static) continue;
			//construct aabb
			auto& max_a = entity_a.GetComponent<BoundingBox2D>()->max;
			auto& min_a = entity_a.GetComponent<BoundingBox2D>()->min;
			
			for (auto& entity_b : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Rigidbody, BoundingBox2D>())
			{
				if (entity_a == entity_b) continue;

				auto& max_b = entity_b.GetComponent<BoundingBox2D>()->max;
				auto& min_b = entity_b.GetComponent<BoundingBox2D>()->min;

				//AABB check
				if (max_a.x < min_b.x || max_a.y < min_b.y || min_a.x > max_b.x || min_a.y > max_b.y) continue;
				else collisions.push_back({ entity_a, entity_b });
			}
		}

	}

	/*!***************************************************************************
	* @brief
	* Adds pushback for colliding entities to make sure they don't overlap.
	******************************************************************************/
	void ResolveCollisions() 
	{
		//float dt = FlexEngine::Application::GetCurrentWindow()->GetDeltaTime();
		for (auto collision : collisions)
		{
			//update status of collision
      collision.first.GetComponent<BoundingBox2D>()->isColliding = true; 
      collision.second.GetComponent<BoundingBox2D>()->isColliding = true; 

			//auto& a_velocity = collision.first.GetComponent<Rigidbody>()->velocity;
			auto& a_position = collision.first.GetComponent<Position>()->position;
			//auto& b_velocity = collision.second.GetComponent<Rigidbody>()->velocity;
			auto& b_position = collision.second.GetComponent<Position>()->position;

			auto& a_max = collision.first.GetComponent<BoundingBox2D>()->max;
			auto& a_min = collision.first.GetComponent<BoundingBox2D>()->min;
			auto& b_max = collision.second.GetComponent<BoundingBox2D>()->max;
			auto& b_min = collision.second.GetComponent<BoundingBox2D>()->min;

			collision.first.GetComponent<Transform>()->is_dirty = true;
			collision.second.GetComponent<Transform>()->is_dirty = true;

			//Check if already resolved
			if (a_max.x < b_min.x || a_max.y < b_min.y || a_min.x > b_max.x || a_min.y > b_max.y) continue;


			Vector2 normal = a_position - b_position;

			const float a_half_width = (a_max.x - a_min.x) / 2.0f;
			const float b_half_width = (b_max.x - b_min.x) / 2.0f;
			const float a_half_height = (a_max.y - a_min.y) / 2.0f;
			const float b_half_height = (b_max.y - b_min.y) / 2.0f;

			const float x_penetration = a_half_width + b_half_width - std::abs(normal.x);
			const float y_penetration = a_half_height + b_half_height - std::abs(normal.y);

			//find shortest collision side 
			const float left = a_max.x - b_min.x;
			const float right = b_max.x - a_min.x;
			const float up = a_max.y - b_min.y;
			const float down = b_max.y - a_min.y;
			const float largest = std::min({ left, right, up, down });

			if (!(collision.first.GetComponent<Rigidbody>()->is_static))
			{
				if (largest == left) {
					a_position.x -= x_penetration;
				}
				else if (largest == right) {
					a_position.x += x_penetration;
				}
				else if (largest == up) {
					a_position.y -= y_penetration;
				}
				else if (largest == down) {
					a_position.y += y_penetration;
				}
				RecomputeBounds(collision.first);
			}
			
			if (!(collision.second.GetComponent<Rigidbody>()->is_static))
			{
				if (largest == left) {
					b_position.x += x_penetration;
				}
				else if (largest == right) {
					b_position.x -= x_penetration;
				}
				else if (largest == up) {
					b_position.y += y_penetration;
				}
				else if (largest == down) {
					b_position.y -= y_penetration;
				}
				RecomputeBounds(collision.second);
			}
		}
	}


	void UpdatePhysicsSystem()
	{
		UpdatePositions();
		UpdateBounds();
		FindCollisions();
		ResolveCollisions();
	}
}