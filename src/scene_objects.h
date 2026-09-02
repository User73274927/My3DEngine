#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <glm/glm.hpp>

struct Mesh {
private:
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;

    Mesh();

public:
    static Mesh* EMPTY;

    /** 
     * Метод создания экземпляра по списку вершин и индексов.
     */
    static Mesh* create(const std::vector<float>& vertices, const std::vector<int>& indices);

    /**
     * Метод создания экземпляра меша по текстовым файлам c вершинами и индексами.
     */
    static Mesh* createFromFiles(std::string indicesPath, std::string verticesPath);

    /**
     * Возвращает список вершин данного меша.
     */
    std::vector<glm::vec3>& getVertices();

    /**
     * Возвращает список индексов данного меша.
     */
    std::vector<int>& getIndices();
};

struct Object3D {
private:
    std::vector<Object3D*> children;
    Object3D* parent;

    Mesh* mesh;
    glm::mat4x4 model;

public:
    Object3D(Mesh* mesh);
    
    ~Object3D();

    /**
     * Добавляет к нему дочерний обьект.
     */
    void addChild(Object3D* child);
    
    /**
     * Удаляет дочерний обьект если он существует.
     */
    void removeChild(Object3D* child);

    /**
     * Выполняет преобразование параллельного переноса на вектор t.
     */
    void translate(glm::vec3 t);
    
    /**
     * Выполняет преобразование гомотетии.
     */
    void scale(float x, float y, float z);
    
    /**
     * Выполняет преобразование поворота по координатным осям Ox, Oy, Oz.
     */
    void rotateRad(float x, float y, float z);

    /**
     * Возвращает список дочерних обьектов.
     */
    std::vector<Object3D*>& getChildren();

    /**
     * Возвращает обьект, к которому данных обьект подчинен.
     */
    Object3D* getParent();

    /**
     * Возвращает модельную матрицу.
     */
    glm::mat4x4& getModelMatrix();

    /**
     * Возвращает используемый данным обьектом меш.
     */
    Mesh* getMesh();
};

struct Camera {
private:
    glm::mat4x4 view, projection;

public:
    Camera();

    /**
     * Метод установки положения и направления обзора камеры.
     * from - положение камеры, to - точка на которую направлена камера.
     */
    void lookAt(glm::vec3 from, glm::vec3 to);

    /**
     * Задает параметры области видимости камеры в форме усеченной пирамиды.
     */
    void frustum(float left, float right, float bottom, float top, float near, float far);

    /**
     * Возвращает матрицу вида.
     */
    glm::mat4x4& getViewMatrix();

    /**
     * Возвращает матрицу проекции.
     */
    glm::mat4x4& getProjectionMatrix();
};
