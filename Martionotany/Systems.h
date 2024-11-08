#pragma once
#include "Window.h"
#include "Input.h"

#pragma region Early Physics

SYSTEM(UpdateGravity, updateGravity, { CompReq({ HASH(PhysicsBody), HASH(Gravity) }) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		PhysicsBody& body = entity[0];
		Gravity& gravity = entity[1];

		body.vel += deltaTime * defaultGravity * gravity.multiplier * gravity.direction;
	}
}

#pragma endregion

#pragma region Game Stuff

SYSTEM(UpdateMouse, updateMouse, { CompReq({ HASH(Camera), HASH(CameraMouse), HASH(Position) }) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Camera& camera = entity[0];
		CameraMouse& mouse = entity[1];
		Position& position = entity[2];

		mouse.camMousePos = (Input::screenMousePos - vec2(0.5f)) * camera.CamDim();
		mouse.worldMousePos = mouse.camMousePos + position.pos;
		mouse.gridMousePos = ToGrid(mouse.worldMousePos) - vec2(0.5f / pixelsPerUnit);
	}
}

SYSTEM(MouseXPhysicsCircle, mouseXPhysicsCircle, SysReq({ CompReq({ HASH(CameraMouse) }),
	CompReq({ HASH(MouseInteractable), HASH(PhysicsCircle), HASH(Position) }) }), Update)
{
	for (ProcEntity& mouseEntity : components[0])
	{
		CameraMouse& mouse = mouseEntity[0];
		for (ProcEntity& physicsEntity : components[1])
			physicsEntity[0].mouseInteractable.Update(physicsEntity[1].physicsCircle.Overlaps(
				mouse.gridMousePos - physicsEntity[2].position.pos));
	}
}

SYSTEM(MouseXPhysicsBox, mouseXPhysicsBox, SysReq({ CompReq({HASH(CameraMouse)}),
	CompReq({ HASH(MouseInteractable), HASH(PhysicsBox), HASH(Position), HASH(Rotation)})}), Update)
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
	{ CompReq({ HASH(InteractableColor), HASH(MouseInteractable), HASH(MeshRenderer) })}, Update)
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

SYSTEM(OnInteractUpdate, onInteractUpdate, { CompReq({ HASH(MouseInteractable), HASH(OnInteract) }) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		MouseInteractable& mouseInteractable = entity[0];
		OnInteract& onInteract = entity[1];

		if (mouseInteractable.pressed && (onInteract.condition == ON_PRESS || onInteract.condition == ON_BOTH))
			onInteract.onInteract.Run();

		if (mouseInteractable.released && (onInteract.condition == ON_RELEASE || onInteract.condition == ON_BOTH))
			onInteract.onInteract.Run();
	}
}

SYSTEM(UpdateFollowCursor, updateFollowCursor, { CompReq({ HASH(FollowCursor), HASH(Position) }) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		FollowCursor& followCursor = entity[0];
		Position& position = entity[1];

		position.pos = followCursor.mouse.worldMousePos;
	}
}

SYSTEM(PlayerMove, playerMove, { CompReq({ HASH(PhysicsBody), HASH(Player), HASH(Rotation) })}, Update)
{
	for (ProcEntity& entity : components[0])
	{
		PhysicsBody& physicsBody = entity[0];
		Player& player = entity[1];
		Rotation& rotation = entity[2];

		float inp = 0;
		if (Input::d.held)
			inp++;
		if (Input::a.held)
			inp--;

		float mult = (Input::sprint.held ? 2 : 1) * (Input::walk.held ? 0.5f : 1);

		if (inp == 0)
			physicsBody.vel.x = TrySubF(physicsBody.vel.x, deltaTime * player.accel);
		else
			physicsBody.vel.x = TryAddF(physicsBody.vel.x, mult * inp * deltaTime * player.accel, mult * player.speed);

		if (Input::jump.pressed)
			physicsBody.vel.y += player.jumpForce;

		rotation.rotation += deltaTime * 45 * mult;
	}
}

#pragma endregion

#pragma region Physics

SYSTEM(UpdatePhysicsBodies, updatePhysicsBodies, { CompReq({HASH(Position), HASH(PhysicsBody)}) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Position& pos = entity[0];
		PhysicsBody& body = entity[1];

		pos.pos += body.vel * deltaTime;
	}
}

