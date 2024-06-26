#pragma once
#include "../geometry/model.h"

struct SceneObject
{
    Geometry::Model* model;
    glm::vec3 position;
};

class Scene {
public:
    void DrawScene(const Shading::ShaderProgram* shader) const;
    void AddObject(Geometry::Model* model, glm::vec3 position);

private:
    std::vector<SceneObject> sceneObjects;
};
