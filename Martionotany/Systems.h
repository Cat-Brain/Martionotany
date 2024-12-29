#pragma once
#include "Prefabs.h"

#pragma region Awake

SYSTEM(EnableAwakens, enableAwakens, { COMP_REQ(, , (NotEnabledOnAwaken), , AwakeEntityEval) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity.entity->enabled = true;
}

SYSTEM(SetProjectileVelToDir, setProjectileVelToDir, { COMP_REQ((PhysicsBody)(Direction)(Projectile), , , , AwakeEntityEval) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity[0].physicsBody.vel = entity[1].direction.dir * entity[2].projectile.speed;
}

// Must be final awake method as it sets entities to know that they are awake
SYSTEM(SilenceAwakens, silenceAwakens, { COMP_REQ(, , , , AwakeEntityEval) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity.entity->firstFrame = false;
}

#pragma endregion

#pragma region Early Physics

SYSTEM(UpdateGravity, updateGravity, { COMP_REQ((PhysicsBody)(Gravity)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		PhysicsBody& body = entity[0];
		Gravity& gravity = entity[1];

		body.vel += deltaTime * defaultGravity * gravity.multiplier * gravity.direction;
	}
}

#pragma endregion

#pragma region Mouse stuff

SYSTEM(ReadMouse, readMouse, { }, Update)
{
	using namespace Input;
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	pixMousePos = vec2(xPos, yPos);
	screenMousePos = vec2(pixMousePos.x / (screenDim.x - 1), 1 - pixMousePos.y / (screenDim.y - 1));
}

SYSTEM(UpdateCameraMouse, updateCameraMouse, { COMP_REQ((Camera)(CameraMouse)(Position)) }, Update)
{ // FIX THIS SYSTEM!!!
	for (ProcEntity& entity : components[0])
	{
		Camera& camera = entity[0];
		CameraMouse& mouse = entity[1];
		Position& position = entity[2];

		mouse.camMousePos = camera.ScreenToCameraSpace(Input::screenMousePos); // Untested but issue probably stems here or ReadMouse
		mouse.worldMousePos = mouse.camMousePos + position.pos;
		// << mouse.camMousePos << " -> " << mouse.worldMousePos << '\n';
		mouse.gridMousePos = ToGrid(mouse.camMousePos + ToGridCentered(position.pos));
	}
}

SYSTEM(MouseXPhysicsCircle, mouseXPhysicsCircle, SysReq({ COMP_REQ((CameraMouse)),
	COMP_REQ((MouseInteractable)(PhysicsCircle)(Position)) }), Update)
{
	for (ProcEntity& mouseEntity : components[0])
	{
		CameraMouse& mouse = mouseEntity[0];
		for (ProcEntity& physicsEntity : components[1])
			physicsEntity[0].mouseInteractable.Update(physicsEntity[1].physicsCircle.Overlaps(
				mouse.gridMousePos - physicsEntity[2].position.pos));
	}
}

SYSTEM(MouseXPhysicsBox, mouseXPhysicsBox, SysReq({ COMP_REQ((CameraMouse)),
	COMP_REQ((MouseInteractable)(PhysicsBox)(Position)(Rotation)) }), Update)
{
	for (ProcEntity& mouseEntity : components[0])
	{
		CameraMouse& mouse = mouseEntity[0];
		for (ProcEntity& physicsEntity : components[1])
			physicsEntity[0].mouseInteractable.Update(physicsEntity[1].physicsBox.Overlaps(
				glm::rotate(mouse.gridMousePos - physicsEntity[2].position.pos, physicsEntity[3].rotation.rotation)));
	}
}

SYSTEM(UpdateInteractableColors, updateInteractableColors,
	{ COMP_REQ((InteractableColor)(MouseInteractable)(MeshRenderer)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		InteractableColor& interactableColor = entity[0];
		MouseInteractable& mouseInteractable = entity[1];
		MeshRenderer& meshRenderer = entity[2];

		if (mouseInteractable.held)
			meshRenderer.color = interactableColor.held;
		else if (mouseInteractable.hovered)
			meshRenderer.color = interactableColor.hovered;
		else
			meshRenderer.color = interactableColor.normal;
	}
}

SYSTEM(TestPrint, testPrint, { COMP_REQ(, (MouseInteractable)(DestroyOnInteract), , (Health), OnReleaseEntityEval)}, Update)
{
	for (ProcEntity& entity : components[0])
	{
		if (entity.ComponentFound(0))
			entity[0].health.health--;
		else
			entity.entity->toDestroy = true;
	}
}

SYSTEM(UpdateFollowCursor, updateFollowCursor, { COMP_REQ((FollowCursor)(Position)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		FollowCursor& followCursor = entity[0];
		Position& position = entity[1];

		position.pos = followCursor.mouse->worldMousePos;
	}
}

#pragma endregion

#pragma region Game Stuff

SYSTEM(PlayerMove, playerMove, { COMP_REQ((PhysicsBody)(Player)(Rotation), , (PlayerJump)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		PhysicsBody& physicsBody = entity[0];
		Player& player = entity[1];
		Rotation& rotation = entity[2];

		vec2 inp(0);
		if (Input::d.held)
			inp.x++;
		if (Input::a.held)
			inp.x--;
		if (Input::w.held)
			inp.y++;
		if (Input::s.held)
			inp.y--;

		if (inp == vec2(0))
			physicsBody.vel = TrySubVec2(physicsBody.vel, deltaTime * player.accel);
		else
			physicsBody.vel = TryAddVec2(physicsBody.vel, normalize(inp) * deltaTime * player.accel, player.speed);
	}
}

SYSTEM(PlayerMoveJump, playerMoveJump, { COMP_REQ((PhysicsBody)(Player)(PlayerJump)(Rotation)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		PhysicsBody& physicsBody = entity[0];
		Player& player = entity[1];
		PlayerJump& playerJump = entity[2];
		Rotation& rotation = entity[3];

		float inp = 0;
		if (Input::d.held)
			inp++;
		if (Input::a.held)
			inp--;

		float mult = (Input::sprint.held ? 2 : 1) * (Input::walk.held ? 0.5f : 1) * (Input::halt.held ? 0 : 1);

		if (inp == 0)
			physicsBody.vel.x = TrySubF(physicsBody.vel.x, deltaTime * player.accel);
		else
			physicsBody.vel.x = TryAddF(physicsBody.vel.x, mult * inp * deltaTime * player.accel, mult * player.speed);

		if (Input::jump.pressed)
			physicsBody.vel.y += playerJump.jumpForce;

		rotation.rotation += deltaTime * 45 * mult;
	}
}

SYSTEM(EnemyMoveToPlayer, enemyMoveToPlayer, SysReq({ COMP_REQ((MoveToPlayer)(PhysicsBody)(Position)), COMP_REQ((Position), (Player)) }), Update)
{
	if (components[1].size() <= 0)
		return;

	for (ProcEntity& enemy : components[0])
	{
		MoveToPlayer& move = enemy[0];
		PhysicsBody& body = enemy[1];
		Position& position = enemy[2];

		Position* nearestPlayer = &components[1][0][0].position;
		for (int i = 1; i < components[1].size(); i++)
		{
			Position& currentPosition = components[1][i][0];
			if (nearestPlayer->Distance2(position) > currentPosition.Distance2(position))
				nearestPlayer = &currentPosition;
		}

		body.vel = normalize(nearestPlayer->pos - position.pos);
	}
}

SYSTEM(TestSpawnEnemies, testSpawnEnemies, { COMP_REQ((CameraMouse)) }, Update)
{
	if (!Input::click2.pressed)
		return;

	for (ProcEntity& entity : components[0])
	{
		CameraMouse& mouse = entity[0];
		ECS::AddEntity(Prefabs::testEnemy.Clone({ Position(mouse.gridMousePos) }));
	}
}

SYSTEM(TestShootProjectiles, testShootProjectiles, SysReq({ COMP_REQ((CameraMouse)), COMP_REQ((Position), (Player))}), Update)
{
	if (!Input::click1.pressed)
		return;

	for (ProcEntity& mouseEntity : components[0])
	{
		CameraMouse& mouse = mouseEntity[0];
		for (ProcEntity& playerEntity : components[1])
		{
			Position& position = playerEntity[0];
			ECS::AddEntity(Prefabs::testProjectile.Clone({ Position(position.pos),
				Direction(SNormalize(mouse.worldMousePos - position.pos)) }, { PhysicsBodyIgnore({ playerEntity.entity }) }));
		}
	}
}

SYSTEM(UpdateProjectileDuration, updateProjectileDuration, { COMP_REQ((Projectile)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Projectile& projectile = entity[0];
		projectile.duration -= deltaTime;
		if (projectile.duration <= 0)
			entity.entity->toDestroy = true;
	}
}

SYSTEM(UpdatePlayerPoints, updatePlayerPoints, { COMP_REQ((PlayerPoints)(NumberRenderer)) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity[1].numberRenderer.number = entity[0].playerPoints.points;
}

SYSTEM(DestroyTheDead, destroyTheDead, { COMP_REQ((Health), , , (CustomDeath)) }, Update)
{
	for (ProcEntity& entity : components[0])
		if (entity[0].health.health <= 0)
		{
			if (entity.ComponentFound(1))
				entity[1].customDeath.dying = true;
			else
				entity.entity->toDestroy = true;
		}
}

#pragma endregion

#pragma region Physics

SYSTEM(ClearTriggerHitLists, clearTriggerHitLists, { COMP_REQ((PhysicsTrigger)) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity[0].physicsTrigger.lastHit = nullptr;
}

SYSTEM(UpdatePhysicsBodies, updatePhysicsBodies, { COMP_REQ((Position)(PhysicsBody)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Position& pos = entity[0];
		PhysicsBody& body = entity[1];

		pos.pos += body.vel * deltaTime;
	}
}

SYSTEM(UpdateCirclesXInfiniteWalls, updateCirclesXInfiniteWalls, SysReq({ COMP_REQ((InfinitePhysicsWall)),
	COMP_REQ((Position)(PhysicsBody)(PhysicsCircle), , (PhysicsTrigger)) }), Update)
{
	for (ProcEntity& wallEntity : components[0])
	{
		InfinitePhysicsWall& wall = wallEntity[0];
		for (ProcEntity& circleEntity : components[1])
		{
			Position& pos = circleEntity[0];
			PhysicsBody& body = circleEntity[1];
			PhysicsCircle& circle = circleEntity[2];

			float height = glm::dot(wall.normal, pos.pos) - circle.radius;
			if (height > wall.height)
				continue;

			if (circleEntity.ComponentFound(3))
			{
				circleEntity[3].physicsTrigger.lastHit = wallEntity.entity;
				continue;
			}
			pos.pos += (wall.height - height) * wall.normal;
			body.vel -= min(0.f, glm::dot(wall.normal, body.vel)) * wall.normal;
		}
	}
}

SYSTEM(UpdateAABBsXInfiniteWalls, updateAABBsXInfiniteWalls, SysReq({ COMP_REQ((InfinitePhysicsWall)),
	COMP_REQ((Position)(Rotation)(PhysicsBody)(PhysicsBox)) }), Update)
{
	for (ProcEntity& wallEntity : components[0])
	{
		InfinitePhysicsWall& wall = wallEntity[0];
		for (ProcEntity& circleEntity : components[1])
		{
			Position& pos = circleEntity[0];
			Rotation& rotation = circleEntity[1];
			PhysicsBody& body = circleEntity[2];
			PhysicsBox& box = circleEntity[3];

			vec2 right = vec2(cos(rotation.rotation), sin(rotation.rotation));
			vec2 up = vec2(-right.y, right.x);

			vec2 corners[4] = {
				pos.pos + box.dimensions * -0.5f,
				pos.pos + box.dimensions * vec2(-0.5f, 0.5f),
				pos.pos + box.dimensions * vec2(0.5f, -0.5f),
				pos.pos + box.dimensions * 0.5f
			};
			
			float height = 99999;
			for (int i = 0; i < 4; i++)
				height = min(height, glm::dot(wall.normal, corners[i]));
			if (height > wall.height)
				continue;

			if (circleEntity.ComponentFound(3))
			{
				circleEntity[3].physicsTrigger.lastHit = wallEntity.entity;
				continue;
			}

			pos.pos += (wall.height - height) * wall.normal;
			body.vel -= min(0.f, glm::dot(wall.normal, body.vel)) * wall.normal;
		}
	}
}

SYSTEM(UpdateCirclesXCircles, updateCirclesXCircles, { COMP_REQ((Position)(PhysicsBody)(PhysicsCircle), , , (PhysicsBodyIgnore)(PhysicsTrigger)) }, Update)
{
	for (int i = 0; i < (int)components[0].size() - 1; i++)
		for (int j = i + 1; j < (int)components[0].size(); j++)
		{
			bool found = false;
			for (int index : { i, j })
			{
				if (components[0][index].ComponentFound(3))
					for (Entity* toIgnore : entityStorage.Get(components[0][index][3].physicsBodyIgnore.storageIndex))
						if (toIgnore == components[0][i + j - index].entity)
						{
							found = true;
							break;
						}
				if (found)
					break;
			}
			if (found) continue;

			Position& posA = components[0][i][0];
			Position& posB = components[0][j][0];

			PhysicsBody& bodyA = components[0][i][1];
			PhysicsBody& bodyB = components[0][j][1];

			PhysicsCircle& circleA = components[0][i][2];
			PhysicsCircle& circleB = components[0][j][2];
			
			float desiredDistance = circleA.radius + circleB.radius;
			if (posA.pos != posB.pos && glm::distance2(posA.pos, posB.pos) < desiredDistance * desiredDistance)
			{
				bool aIsTrigger = components[0][i].ComponentFound(4);
				bool bIsTrigger = components[0][j].ComponentFound(4);
 				if (aIsTrigger != bIsTrigger)
				{
					if (aIsTrigger)
						components[0][i][4].physicsTrigger.lastHit = components[0][j].entity;
					else
						components[0][j][4].physicsTrigger.lastHit = components[0][i].entity;
				}
				if (aIsTrigger || bIsTrigger)
					continue;

				// Implement better physics here!!!
				float ratioA = bodyB.mass / (bodyA.mass + bodyB.mass);
				float ratioB = 1 - ratioA;

				vec2 dir = glm::normalize(posB.pos - posA.pos);
				vec2 weightedCenter = glm::mix(posA.pos, posB.pos, ratioA);
				posA.pos = weightedCenter - dir * desiredDistance * ratioA;
				posB.pos = weightedCenter + dir * desiredDistance * ratioB;
			}
		}
}

SYSTEM(PlayerCameraUpdate, playerCameraUpdate, { COMP_REQ((Position)(Player)) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity[1].player.camera.pos = entity[0].position.pos;
}

SYSTEM(UpdateFollowers, updateFollowers, { COMP_REQ((Position)(Follower)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Follower& follower = entity[1];
		entity[0].position.pos = follower.parent->pos + follower.offset;
	}
}

#pragma endregion

#pragma region Game Stuff 2

SYSTEM(ProjectileHit, projectilHit, { COMP_REQ((PhysicsTrigger)(Projectile), , , , OnTriggerOverlapEntityEval)}, Update)
{
	for (ProcEntity& entity : components[0])
	{
		PhysicsTrigger& trigger = entity[0];
		Projectile& projectile = entity[1];

		Entity* hit = trigger.lastHit;//for (Entity* hit : trigger.lastHit)
		{
			Health* health = hit->TryGetComponent<Health>();
			if (health != nullptr)
				health->health -= projectile.damage;
			projectile.pierce--;
			if (projectile.pierce <= 0)
			{
				entity.entity->toDestroy = true;
				break;
			}
		}
	}
}

#pragma endregion

#pragma region Rendering

SYSTEM(CameraMatrixUpdate, cameraMatrixUpdate, { COMP_REQ((Position)(Camera)(CameraMatrix)) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Position& pos = entity[0];
		Camera& camera = entity[1];
		CameraMatrix& camMat = entity[2];
		vec2 stretch = camera.framebuffer->FindStretch();
		// Find the typical matrix for rendering:
		camMat.matrix = glm::identity<mat4>();
		camMat.matrix = glm::scale(camMat.matrix,
			vec3(pixelsPerUnit * 2.f / camera.framebuffer->dim.x,
				pixelsPerUnit * 2.f / camera.framebuffer->dim.y, 1));
		camMat.matrix = glm::translate(camMat.matrix, vec3(-pos.pos, 0));
		// Find the debug matrix for rendering of stuff non-pixelated:
		camMat.debugMatrix = glm::identity<mat4>();
		camMat.debugMatrix = glm::translate(camMat.debugMatrix, -vec3(stretch - vec2(1), 0));
		camMat.debugMatrix = glm::scale(camMat.debugMatrix,
			vec3(pixelsPerUnit * 2.f / (stretch.x * camera.framebuffer->dim.x),
				pixelsPerUnit * 2.f / (stretch.y * camera.framebuffer->dim.y), 1));
		camMat.debugMatrix = glm::translate(camMat.debugMatrix, vec3(-pos.pos, 0));
	}
}

SYSTEM(MeshRenderUpdate, meshRenderUpdate, SysReq({ COMP_REQ((Camera)(CameraMatrix)),
	COMP_REQ((MeshRenderer)(Position)(Scale)(Rotation)) }), Update)
{
	for (ProcEntity& cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0];
		CameraMatrix& camMat = cameraEntity[1];

		glUseProgram(camera.shader.program);
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camMat.matrix));

		for (ProcEntity& meshEntity : components[1])
		{
			MeshRenderer& meshRenderer = meshEntity[0];
			if ((camera.renderMask & static_cast<byte>(meshRenderer.renderLayer)) == 0)
				continue;
			Position& pos = meshEntity[1];
			Scale& scale = meshEntity[2];
			Rotation& rotation = meshEntity[3];
			
			mat3 transform = glm::identity<mat3>();
			transform = glm::translate(transform, pos.pos);
			transform = glm::rotate(transform, glm::radians(rotation.rotation));
			transform = glm::scale(transform, scale.scale * 0.5f);

			glUseProgram(meshRenderer.shader.program);
			glUniformMatrix3fv(glGetUniformLocation(meshRenderer.shader.program, "transform"),
				1, GL_FALSE, glm::value_ptr(transform));
			glUniform4f(glGetUniformLocation(meshRenderer.shader.program, "color"),
				meshRenderer.color.r, meshRenderer.color.g, meshRenderer.color.b, meshRenderer.color.a);
			meshRenderer.mesh.Draw();
		}
	}
}

SYSTEM(RenderToScreen, renderToScreen, { }, Update)
{
	Framebuffer::BindScreen();
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainFramebuffer.texture);

	glUseProgram(framebufferShader.program);
	vec2 stretch = mainFramebuffer.FindStretch();
	glUniform2f(glGetUniformLocation(framebufferShader.program, "stretch"),
		stretch.x, stretch.y);

	quadMesh.Draw();
}

SYSTEM(NumberRenderUpdate, numberRenderUpdate, SysReq({ COMP_REQ((Camera)(CameraMatrix)),
	COMP_REQ((NumberRenderer)(Position)(Scale)(Rotation)) }), Update)
{
	glEnable(GL_BLEND);
	for (ProcEntity& cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0];
		CameraMatrix& camMat = cameraEntity[1];

		glUseProgram(textShader.program);
		glUniformMatrix4fv(glGetUniformLocation(textShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camMat.debugMatrix));

		for (ProcEntity& meshEntity : components[1])
		{
			NumberRenderer& numberRenderer = meshEntity[0];
			if ((camera.renderMask & static_cast<byte>(numberRenderer.renderLayer)) == 0)
				continue;
			Position& pos = meshEntity[1];
			Scale& scale = meshEntity[2];
			Rotation& rotation = meshEntity[3];

			font.Render(numberRenderer.Text(), pos.pos, numberRenderer.size, rotation.rotation, numberRenderer.color);
		}
	}
	glDisable(GL_BLEND);
}

SYSTEM(TextRenderUpdate, textRenderUpdate, SysReq({ COMP_REQ((Camera)(CameraMatrix)),
	COMP_REQ((TextRenderer)(Position)(Scale)(Rotation)) }), Update)
{
	glEnable(GL_BLEND);
	for (ProcEntity& cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0];
		CameraMatrix& camMat = cameraEntity[1];

		glUseProgram(textShader.program);
		glUniformMatrix4fv(glGetUniformLocation(textShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camMat.debugMatrix));

		for (ProcEntity& meshEntity : components[1])
		{
			TextRenderer& textRenderer = meshEntity[0];
			if ((camera.renderMask & static_cast<byte>(textRenderer.renderLayer)) == 0)
				continue;
			Position& pos = meshEntity[1];
			Scale& scale = meshEntity[2];
			Rotation& rotation = meshEntity[3];

			font.Render(stringStorage.Get(textRenderer.storageIndex), pos.pos, textRenderer.size, rotation.rotation, textRenderer.color);
		}
	}
	glDisable(GL_BLEND);
}

constexpr vec4 debugColor = vec4(0, 1, 0, 0.3f);

SYSTEM(DebugRenderUpdate, debugRenderUpdate, SysReq({ COMP_REQ((Camera)(CameraMatrix)),
	COMP_REQ((MeshRenderer)(Position)(Scale)(Rotation)) }), Update)
{

	if (!inDebug)
		return;

	glEnable(GL_BLEND);
	for (ProcEntity& cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0];
		CameraMatrix& camMat = cameraEntity[1];

		glUseProgram(camera.shader.program);
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camMat.debugMatrix));

		for (ProcEntity& meshEntity : components[1])
		{
			MeshRenderer& meshRenderer = meshEntity[0];
			if ((camera.renderMask & static_cast<byte>(meshRenderer.renderLayer)) == 0)
				continue;
			Position& pos = meshEntity[1];
			Scale& scale = meshEntity[2];
			Rotation& rotation = meshEntity[3];

			mat3 transform = glm::identity<mat3>();
			transform = glm::translate(transform, pos.pos);
			transform = glm::rotate(transform, glm::radians(rotation.rotation));
			transform = glm::scale(transform, scale.scale * 0.5f);

			glUseProgram(meshRenderer.shader.program);
			glUniformMatrix3fv(glGetUniformLocation(meshRenderer.shader.program, "transform"),
				1, GL_FALSE, glm::value_ptr(transform));
			glUniform4f(glGetUniformLocation(meshRenderer.shader.program, "color"),
				debugColor.r, debugColor.g, debugColor.b, debugColor.a);
			meshRenderer.mesh.Draw();
		}
	}
	glDisable(GL_BLEND);
}

#pragma endregion

#pragma region Destroy

SYSTEM(AwardPoints, awardPoints, SysReq({ COMP_REQ((PointAward), , , , DestroyEntityEval), COMP_REQ((PlayerPoints)) }), Update)
{
	for (ProcEntity& playerPointKeeper : components[1])
	{
		PlayerPoints& playerPoints = playerPointKeeper[0];
		for (ProcEntity& pointAwarder : components[0])
			playerPoints.points += pointAwarder[0].pointAward.points;
	}
}

SYSTEM(ExecuteDestruction, executeDestruction, { COMP_REQ(, , , , DestroyEntityEval) }, Update)
{
	for (int i = components[0].size() - 1; i >= 0; i--)
		ECS::RemoveEntity(components[0][i].entity->index);
}

#pragma endregion



#pragma region Closes

#pragma endregion