SYSTEM(UpdateCirclesXInfiniteWalls, updateCirclesXInfiniteWalls, SysReq({ CompReq({HASH(InfinitePhysicsWall)}),
	CompReq({HASH(Position), HASH(PhysicsBody), HASH(PhysicsCircle)}) }), Update)
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

			pos.pos += (wall.height - height) * wall.normal;
			body.vel -= min(0.f, glm::dot(wall.normal, body.vel)) * wall.normal;
		}
	}
}

SYSTEM(UpdateAABBsXInfiniteWalls, updateAABBsXInfiniteWalls, SysReq({ CompReq({HASH(InfinitePhysicsWall)}),
	CompReq({HASH(Position), HASH(Rotation), HASH(PhysicsBody), HASH(PhysicsBox)})}), Update)
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

			pos.pos += (wall.height - height) * wall.normal;
			body.vel -= min(0.f, glm::dot(wall.normal, body.vel)) * wall.normal;
		}
	}
}

SYSTEM(UpdateCirclesXCircles, updateCirclesXCircles, { CompReq({HASH(Position), HASH(PhysicsBody), HASH(PhysicsCircle)}) }, Update)
{
	for (int i = 0; i < (int)components[0].size() - 1; i++)
		for (int j = i + 1; j < (int)components[0].size(); j++)
		{
			Position& posA = components[0][i][0];
			Position& posB = components[0][j][0];

			PhysicsBody& bodyA = components[0][i][1];
			PhysicsBody& bodyB = components[0][j][1];

			PhysicsCircle& circleA = components[0][i][2];
			PhysicsCircle& circleB = components[0][j][2];
			
			float desiredDistance = circleA.radius + circleB.radius;
			if (posA.pos != posB.pos && glm::distance2(posA.pos, posB.pos) < desiredDistance * desiredDistance)
			{
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

#pragma endregion

#pragma region Rendering

SYSTEM(PlayerCameraUpdate, playerCameraUpdate, { CompReq({ HASH(Position), HASH(Player) }) }, Update)
{
	for (ProcEntity& entity : components[0])
		entity[1].player.camera.pos = entity[0].position.pos;
}

SYSTEM(CameraMatrixUpdate, cameraMatrixUpdate, { CompReq({ HASH(Position), HASH(Camera) }) }, Update)
{
	for (ProcEntity& entity : components[0])
	{
		Position& pos = entity[0];
		Camera& camera = entity[1];
		vec2 stretch = camera.framebuffer->FindStretch();
		// Find the typical matrix for rendering:
		camera.matrix = glm::identity<mat4>();
		camera.matrix = glm::scale(camera.matrix,
			vec3(pixelsPerUnit * 2.f / camera.framebuffer->dim.x,
				pixelsPerUnit * 2.f / camera.framebuffer->dim.y, 1));
		camera.matrix = glm::translate(camera.matrix, vec3(-pos.pos, 0));
		// Find the debug matrix for rendering of stuff non-pixelated:
		camera.debugMatrix = glm::identity<mat4>();
		camera.debugMatrix = glm::translate(camera.debugMatrix, -vec3(stretch - vec2(1), 0));
		camera.debugMatrix = glm::scale(camera.debugMatrix,
			vec3(pixelsPerUnit * 2.f / (stretch.x * camera.framebuffer->dim.x),
				pixelsPerUnit * 2.f / (stretch.y * camera.framebuffer->dim.y), 1));
		camera.debugMatrix = glm::translate(camera.debugMatrix, vec3(-pos.pos, 0));
	}
}

SYSTEM(MeshRenderUpdate, meshRenderUpdate, SysReq({ CompReq({HASH(Camera)}),
	CompReq({HASH(MeshRenderer), HASH(Position), HASH(Scale), HASH(Rotation)})}), Update)
{
	for (ProcEntity& cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0];
		glUseProgram(camera.shader.program);
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camera.matrix));

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

constexpr vec4 debugColor = vec4(0, 1, 0, 0.3f);

SYSTEM(DebugRenderUpdate, debugRenderUpdate, SysReq({ CompReq({HASH(Camera)}),
	CompReq({HASH(MeshRenderer), HASH(Position), HASH(Scale), HASH(Rotation)}) }), Update)
{
	if (!inDebug)
		return;

	glEnable(GL_BLEND);
	for (ProcEntity& cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0];
		glUseProgram(camera.shader.program);
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camera.debugMatrix));

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