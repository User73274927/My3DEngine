#pragma once

#include "../scene.h"
#include "../models.h"

/**
 * Сцена с вращающиемся по осям Ox, Oy, Oz кубом.
 */
struct CubeScene : Scene {
private:
    Mesh* cubeMesh;

    Object3D* cube;

public:
    void loadMeshes() {
        cubeMesh = Mesh::create(cubeVertices, cubeIndices);
    }

    void build() {
        cube = new Object3D(cubeMesh);

        root->addChild(cube);
    }

    void update() {
        cube->rotateRad(M_PI/180, M_PI/180, M_PI/180);
    }
};