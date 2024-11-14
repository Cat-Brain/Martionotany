#pragma once
#include "Window.h"
#include "Input.h"

Prefab testClickable = Prefab({ MeshRenderer(defaultShader, quadMesh),
    Position({3, 0}), Scale(), Rotation(), PhysicsBox(),
    MouseInteractable(), DestroyOnInteract(),
    InteractableColor(vec4(0.8f, 0.7f, 0.9f, 1.f), vec4(0.6f, 0.5f, 0.7f, 1.f), vec4(0.4f, 0.3f, 0.5f, 1.f)) });