#include "scene_objects.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

#include <SFML/Graphics.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>

void readVertices(std::vector<float>& vertices, std::string internal);

void readIndices(std::vector<int>& indices, std::string internal);

Mesh* Mesh::EMPTY = new Mesh();

Mesh::Mesh() {
}

Mesh* Mesh::create(const std::vector<float>& vertices, const std::vector<int>& indices) {
    Mesh* instance = new Mesh();
    instance->vertices.resize(vertices.size()/3);
    for (int i = 0; i < vertices.size(); i += 3) {
        glm::vec3 v = {vertices[i], vertices[i+1], vertices[i+2]};
        instance->vertices[i/3] = v;
    }
    instance->indices = indices;
    return instance;
}

Mesh* Mesh::createFromFiles(std::string verticesPath, std::string indicesPath) {
    std::vector<float> vertices;
    std::vector<int> indices;
    readVertices(vertices, verticesPath);
    readIndices(indices, indicesPath);
    return create(vertices, indices);
}

void readVertices(std::vector<float>& vertices, std::string path) {
    std::fstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }
    std::vector<std::string> svertices(
        (std::istream_iterator<std::string>(file)),
        std::istream_iterator<std::string>()
    );
    std::transform(
        svertices.begin(), svertices.end(), 
        std::back_inserter(vertices), [](const std::string& s) { return std::stof(s); }
    );
    file.close();
}

void readIndices(std::vector<int>& indices, std::string path) {
    std::fstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }
    std::vector<std::string> svertices(
        (std::istream_iterator<std::string>(file)),
        std::istream_iterator<std::string>()
    );
    std::transform(
        svertices.begin(), svertices.end(), 
        std::back_inserter(indices), [](const std::string& s) { return std::stoi(s)-1; }
    );
    file.close();
}

std::vector<glm::vec3>& Mesh::getVertices() {
    return vertices;
}

std::vector<int>& Mesh::getIndices() {
    return indices;
}

Camera::Camera() {
    this->view = glm::identity<glm::mat4x4>();
    frustum(-1,1, -1,1, 0.5, 3);
}

void Camera::lookAt(glm::vec3 from, glm::vec3 to) {
    glm::vec3 forward = glm::normalize(from - to);
    glm::vec3 tmp(0, 1, 0);
    glm::vec3 right = glm::cross(tmp, forward);
    glm::vec3 up = glm::cross(forward, right);

    view[0][0] = right.x;
    view[0][1] = right.y;
    view[0][2] = right.z;

    view[1][0] = up.x;
    view[1][1] = up.y;
    view[1][2] = up.z;

    view[2][0] = forward.x;
    view[2][1] = forward.y;
    view[2][2] = forward.z;

    view[3][0] = from.x;
    view[3][1] = from.y;
    view[3][2] = from.z;
}

void Camera::frustum(float left, float right, float bottom, float top, float near, float far) {
    projection = glm::mat4(0);

    projection[0][0] = 2*near / (right - left);
    projection[0][2] = (right + left) / (right - left);

    projection[1][1] = 2*near / (top - bottom);
    projection[1][2] = (top + bottom) / (top - bottom);

    projection[2][2] = -(far + near) / (far - near);
    projection[2][3] = -2*far*near / (far - near);

    projection[3][2] = -1;
}

glm::mat4x4& Camera::getViewMatrix() {
    return view;
}

glm::mat4x4& Camera::getProjectionMatrix() {
    return projection;
}

Object3D::Object3D(Mesh* mesh) {
    this->model = glm::identity<glm::mat4x4>();
    this->mesh = mesh;
}

Object3D::~Object3D() {
    for (Object3D* child : children) {
        delete child;
    }
}

void Object3D::addChild(Object3D* child) {
    if (find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void Object3D::removeChild(Object3D* child) {
    children.erase(find(children.begin(), children.end(), child));
}

void Object3D::rotateRad(float x, float y, float z) {
    glm::mat3x3 rotation = glm::mat3x3(
        cosf(x), -sinf(x), 0,
        sinf(x), cosf(x), 0,
        0, 0, 1
    ) * glm::mat3x3(
        cosf(y), 0, sinf(y),
        0, 1, 0,
        -sin(y), 0, cosf(y)
    ) * glm::mat3x3(
        1, 0, 0,
        0, cosf(z), -sinf(z),
        0, sinf(z), cosf(z)
    );
    model *= glm::mat4x4(rotation);
}

void Object3D::translate(glm::vec3 t) {
    glm::mat4x4 translation = glm::mat4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t.x, t.y, t.z, 1
    );
    model *= translation;
}

void Object3D::scale(float x, float y, float z) {
    glm::mat3x3 homothety = glm::mat3x3(
        x, 0, 0,
        0, y, 0,
        0, 0, z
    );
    model *= glm::mat4x4(homothety);
}

std::vector<Object3D*>& Object3D::getChildren() {
    return children;
}

glm::mat4x4& Object3D::getModelMatrix() {
    return model;
}

Mesh* Object3D::getMesh() {
    return mesh;
}
