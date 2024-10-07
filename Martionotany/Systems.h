#pragma once
#include "Window.h"
#include "Input.h"

#pragma region Early Physics

SYSTEM(UpdateGravity, updateGravity, { CompList({ HASH(PhysicsBody), HASH(Gravity) }) }, Update)
{
	for (vector<Component*> entity : components[0])
	{
		PhysicsBody& body = entity[0]->physicsBody;
		Gravity& gravity = entity[1]->gravity;

		body.vel += deltaTime * defaultGravity * gravity.multiplier * gravity.direction;
	}
}

#pragma endregion

#pragma region Game Stuff

SYSTEM(PlayerClickTest, playerClickTest, { CompList({ HASH(Position), HASH(Player) }) }, Update)
{
	if (!Input::click1.held)
		return;
	for (vector<Component*> entity : components[0])
	{
		Position& position = entity[0]->position;
		Player& player = entity[1]->player;


	}
}

SYSTEM(PlayerMove, playerMove, { CompList({ HASH(PhysicsBody), HASH(Player) }) }, Update)
{
	for (vector<Component*> entity : components[0])
	{
		PhysicsBody& physicsBody = entity[0]->physicsBody;
		Player& player = entity[1]->player;

		float inp = 0;
		if (Input::d.held)
			inp++;
		if (Input::a.held)
			inp--;

		if (inp == 0)
			physicsBody.vel.x = TrySubF(physicsBody.vel.x, deltaTime * player.accel);
		else
			physicsBody.vel.x = TryAddF(physicsBody.vel.x, inp * deltaTime * player.accel, player.speed);

		if (Input::jump.pressed)
			physicsBody.vel.y += player.jumpForce;
	}
}

#pragma endregion

#pragma region Physics

SYSTEM(UpdatePhysicsBodies, updatePhysicsBodies, { CompList({HASH(Position), HASH(PhysicsBody)}) }, Update)
{
	for (vector<Component*> entity : components[0])
	{
		Position& pos = entity[0]->position;
		PhysicsBody& body = entity[1]->physicsBody;

		pos.pos += body.vel * deltaTime;
	}
}

SYSTEM(UpdateCirclesXInfiniteWalls, updateCirclesXInfiniteWalls, SysReq({ {HASH(InfinitePhysicsWall)},
	CompList({HASH(Position), HASH(PhysicsBody), HASH(PhysicsCircle)}) }), Update)
{
	for (vector<Component*> wallEntity : components[0])
	{
		InfinitePhysicsWall& wall = wallEntity[0]->infinitePhysicsWall;
		for (vector<Component*> circleEntity : components[1])
		{
			Position& pos = circleEntity[0]->position;
			PhysicsBody& body = circleEntity[1]->physicsBody;
			PhysicsCircle& circle = circleEntity[2]->physicsCircle;

			float height = glm::dot(wall.normal, pos.pos) - circle.radius;
			if (height > wall.height)
				continue;

			pos.pos += (wall.height - height) * wall.normal;
			body.vel -= min(0.f, glm::dot(wall.normal, body.vel)) * wall.normal;
		}
	}
}

SYSTEM(UpdateAABBsXInfiniteWalls, updateAABBsXInfiniteWalls, SysReq({ {HASH(InfinitePhysicsWall)},
	CompList({HASH(Position), HASH(PhysicsBody), HASH(PhysicsBox)}) }), Update)
{
	for (vector<Component*> wallEntity : components[0])
	{
		InfinitePhysicsWall& wall = wallEntity[0]->infinitePhysicsWall;
		for (vector<Component*> circleEntity : components[1])
		{
			Position& pos = circleEntity[0]->position;
			PhysicsBody& body = circleEntity[1]->physicsBody;
			PhysicsBox& box = circleEntity[2]->physicsBox;

			vec2 right = vec2(cos(box.rotation), sin(box.rotation));
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

SYSTEM(UpdateCirclesXCircles, updateCirclesXCircles, { CompList({HASH(Position), HASH(PhysicsBody), HASH(PhysicsCircle)}) }, Update)
{
	for (int i = 0; i < (int)components[0].size() - 1; i++)
		for (int j = i + 1; j < (int)components[0].size(); j++)
		{
			Position& posA = components[0][i][0]->position;
			Position& posB = components[0][j][0]->position;

			PhysicsBody& bodyA = components[0][i][1]->physicsBody;
			PhysicsBody& bodyB = components[0][j][1]->physicsBody;

			PhysicsCircle& circleA = components[0][i][2]->physicsCircle;
			PhysicsCircle& circleB = components[0][j][2]->physicsCircle;
			
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

SYSTEM(PlayerCameraUpdate, playerCameraUpdate, { CompList({ HASH(Position), HASH(Player) }) }, Update)
{
	for (vector<Component*> entity : components[0])
		entity[1]->player.camera.pos = entity[0]->position.pos;
}

SYSTEM(CameraMatrixUpdate, cameraMatrixUpdate, { CompList({ HASH(Position), HASH(Camera) }) }, Update)
{
	for (vector<Component*> entity : components[0])
	{
		Position& pos = entity[0]->position;
		Camera& camera = entity[1]->camera;
		camera.matrix = glm::identity<mat4>();
		vec3 test = vec3(pixelsPerUnit * 2.f / camera.framebuffer->dim.x, pixelsPerUnit * 2.f / camera.framebuffer->dim.y, 1);
		camera.matrix = glm::scale(camera.matrix, test);
		camera.matrix = glm::translate(camera.matrix, vec3(-pos.pos, 0));
	}
}

SYSTEM(MeshRenderUpdate, meshRenderUpdate, SysReq({ {HASH(Camera)}, CompList({HASH(MeshRenderer), HASH(Position), HASH(Scale), HASH(Rotation)})}), Update)
{
	for (vector<Component*> cameraEntity : components[0])
	{
		Camera& camera = cameraEntity[0]->camera;
		glUseProgram(camera.shader.program);
		glUniformMatrix4fv(glGetUniformLocation(defaultShader.program, "camera"), 1, GL_FALSE, glm::value_ptr(camera.matrix));

		for (vector<Component*> meshEntity : components[1])
		{
			MeshRenderer& meshRenderer = meshEntity[0]->meshRenderer;
			if ((camera.renderMask & static_cast<byte>(meshRenderer.renderLayer)) == 0)
				continue;
			Position& pos = meshEntity[1]->position;
			Scale& scale = meshEntity[2]->scale;
			Rotation& rotation = meshEntity[3]->rotation;
			
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

#pragma endregion