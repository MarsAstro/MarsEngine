//
// Created by Marius on 26-Jun-24.
//

#include "scene.h"

void Scene::DrawScene(const Shading::ShaderProgram* shader) const
{
    for (auto object : sceneObjects)
    {
        object.model->position = object.position;
        object.model->Draw(shader);
    }
}

void Scene::AddObject(Geometry::Model* model, glm::vec3 position)
{
    sceneObjects.emplace_back(model, position);
}
