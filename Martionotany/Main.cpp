#include "Systems.h"

class TestDestroyMethod
{
public:
    int value;

    TestDestroyMethod(int value) :
        value(value) { }

    ~TestDestroyMethod()
    {
        cout << value << '\n';
    }

    TestDestroyMethod(TestDestroyMethod&) = delete;
};


int main()
{
    cout << "Greetings Universe!\n";

    //{
    //    TestDestroyMethod testA(1);
    //    vector<TestDestroyMethod> testB = make_vector<TestDestroyMethod>(2, 3);
    //    vector<TestDestroyMethod> testC = make_vector<TestDestroyMethod>(4, 5);
    //    /*testC.reserve(testC.size() + testB.size());
    //    for (TestDestroyMethod& testTemp : testB)
    //        testC.emplace_back(move(testTemp));*/
    //    //move(std::make_move_iterator(testB.begin()), std::make_move_iterator(testB.end()), );
    //    testC.reserve(testC.size() + testB.size());
    //    //testC.insert(testC.end(), std::make_move_iterator(testB.begin()), std::make_move_iterator(testB.end()));
    //    testC.insert(testC.end(), testB.begin(), testB.end());

    //    cout << "Test\n";
    //}

    // Entities:
    int cameraIndex = ECS::AddEntity(Entity({ Position(),
        Camera(&mainFramebuffer, RenderLayer::DEFAULT), CameraMatrix(), CameraMouse() }));

    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.6f, 0.8f, 0.4f, 1.f)),
        Position({0, 0}), Scale(), Rotation(45), PhysicsBody(1), PhysicsBox(),
        Player(ECS::GetEntity(cameraIndex).GetComponent(HASH(Position)), 30, 3) }));

    ECS::AddEntity(Entity({ NumberRenderer(1, 1234567890),
        FollowCursor(/*ECS::GetEntity(cameraIndex).GetComponent(HASH(CameraMouse))*/),
        Position(), Scale(), Rotation() }));

    ECS::AddEntity(testClickable.Clone());
    ECS::AddEntity(testClickable.Clone({ Position({-2, 2}) }));

    ECS::AddEntity(Entity({ MeshRenderer(defaultShader, quadMesh, vec4(0.8f, 0.6f, 0.8f, 1.f)),
        Position({5, 0}), Scale(), Rotation() }));

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
Game To Do:
    Health
    Projectiles
    Player Projectiles
    Enemy Basics
    Enemy Advanced
    Lose
    Restart
    Main Menu
*/

/*
Engine To Do:
    Fix weird issue with worldMousePos and gridMousePos not working quite right (it's soooo close)
        ^^ Seems to be highly tied with the amount of pixels not displayed that are rendered. Solution probably deals with them.
    Add saving
    Add file loading
    Allow for custom destructors in components or some other workaround to allow for vectors
        ^^ Partially implemented and likely to deal with Destroy systems
    Allow for proper handling of multiple of same component
    Rework window handling to allow for more windows
    Add text rendering
        Fix to use proper coordinate systems and matrix multiplication
    Add UI layer
    Add basic support for Audio
        ^^ CAudio?
*/
