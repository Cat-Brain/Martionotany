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

    ECS::AddEntity(Prefabs::player.Clone({}, { Player(ECS::GetEntity(cameraIndex).GetComponent(HASH(Position)), 30, 3) }));
    ECS::GetEntity(cameraIndex).GetComponent(HASH(CameraMouse));
    ECS::AddEntity(Entity({ NumberRenderer(1), PlayerPoints(),
        Follower(ECS::GetEntity(cameraIndex).TryGetComponent<Position>(), {0, -3.5f}),
        Position(), Scale(), Rotation() }));

    ECS::AddEntity(Prefabs::testClickable.Clone({ Position({3, 0}) }));
    ECS::AddEntity(Prefabs::testClickable.Clone({ Position({-2, 2}) }));

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
    Player Projectiles
    Enemy Basics
    Enemy Advanced
    Finish Player Health
    Lose State
    Restart
    Main Menu
*/

/*
Bug Fixes:
    Make player collide with enemies?
    Fix weird issue with worldMousePos and gridMousePos not working quite right (it's soooo close)
        Seems to be an issue with worldMousePos NOT gridMousePos, use this information for further investigation.
        Seems to effect both dimensions but verticality strongest (which is the inverse of what was expected).
*/

/*
Engine To Do:
    Implement physics layers
    Upgrade triggers to rely on vectors of hits rather than just storing the last one
    Add saving
    Add file loading
    Allow for custom destructors in components or some other workaround to allow for vectors
        ^^ Partially implemented and likely to deal with Destroy systems
    Allow for proper handling of multiple of same component
    Rework window handling to allow for more windows
    Add text rendering
        Implement actual text renderer in addition to just number renderer
    Add UI layer
    Add basic support for Audio
        ^^ CAudio?
*/
