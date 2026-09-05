#pragma once

/* [DemoConfig]
* Shared constants for the collider demo project.
*/
namespace DemoConfig
{
	constexpr int WindowWidth = 1280;
	constexpr int WindowHeight = 720;

	constexpr float SpawnOffsetX = 450.0f;   // spawn distance from center (left/right)
	constexpr float MoveSpeed = 450.0f;      // keyboard-controlled move speed (px/sec)

	constexpr float CircleRadius = 153.0f;   // half of Circle.png (306x306), native/unscaled
	constexpr float BoxSize = 306.0f;        // Box.png (306x306) native size, unscaled

	// Applied as Transform scale on the Circle/Box demo objects (scenes 1-3).
	// BoxCollider::size / CircleCollider::radius are multiplied by this same scale
	// (see BoxCollider::UpdateBounds), so the collider shrinks along with the sprite.
	constexpr float ColliderObjectScale = 2.0f / 3.0f;
}
