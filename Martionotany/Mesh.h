#pragma once
#include "Shader.h"
#include "Camera.h"

class VMesh;
vector<VMesh*> vMeshes;
class TMesh;
vector<TMesh*> tMeshes;

class Mesh
{
public:
    virtual void Draw() const = 0;
};

enum Read: byte { read };
enum Open: byte { open };

class VMesh : public Mesh
{
public:
    uint vao, vbo, vertCount;
    vector<float> vertices;
    string location;

    VMesh(uint vao = 0, uint vbo = 0, uint vertCount = 0) :
        vao(vao), vbo(vbo), vertCount(vertCount) { }

    VMesh(Read read, const vector<float>&& vertices) :
        vertices(vertices), location("N"), vao(0), vbo(0),
        vertCount(static_cast<uint>(vertices.size()))
    {
        vMeshes.push_back(this);
    }
    VMesh(Open open, const string&& location) :
        location(location), vao(0), vbo(0),
        vertCount(static_cast<uint>(vertices.size()))
    {
        vMeshes.push_back(this);
    }

    void Init()
    {
        if (location != "N")
        {
            fastObjMesh* mesh = fast_obj_read((location + ".obj").c_str());
            
            vector<float> positions((mesh->position_count - 1) * 3);
            for (uint i = 0; i < positions.size(); i++)
                positions[i] = mesh->positions[i + 3];

            vector<float> normals((mesh->normal_count - 1) * 3);
            for (uint i = 0; i < normals.size(); i++)
                normals[i] = mesh->normals[i + 3];

            vector<uint> indices;
            for (uint i = 0; i * 3 < mesh->index_count; i++)
            {
                indices.push_back(mesh->indices[i * 3 + 2].p - 1);
                indices.push_back(mesh->indices[i * 3 + 1].p - 1);
                indices.push_back(mesh->indices[i * 3].p - 1);
                indices.push_back(mesh->indices[i * 3 + 2].n - 1);
                indices.push_back(mesh->indices[i * 3 + 1].n - 1);
                indices.push_back(mesh->indices[i * 3].n - 1);
            }

            fast_obj_destroy(mesh);

            vector<float> data;
            for (size_t i = 0; i * 6 < indices.size(); i++)
            {
                data.push_back(positions[static_cast<size_t>(indices[i * 6]) * 3]);
                data.push_back(positions[static_cast<size_t>(indices[i * 6]) * 3 + 1]);
                data.push_back(positions[static_cast<size_t>(indices[i * 6]) * 3 + 2]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 3]) * 3]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 3]) * 3 + 1]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 3]) * 3 + 2]);

                data.push_back(positions[static_cast<size_t>(indices[i * 6 + 1]) * 3]);
                data.push_back(positions[static_cast<size_t>(indices[i * 6 + 1]) * 3 + 1]);
                data.push_back(positions[static_cast<size_t>(indices[i * 6 + 1]) * 3 + 2]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 4]) * 3]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 4]) * 3 + 1]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 4]) * 3 + 2]);

                data.push_back(positions[static_cast<size_t>(indices[i * 6 + 2]) * 3]);
                data.push_back(positions[static_cast<size_t>(indices[i * 6 + 2]) * 3 + 1]);
                data.push_back(positions[static_cast<size_t>(indices[i * 6 + 2]) * 3 + 2]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 5]) * 3]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 5]) * 3 + 1]);
                data.push_back(normals[static_cast<size_t>(indices[i * 6 + 5]) * 3 + 2]);
            }
        }
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void Use() const
    {
        glBindVertexArray(vao);
    }

    void Draw() const override
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertCount);
    }

    void Terminate() const
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};

class TMesh : public Mesh
{
public:
    uint vao, vbo, ebo, indCount;
    vector<float> vertices;
    vector<uint> indices;

    TMesh(uint vao = 0, uint vbo = 0, uint ebo = 0, uint indCount = 0) :
        vao(vao), vbo(vbo), ebo(ebo), indCount(indCount) { }

    TMesh(const vector<float>&& vertices, const vector<uint>&& indices) :
        vertices(vertices), indices(indices), indCount(static_cast<uint>(indices.size())),
        vao(0), vbo(0), ebo(0)
    {
        tMeshes.push_back(this);
    }

    void Init()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void Use() const
    {
        glBindVertexArray(vao);
    }

    void Draw() const override
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0);
    }

    void Terminate() const
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
};

VMesh triMesh = VMesh(read, {
       -0.5f,-0.5f, 0.f, 0.f,
         0.f, 0.5f,0.5f, 1.f,
        0.5f,-0.5f, 1.f, 0.f
    });
TMesh quadMesh = TMesh({
        1.0f,  1.0f, 1.f, 1.f,
        1.0f, -1.0f, 1.f, 0.f,
       -1.0f, -1.0f, 0.f, 0.f,
       -1.0f,  1.0f, 0.f, 1.f
    }, {
        0, 1, 3,
        1, 2, 3
    });

GENERIC_SYSTEM(MeshInit, meshInit, After, Start)
{
    for (VMesh* vMesh : vMeshes)
        vMesh->Init();
    for (TMesh* tMesh : tMeshes)
        tMesh->Init();
}

GENERIC_SYSTEM(MeshTerminate, meshTerminate, After, Close)
{
    triMesh.Terminate();
    quadMesh.Terminate();
}

class MeshRenderer : public BaseComponent
{
public:
    Shader& shader;
    Mesh& mesh;
    vec4 color;
    byte renderLayer;

    MeshRenderer(Shader& shader, Mesh& mesh, vec4 color = { 1, 1, 1, 1 }, byte renderLayer = RenderLayer::DEFAULT) :
        shader(shader), mesh(mesh), color(color), renderLayer(renderLayer) { SET_HASH; }
};

class Scale : public BaseComponent
{
public:
    vec2 scale;

    Scale(vec2 scale = vec2(1)):
        scale(scale) { SET_HASH; }
};

class Rotation : public BaseComponent
{
public:
    float rotation;

    Rotation(float rotation = 0) :
        rotation(rotation) {
        SET_HASH;
    }
};