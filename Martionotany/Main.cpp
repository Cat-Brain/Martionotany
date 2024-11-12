#include "Systems.h"

#define FOO1(a) cout << a;
#define FOO2(a, b) cout << a << ", " << b;

int main()
{
	cout << "Greetings Universe!\n";

    // Entities:
    int cameraIndex = ECS::AddEntity(Entity({ Position(), Camera(&mainFramebuffer, RenderLayer::DEFAULT),
        CameraMouse() }));
    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.6f, 0.8f, 0.4f, 1.f)),
        Position({0, 0}), Scale(), Rotation(45), PhysicsBody(1), PhysicsBox(), Gravity(),
        Player(ECS::GetEntity(cameraIndex).GetComponent(HASH(Position)), 10, 2, 5)}));

    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh),
        Position({3, 0}), Scale(), Rotation(), PhysicsBox(),
        MouseInteractable(),
        InteractableColor(vec4(0.8f, 0.7f, 0.9f, 1.f), vec4(0.6f, 0.5f, 0.7f, 1.f), vec4(0.4f, 0.3f, 0.5f, 1.f)) }));

    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({5, 0}), Scale(), Rotation() }));

    //ecs.AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
    //    Position(), Scale(), Rotation(), FollowCursor(ecs.GetEntity(cameraIndex).GetComponent<CameraMouse>())}));

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
    Fix RemoveEntity
    Allow for custom destructors in components or some other workaround to allow for vectors
        ^^ Partially implemented and likely to deal with Destroy systems
    Allow for proper handling of multiple of same component
    Rework window handling to allow for more windows
    Add text rendering
    Add UI layer
    Make resources all be in a folder such that they are easier to manage and organize <- Ooooh, maybe in the copy file tab in project settings?
    Add basic support for Audio <- CAudio?
*/