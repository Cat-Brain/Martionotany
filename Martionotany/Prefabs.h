#pragma once
#include "Window.h"
#include "Input.h"

namespace Prefabs
{
	Prefab testClickable = Prefab({ MeshRenderer(defaultShader, quadMesh),
		Position(), Scale(), Rotation(), PhysicsCircle(),
		MouseInteractable(), DestroyOnInteract(), PointAward(5),
		InteractableColor(vec4(0.6f, 0.7f, 0.5f, 1.f), vec4(0.4f, 0.5f, 0.3f, 1.f), vec4(0.2f, 0.3f, 0.1f, 1.f)) });

	Prefab testEnemy = Prefab({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.7f, 0.9f, 1.f)),
		Position(), Scale(), Rotation(), PhysicsBody(1), PhysicsCircle(),
		MoveToPlayer(1), Health(3), PointAward(5) });

	Prefab testProjectile = Prefab({ MeshRenderer(defaultShader, quadMesh, vec4(0.5f, 0.7f, 0.6f, 1.f)),
		Position(), Scale(), Rotation(), PhysicsBody(1), PhysicsTrigger(), PhysicsCircle(),
		Direction(), SetVelToDir(), Projectile(7.5f, 5, 1) });

	Prefab player = Prefab({ MeshRenderer(defaultShader, quadMesh, vec4(0.6f, 0.8f, 0.4f, 1.f)),
		Health(10), CustomDeath(),
		Position(), Scale(), Rotation(45), PhysicsBody(1), PhysicsCircle() });
}