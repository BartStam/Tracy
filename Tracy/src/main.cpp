#include "pch.h"
#include "cores/easycore/easycore.h"

int main() {
    int windowWidth = 960;
    int windowHeight = 540;

    // GLFW
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(windowWidth, windowHeight, "Tracy", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error: failed to initialize OpenGL context." << std::endl;
        return -1;
    }

    glViewport(0, 0, windowWidth, windowHeight);

    // Buffers
    const float vertices[] = {
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,   // Top right
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // Bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // Bottom left
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f    // Top left 
    };

    const unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int vertexArray, vertexBuffer, elementBuffer;

    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &elementBuffer);

    glBindVertexArray(vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Texture
    unsigned int frameTexture;
    glGenTextures(1, &frameTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Render core
    EasyCore renderCore(windowWidth, windowHeight);

    // Materials
    std::vector<CoreMaterial> materialData;
    CoreMaterial sphereMaterial{ glm::vec3(1.0f, 0.0f, 0.5f), 1.0f };
    materialData.push_back(sphereMaterial);

    CoreMaterial floorMaterial{ glm::vec3(0.5f, 0.5, 0.65f), 1.0f };
    materialData.push_back(floorMaterial);

    CoreMaterial lightMaterial{ glm::vec3(8.0f, 8.0f, 20.0f), 1.0f };
    materialData.push_back(lightMaterial);

    // Spheres
    std::vector<CoreSphere> sphereData;
    CoreSphere sphere1{glm::vec3(0.0f, 1.0f, 2.0f), 1.0f, 0};
    sphereData.push_back(sphere1);
    
    // Floor
    std::vector<CoreTriangle> triangleData;
    glm::vec3 v0 = glm::vec3( 5.0f, 0.0f,  5.0f);
    glm::vec3 v1 = glm::vec3( 5.0f, 0.0f, -5.0f);
    glm::vec3 v2 = glm::vec3(-5.0f, 0.0f,  5.0f);
    glm::vec3 n = tmath::triangleNormal(v0, v1, v2);
    CoreTriangle triangle0{ v0, v1, v2, n, 1 };
    triangleData.push_back(triangle0);

    v0 = glm::vec3( 5.0f, 0.0f, -5.0f);
    v1 = glm::vec3(-5.0f, 0.0f, -5.0f);
    v2 = glm::vec3(-5.0f, 0.0f,  5.0f);
    n = tmath::triangleNormal(v0, v1, v2);
    CoreTriangle triangle1{ v0, v1, v2, n, 1 };
    triangleData.push_back(triangle1);

    // Overhead light
    v2 = glm::vec3(5.0f, 5.0f, 5.0f);
    v1 = glm::vec3(5.0f, 5.0f, -5.0f);
    v0 = glm::vec3(-5.0f, 5.0f, 5.0f);
    n = tmath::triangleNormal(v0, v1, v2);
    CoreTriangle triangle2{ v0, v1, v2, n, 2 };
    triangleData.push_back(triangle2);

    v2 = glm::vec3(5.0f, 5.0f, -5.0f);
    v1 = glm::vec3(-5.0f, 5.0f, -5.0f);
    v0 = glm::vec3(-5.0f, 5.0f, 5.0f);
    n = tmath::triangleNormal(v0, v1, v2);
    CoreTriangle triangle3{ v0, v1, v2, n, 2 };
    triangleData.push_back(triangle3);

    renderCore.setMaterialData(materialData);
    renderCore.setSphereData(sphereData);
    renderCore.setTriangleData(triangleData);
    renderCore.setSkydomeData("res/textures/skydome3.png");

    // Draw loop
    while (!glfwWindowShouldClose(window)) {
        renderCore.processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        std::vector<uint8_t> frame = renderCore.nextFrame();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data());

        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
