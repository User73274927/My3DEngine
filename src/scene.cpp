#include "scene.h"

Scene::Scene() {
    this->root = new Object3D(Mesh::EMPTY);
}

Scene::~Scene() {
    delete root;
}

Object3D* Scene::getRoot() {
    return root;
}