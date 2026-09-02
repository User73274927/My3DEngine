#pragma once

#include "../scene.h"
#include "../models.h"

/**
 * Сцена с вращающимся по оси Oy моделью пингвина.
 */
struct PengerScene : Scene {
private:
    Mesh* pengerMesh;

    Object3D* penger;

public:
    void loadMeshes() {
        pengerMesh = Mesh::createFromFiles(
            "/Users/dzunko/Программирование/My3DEngine/res/penger-vertices.txt",
            "/Users/dzunko/Программирование/My3DEngine/res/penger-indices.txt"
        );
    }

    void build() {
        penger = new Object3D(pengerMesh);

        root->addChild(penger);
    }

    void update() {
        penger->rotateRad(0, M_PI/180, 0);
    }

    void dispose() {
        delete pengerMesh;
    }
};