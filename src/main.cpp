#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <algorithm>
#include <fstream>

#include <SFML/Graphics.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>

#include "scene_objects.h"
#include "scenes/cube_scene.h"
#include "scenes/penger_scene.h"
#include "scene.h"
#include "models.h"

const int WIDTH = 1440;
const int HEIGHT = 900;

const glm::mat3x3 transformNdcToScreen = glm::mat3x3(
    WIDTH/2, 0, 0,
    0, -HEIGHT/2, 0,
    WIDTH/2, HEIGHT/2, 1
);

std::stack<glm::mat4x4> modelStack;
std::unordered_set<Object3D*> visited;
std::stack<Object3D*> st;

sf::VertexArray vbo;
Camera camera;
Scene* scene;

/**
 * Создает и записывает в буфер треугольную грань по трем экранным вершинам.
 */
void createFace(sf::VertexArray& vbo, glm::vec2 a, glm::vec2 b, glm::vec2 c, int k) { 
    vbo[k].position = sf::Vector2f(a.x, a.y);
    vbo[k].color = sf::Color::Green;
    vbo[k+1].position = sf::Vector2f(b.x, b.y);
    vbo[k+1].color = sf::Color::Green;

    vbo[k+2].position = sf::Vector2f(b.x, b.y);
    vbo[k+2].color = sf::Color::Green;
    vbo[k+3].position = sf::Vector2f(c.x, c.y);
    vbo[k+3].color = sf::Color::Green;

    vbo[k+4].position = sf::Vector2f(a.x, a.y);
    vbo[k+4].color = sf::Color::Green;
    vbo[k+5].position = sf::Vector2f(c.x, c.y);
    vbo[k+5].color = sf::Color::Green;
}

/**
 * Обновляет состояние текущего обьекта. Записывает преобразованные вершины в буфер вершин.
 * Вызывать строго перед вызовом drawObject3D.
 */
void updateObject3D(Object3D* currentObject) {
    glm::mat4x4& projection = camera.getProjectionMatrix();
    glm::mat4x4& view = camera.getViewMatrix();
    glm::mat4x4& world = modelStack.top();

    std::vector<glm::vec3>& vertices = currentObject->getMesh()->getVertices();
    std::vector<int>& indices = currentObject->getMesh()->getIndices();

    glm::mat4x4 mvp = projection * view * world;
    std::vector<glm::vec3> screenCoordsArr;

    for (glm::vec3& v : vertices) {
        glm::vec4 vertex = glm::vec4(v, 1);
        glm::vec4 clipCoords = mvp * vertex;
        glm::vec3 ndcCoords(clipCoords.x/clipCoords.w, clipCoords.y/clipCoords.w, 1);
        glm::vec3 screenCoords = transformNdcToScreen * ndcCoords;
        screenCoordsArr.push_back(screenCoords);
    }
    
    Mesh* mesh = currentObject->getMesh();
    vbo.resize(mesh->getIndices().size()*2);
    for (int i = 0; i < indices.size(); i += 3) {
        glm::vec2 a = screenCoordsArr[indices[i]], b = screenCoordsArr[indices[i+1]], c = screenCoordsArr[indices[i+2]];
        createFace(vbo, a, b, c, i*2);
    }
}

void updateScene() {
    scene->update();
}

/**
 * Отрисовывает текущий обьект с помощью буфера вершин.
 * Вызывать строго после вызова updateObject3D.
 */
void drawObject3D(sf::RenderWindow& window) {
    window.draw(vbo);
}

/**
 * Сбрасывает предыдущее состояние обхода в глубину по дереву сцены.
 * Вызывать строго перед процедурами nextObject3D.
 */
void resetDFS() {
    while (!st.empty()) st.pop();
    while (!modelStack.empty()) modelStack.pop();
    visited.clear();

    modelStack.push(scene->getRoot()->getModelMatrix());
    st.push(scene->getRoot());
}

/**
 * Возвращает следующий обьект из дерева сцены и обновляет стек модельных матриц.
 * Следует порядку обхода в глубину.
 * Если все обьекты обойдены, то возвращает nullptr.
 * Вызывать строго после вызова resetDFS.
 */
Object3D* nextObject3D() {
    if (st.empty()) return nullptr;
    Object3D* current = st.top();
    
    for (Object3D* child : current->getChildren()) {
        if (visited.find(child) == visited.end()) {
            visited.insert(child);
            modelStack.push(modelStack.top() * child->getModelMatrix());
            st.push(child);
            return child;
        }
    }
    st.pop();
    modelStack.pop();
    return nextObject3D();
}

/**
 * Процедура загрузки и инициализации сцены.
 */
void loadScene(Scene* sc) {
    scene = sc;
    scene->loadMeshes();
    scene->build();
}

/**
 * Процедура уничтожения сцены.
 */
void disposeScene() {
    scene->dispose();
    delete scene;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "3D Graphics");
    window.setFramerateLimit(60);
    
    vbo.setPrimitiveType(sf::PrimitiveType::Lines);
    camera.lookAt(glm::vec3(0,-0.5f,-1), glm::vec3(0,-0.5f,0));
    camera.frustum(-(float)WIDTH/HEIGHT,(float)WIDTH/HEIGHT, -1,1, 0.5,3);

    loadScene(new PengerScene());

    Object3D* currentObject;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        window.clear();
        updateScene();
        resetDFS();
        while ((currentObject = nextObject3D()) != nullptr) {
            updateObject3D(currentObject);
            drawObject3D(window);
        }
        window.display();
    }

    disposeScene();
}
