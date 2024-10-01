#include "Systems.h"

int main()
{
	cout << "Greetings Universe!\n";

    ECS ecs;
    
    // Entities:
    ecs.AddEntity(Entity({ Position(), Camera(&mainFramebuffer, RenderLayer::DEFAULT) }));
    ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.6f, 0.8f, 0.4f, 1.f)),
        Position({0, 0}), Scale(), Rotation(45), PhysicsBody(1), PhysicsBox(), Gravity(),
        Player(ecs.GetEntityReversed(0).components[0].position, 10, 2, 5)}));

    ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({3, 0}), Scale(), Rotation() }));
    ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({5, 0}), Scale(), Rotation() }));

    ecs.AddEntity(Entity({ InfinitePhysicsWall(Vec::up, -0.5f) }));

    // Program Execution:
    ecs.Start();


    while (!glfwWindowShouldClose(window))
    {
        ecs.Update();
        glfwPollEvents();
    }
    
    ecs.Close();

    cout << "Farewell Universe!\n";

	return 0;
}

/*
To Do:
    Make resources all be in a folder such that they are easier to manage and organize
    Complete ECS
    Move starting implementations into ECS instead of normal functions for consistency
*/