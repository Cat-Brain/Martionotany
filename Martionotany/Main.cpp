#include "Systems.h"

int main()
{
	cout << "Greetings Universe!\n";

    ECS ecs;

    // Startups:
    System::startupGenerics.push_back({ WindowInit, Before });
    System::startupGenerics.push_back({ ShaderInit, Before });
    System::startupGenerics.push_back({ MeshInit, Before });

    // Updates:
    System::updateGenerics.push_back({ WindowUpdate, Before });
    System::updateGenerics.push_back({ Input::UpdateKeys, Before });

    // Closes:
    System::closeGenerics.push_back({ ShaderTerminate, After });
    System::closeGenerics.push_back({ MeshTerminate, After });
    System::closeGenerics.push_back({ WindowTerminate, After });

    // Entities:
    ecs.AddEntity(Entity({ Position(), Camera(5, RenderLayer::DEFAULT) }));
    ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.6f, 0.8f, 0.4f, 1.f)),
        Position(), MeshScale(), Player(ecs.GetEntityReversed(0).components[0].position, 10, 2, 5), }));

    ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({3, 0}), MeshScale() }));
    ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({5, 0}), MeshScale() }));

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