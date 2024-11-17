#include "Systems.h"


int main()
{
    cout << "Greetings Universe!\n";

    vector<CHash> test = HASH_ALL((InteractableColor)(MouseInteractable)(MeshRenderer));

    // Entities:
    int cameraIndex = ECS::AddEntity(Entity({ Position(), Camera(&mainFramebuffer, RenderLayer::DEFAULT),
        CameraMatrix(), CameraMouse() }));
    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.6f, 0.8f, 0.4f, 1.f)),
        Position({0, 0}), Scale(), Rotation(45), PhysicsBody(1), PhysicsBox(), Gravity(),
        Player(ECS::GetEntity(cameraIndex).GetComponent(HASH(Position)), 10, 2, 5) }));

    ECS::AddEntity(testClickable.Clone());
    ECS::AddEntity(testClickable.Clone({ Position({-2, 2}) }));

    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({5, 0}), Scale(), Rotation() }));

    ECS::AddEntity(Entity({ InfinitePhysicsWall(Vec::up, -0.5f) }));

    // Program Execution:
    ECS::Start();


    while (!glfwWindowShouldClose(window))
    {
        ECS::Update();
        glfwPollEvents();
    }

    ECS::Close();

    cout << "Farewell Universe!\n";

    return 0;
}

/*
To Do:
    Fix weird issue with worldMousePos and gridMousePos not working quite right (it's soooo close)
        ^^ Seems to be highly tied with the amount of pixels not displayed that are rendered. Solution probably deals with them.
    Add saving
    Add file loading
    Allow for custom destructors in components or some other workaround to allow for vectors
        ^^ Partially implemented and likely to deal with Destroy systems
    Allow for proper handling of multiple of same component
    Rework window handling to allow for more windows
    Add text rendering
    Add UI layer
    Add basic support for Audio
        ^^ CAudio?
*/
