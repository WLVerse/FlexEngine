/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// physicssystem.h
//
// Simple AABB Physics system
//
// AUTHORS
// [90%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
// [10%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Added some code for mouse over detection
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "physicssystem.h"
#include "FlexECS/enginecomponents.h"

#include <vector>
#include <cmath>
#include <algorithm>

#include "application.h"

namespace FlexEngine
{
	std::vector<std::pair<FlexECS::Entity, FlexECS::Entity>> PhysicsSystem::collisions {};


	/*!***************************************************************************
	* @brief
	* Recalculates the AABB bounding box for an entity.
	* @param entity
	* The entity to recalculate for.
	******************************************************************************/
	void PhysicsSystem::RecomputeBounds(FlexECS::Entity entity)
	{
		if (entity.HasComponent<Sprite>())
		{
			// Need to take into account the model scaling, which can be obtained from sprite
			auto& sprite_scale = entity.GetComponent<Sprite>()->model_matrix;
      auto& position = entity.GetComponent<Position>()->position;
      auto& scale = entity.GetComponent<Scale>()->scale;
      auto& size = entity.GetComponent<BoundingBox2D>()->size;

      entity.GetComponent<BoundingBox2D>()->max.x = position.x + scale.x / 2 * size.x * sprite_scale[0];
      entity.GetComponent<BoundingBox2D>()->max.y = position.y + scale.y / 2 * size.y * sprite_scale[5];
      entity.GetComponent<BoundingBox2D>()->min.x = position.x - scale.x / 2 * size.x * sprite_scale[0];
      entity.GetComponent<BoundingBox2D>()->min.y = position.y - scale.y / 2 * size.y * sprite_scale[5];
		}
		else
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
	* Updates Positions of all rigidbodies based on their velocity.
	******************************************************************************/
	void PhysicsSystem::UpdatePositions()
	{
		
		float dt = Application::GetCurrentWindow()->GetFramerateController().GetFixedDeltaTime();
		for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Rigidbody>())
		{
			auto& velocity = entity.GetComponent<Rigidbody>()->velocity;
			auto& position = entity.GetComponent<Position>()->position;

			position.x += velocity.x * dt;
			position.y += velocity.y * dt;
		}

		// Reset collision data from the previous frame
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<BoundingBox2D>())
    {
      entity.GetComponent<BoundingBox2D>()->is_colliding = false;
    }
	}
	
	/*!***************************************************************************
	* @brief
	* Recalculates the AABB bounding box for all entities.
	******************************************************************************/
	void PhysicsSystem::UpdateBounds()
	{
		for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, BoundingBox2D>())
		{
      RecomputeBounds(entity);
		}
	}

	/*!***************************************************************************
	* @brief
	* Finds all collisions between rigidbodies.
  * Checks if the mouse is over the entity. (Wen Loong)
	* If Rigidbody is static, skips the check.
	******************************************************************************/
	void PhysicsSystem::FindCollisions()
	{
		if (CameraManager::has_main_camera)
		{
			// mouse over detection
			//Convert raw_mouse to world space;
			float app_width = static_cast<float>(Application::GetCurrentWindow()->GetWidth());
			float app_height = static_cast<float>(Application::GetCurrentWindow()->GetHeight());
			auto raw_mouse = Input::GetMousePosition();
			Vector2 ndc_click_pos = { (2 * raw_mouse.x / app_width) - 1, 1 - 2 * raw_mouse.y / app_height };
			Matrix4x4 inverse = CameraManager::GetMainGameCamera()->GetProjViewMatrix().Inverse();
			Vector4 clip = { ndc_click_pos.x,
											 ndc_click_pos.y,
											 1.0f,
											 1 };
			Vector4 mouse_world_pos = inverse * clip;

			for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, BoundingBox2D>())
			{
				auto& bb = *entity.GetComponent<BoundingBox2D>();

				bb.is_mouse_over_cached = bb.is_mouse_over;

				auto& max = bb.max;
				auto& min = bb.min;
			
				bb.is_mouse_over = mouse_world_pos.x > min.x && mouse_world_pos.x < max.x && 
													 mouse_world_pos.y > min.y && mouse_world_pos.y < max.y;
			}
		}


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
	void PhysicsSystem::ResolveCollisions()
	{
		//float dt = FlexEngine::Application::GetCurrentWindow()->GetFixedDeltaTime();
		for (auto collision : collisions)
		{
			//update status of collision
      collision.first.GetComponent<BoundingBox2D>()->is_colliding = true; 
      collision.second.GetComponent<BoundingBox2D>()->is_colliding = true; 

			//auto& a_velocity = collision.first.GetComponent<Rigidbody>()->velocity;
			auto& a_position = collision.first.GetComponent<Position>()->position;
			//auto& b_velocity = collision.second.GetComponent<Rigidbody>()->velocity;
			auto& b_position = collision.second.GetComponent<Position>()->position;

			auto& a_max = collision.first.GetComponent<BoundingBox2D>()->max;
			auto& a_min = collision.first.GetComponent<BoundingBox2D>()->min;
			auto& b_max = collision.second.GetComponent<BoundingBox2D>()->max;
			auto& b_min = collision.second.GetComponent<BoundingBox2D>()->min;

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


	void PhysicsSystem::UpdatePhysicsSystem()
	{
    // Update physics system based on the number of steps
    // for (unsigned int step = 0; step < Application::GetCurrentWindow()->GetFramerateController().GetNumberOfSteps(); ++step)
    {
      UpdatePositions();
      UpdateBounds();
      FindCollisions();
      ResolveCollisions();
    }
	}
}