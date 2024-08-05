#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>
#include <vector>
#include <random>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>
#include <SFML/Audio.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int loadTexture(const char* path);

void drawField(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& field);





// Settings FHD
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Settings 2K
//const unsigned int SCR_WIDTH = 2560;
//const unsigned int SCR_HEIGHT = 1440;

// Camera
Camera camera(glm::vec3(120.0f, 15.0f, -120.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Visualizar armas
bool showDeagle = false;
bool showM4 = false;
bool showBayonet = true;

// Visualizar disparo
bool isShooting = false; // Estado del disparo
float shootTime = 0.0f; // Tiempo desde que se disparó
float shootDuration = 0.1f; // Duración visible del disparo

Model blanco;
glm::mat4 targetModelMatrix = glm::mat4(1.0f);

// posición de las lámparas
glm::vec3 posLamp1 = glm::vec3(-180.0f, -1.2f, 20.0f);
glm::vec3 posLamp2 = glm::vec3(180.0f, -1.2f, 20.0f);

float speed = 10.0f; // Velocidad de movimiento
float range = 300.0f; // Rango de movimiento en el eje Z
float direction = 1.0f; // Dirección del movimiento (1 para derecha, -1 para izquierda)

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ProyectoExamen", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader ourShader("shaders/shader_exercise16_mloading.vs", "shaders/shader_exercise16_mloading.fs");
    Shader textureShader("shaders/texture_shader.vs", "shaders/texture_shader.fs");



    //carga de cuadrado
    float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1 = loadTexture("textures/instrucciones2.png");
    //unsigned int texture2 = loadTexture("textures/container2_specular.png");

    // load models

    Model escenario("model/escenario2/scene.gltf");

    blanco = Model("model/blanco/scene.gltf");

    targetModelMatrix = glm::mat4(1.0f);
    targetModelMatrix = glm::translate(targetModelMatrix, glm::vec3(30.0f, 50.0f, 50.0f));
    targetModelMatrix = glm::rotate(targetModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    targetModelMatrix = glm::rotate(targetModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    targetModelMatrix = glm::scale(targetModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 targetModelMatrix2 = glm::mat4(1.0f);
    targetModelMatrix2 = glm::translate(targetModelMatrix2, glm::vec3(-30.0f, 50.0f, 50.0f));
    targetModelMatrix2 = glm::rotate(targetModelMatrix2, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    targetModelMatrix2 = glm::scale(targetModelMatrix2, glm::vec3(3.0f, 3.0f, 3.0f));

    camera.MovementSpeed = 100;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;



        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // Render the square
        glBindVertexArray(VAO);
        textureShader.use(); // Usa el nuevo shader
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-10.0f, 50.0f, -300.0f));
        model = glm::scale(model, glm::vec3(400.0f, -120.0f, 10.0f));
        textureShader.setMat4("model", model);
        textureShader.setMat4("view", camera.GetViewMatrix());
        textureShader.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1500.0f));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        ourShader.setMat4("model", targetModelMatrix);
        blanco.Draw(ourShader);

        // render container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Deallocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



// Dibujar Field
void drawField(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& field) {
    glm::mat4 fieldMatrix = glm::mat4(1.0f); // Initialize the transformation matrix to identity

    // Apply transformations (excluding translation)
    fieldMatrix = glm::rotate(fieldMatrix, glm::radians(120.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate 120 degrees around the z-axis
    fieldMatrix = glm::rotate(fieldMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate 90 degrees around the y-axis
    fieldMatrix = glm::rotate(fieldMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 30 degrees around the x-axis
    fieldMatrix = glm::scale(fieldMatrix, glm::vec3(1.0f)); // Scale the field down by a factor of 0.2

    // Set the transformed matrix to the shader
    shader.setMat4("model", fieldMatrix);

    // Draw the field model using the shader
    field.Draw(shader);
}