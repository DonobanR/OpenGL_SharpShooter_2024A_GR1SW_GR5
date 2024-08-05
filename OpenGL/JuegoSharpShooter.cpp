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
void drawM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& m4);
void drawDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& deagle);
void drawBayonet(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& bayonet);
void drawReticle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& reticle2d);
void drawLogo(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& logo);
void drawSkybox(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& skybox);
void drawShootDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& shootD);
void drawShootM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& shootM);
void drawField(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& field);
void drawLamp1(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& lamp1);
void drawLamp2(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& lamp2);

void shootRayFromCamera(Camera& camera, Model& target, glm::mat4& targetModelMatrix);
bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
bool intersectsTargetRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const Model& model);
void checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::mat4& targetModelMatrix, const Model& target);
void repositionTarget(glm::mat4& modelMatrix, const glm::vec3& currentPosition);

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

#ifdef _APPLE_
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
    Model pistola1("model/deagle/deagle.gltf");
    Model pistolaM4("model/m4/m4.gltf");
    Model cielo("model/skybox/skybox.gltf");
    Model blanco("model/blanco/scene.gltf");
    Model blanco2("model/blanco2/scene.gltf");
    Model logo("model/logo/logo.gltf");
    Model cuchillo("model/bayonet/scene.gltf");
    Model mirilla("model/mira4/miragreen.gltf");
    Model animacionDisparo("model/shoot/shootD.gltf");
    Model animacionDisparo2("model/shoot/shootM.gltf");
    Model escenario("model/escenario2/scene.gltf");
    Model lamp("model/lamp/lamp.gltf");
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

        ourShader.use();
        // Se activa activar el shader para configurar las variables uniformes/dibujar objetos
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 100.0f);


        // Luz direccional para el sol
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // Punto de luz [LÁMPARA 1] izquierda
        ourShader.setVec3("pointLights[0].position", posLamp1.x + 0.5f, -(posLamp1.y) + 1.0f, posLamp1.z + 4.0f);
        ourShader.setVec3("pointLights[0].ambient", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].diffuse", 1.0f, 0.82f, 0.0f); //cambio de color
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.05); //distancia luz-fragmento
        ourShader.setFloat("pointLights[0].quadratic", 0.00000000001); //atenuación con la distancia

        // Punto de luz [LÁMPARA 2] derecha
        ourShader.setVec3("pointLights[1].position", posLamp2.x + 0.5f, -(posLamp2.y) + 1.0f, posLamp1.z + 4.0f);
        ourShader.setVec3("pointLights[1].ambient", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[1].diffuse", 1.0f, 0.82f, 0.0f);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.05);
        ourShader.setFloat("pointLights[1].quadratic", 0.00000000001);

        // view / projection / transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Target
        ourShader.setMat4("model", targetModelMatrix);
        blanco.Draw(ourShader);

        // Sbybox
        drawSkybox(ourShader, view, projection, cielo);

        // Mira
        drawReticle(ourShader, view, projection, mirilla);

        // Logo
        //drawLogo(ourShader, view, projection, logo);

        // Field
        drawField(ourShader, view, projection, escenario);

        // Lampara 1
        drawLamp1(ourShader, view, projection, lamp);

        // Lampara 2
        drawLamp2(ourShader, view, projection, lamp);

        // Dibujar el arma seleccionada
        if (showDeagle) {
            drawDeagle(ourShader, view, projection, pistola1);
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                bool played = PlaySound(TEXT("pistola2.wav"), NULL, SND_ASYNC);
            }
        }
        else if (showM4) {
            drawM4(ourShader, view, projection, pistolaM4);
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                bool played = PlaySound(TEXT("M4.wav"), NULL, SND_ASYNC);
            }
        }
        else if (showBayonet) {
            drawBayonet(ourShader, view, projection, cuchillo);
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                bool played = PlaySound(TEXT("CZ75.wav"), NULL, SND_ASYNC);
            }
        }

        // Manejar la lógica del disparo
        if (isShooting) {
            shootTime += deltaTime; // Actualiza el tiempo desde que se disparó

            if (shootTime < shootDuration) {
                // Dibuja el efecto de disparo dependiendo del arma seleccionada
                if (showDeagle) {
                    drawShootDeagle(ourShader, view, projection, animacionDisparo);
                }
                else if (showM4) {
                    drawShootM4(ourShader, view, projection, animacionDisparo2);
                }
                // El bayonet no tiene efecto de disparo
            }
            else {
                isShooting = false; // Termina el efecto de disparo
            }
        }

        // Verificar la acción de disparo
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            shootRayFromCamera(camera, blanco, targetModelMatrix);
        }
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
void processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float currentCameraY = camera.Position.y; // Guardar la posición Y actual de la cámara

    glm::vec3 newPosition = camera.Position;

    // Movimiento y Restriccón de movimiento
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPosition += camera.Front * camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPosition -= camera.Front * camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPosition -= camera.Right * camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPosition += camera.Right * camera.MovementSpeed * deltaTime;

    // Restringe la posición en el eje X
    if (newPosition.x < -180.0f) newPosition.x = -180.0f;
    if (newPosition.x > 180.0f) newPosition.x = 180.0f;

    // Restringe la posición en el eje Z
    if (newPosition.z < -290.0f) newPosition.z = -290.0f;
    if (newPosition.z > 680.0f) newPosition.z = 680.0f;

    // Aplica la posición ajustada, manteniendo constante la altura (eje Y)
    camera.Position.x = newPosition.x;
    camera.Position.z = newPosition.z;

    // Mantener la altura constante
    camera.Position.y = currentCameraY; // Restablecer la posición Y de la cámara a su valor original

    // Alternar entre las armas
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        showDeagle = false;
        showM4 = true;
        showBayonet = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        showDeagle = true;
        showM4 = false;
        showBayonet = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        showDeagle = false;
        showM4 = false;
        showBayonet = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glm::mat4 targetModelMatrix;
        shootRayFromCamera(camera, blanco, targetModelMatrix);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isShooting) {
        isShooting = true; // Establece el estado de disparo a verdadero
        shootTime = 0.0f; // Reinicia el contador de tiempo de disparo

        // El resto de la lógica de disparo permanece igual
        glm::mat4 targetModelMatrix;
        shootRayFromCamera(camera, blanco, targetModelMatrix);
    }
}

void shootRayFromCamera(Camera& camera, Model& target, glm::mat4& targetModelMatrix) {
    glm::vec3 rayOrigin = camera.Position;
    glm::vec3 rayDirection = camera.Front;
    checkRayIntersection(rayOrigin, rayDirection, targetModelMatrix, target);
}


bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
    const float EPSILON = 0.0000001f;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = glm::cross(rayDir, edge2);
    a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // El rayo es paralelo al triángulo.
    f = 1.0 / a;
    s = rayOrigin - v0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(rayDir, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // En este punto sabemos que hay una intersección en la línea del rayo, pero no si el rayo realmente la intersecta.
    t = f * glm::dot(edge2, q);
    if (t > EPSILON) // Intersección con el rayo
        return true;

    return false;
}

bool intersectsTargetRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const Model& model, const glm::mat4& modelMatrix) {
    for (const Mesh& mesh : model.meshes) {
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(mesh.vertices[mesh.indices[i]].Position, 1.0));
            glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(mesh.vertices[mesh.indices[i + 1]].Position, 1.0));
            glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(mesh.vertices[mesh.indices[i + 2]].Position, 1.0));

            float t = 0.0f;
            if (intersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, t)) {
                return true; // Colisiona
            }
        }
    }
    return false; // No colisiona
}

void checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::mat4& targetModelMatrix, const Model& target) {
    if (intersectsTargetRayTriangle(rayOrigin, rayDirection, target, targetModelMatrix)) {
        // Extracción de la posición actual del modelo
        glm::vec3 currentPosition = glm::vec3(targetModelMatrix[3][0], targetModelMatrix[3][1], targetModelMatrix[3][2]);


        // Llamar a repositionTarget con la matriz del modelo y la posición actual
        repositionTarget(targetModelMatrix, currentPosition);
    }
}


//cambio de posicion de mi blanco en cada disparo
void repositionTarget(glm::mat4& modelMatrix, const glm::vec3& currentPosition) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(-100.0, 180.0); // Límite en el eje X
    std::uniform_real_distribution<> disY(50.0, 80.0);  // Límite en el eje Y
    std::uniform_real_distribution<> disZ(-300.0, 600.0); // Desplazamiento en el eje Z

    // Generar nueva posición dentro de los límites específicos
    float newZ = currentPosition.z + disZ(gen);
    glm::vec3 newPosition = glm::vec3(disX(gen), disY(gen), currentPosition.z + disZ(gen));

    // Asegurar que el valor de Z no exceda los límites globales
    float zMinGlobal = -100.0f;
    float zMaxGlobal = 400.0f;

    newPosition.z = glm::clamp(newPosition.z, zMinGlobal, zMaxGlobal);

    // Restablecer la matriz del modelo para aplicar la nueva posición
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, newPosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));

    // Cambiar la orientación y escala del target
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

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



// Dibujar Deagle
void drawDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& deagle) {
    glm::mat4 pistolaMatrix = glm::mat4(1.0f);
    pistolaMatrix = glm::translate(pistolaMatrix, glm::vec3(0.21f, -0.4f, -0.38f));
    pistolaMatrix = glm::rotate(pistolaMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    pistolaMatrix = glm::rotate(pistolaMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    pistolaMatrix = glm::scale(pistolaMatrix, glm::vec3(0.06f));
    pistolaMatrix = glm::inverse(view) * pistolaMatrix;
    shader.setMat4("model", pistolaMatrix);
    deagle.Draw(shader);
}

// Dibujar M4
void drawM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& m4) {
    glm::mat4 armaMatrix = glm::mat4(1.0f);
    armaMatrix = glm::translate(armaMatrix, glm::vec3(0.28f, -0.7f, -0.1f));
    armaMatrix = glm::rotate(armaMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    armaMatrix = glm::scale(armaMatrix, glm::vec3(0.04f));
    armaMatrix = glm::inverse(view) * armaMatrix;
    shader.setMat4("model", armaMatrix);
    m4.Draw(shader);
}

// Dibujar Bayonet
void drawBayonet(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& bayonet) {
    glm::mat4 cuchilloMatrix = glm::mat4(1.0f);
    cuchilloMatrix = glm::translate(cuchilloMatrix, glm::vec3(0.5f, -1.0f, -1.0f));
    cuchilloMatrix = glm::rotate(cuchilloMatrix, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cuchilloMatrix = glm::rotate(cuchilloMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    cuchilloMatrix = glm::rotate(cuchilloMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cuchilloMatrix = glm::scale(cuchilloMatrix, glm::vec3(0.14f));
    cuchilloMatrix = glm::inverse(view) * cuchilloMatrix;
    shader.setMat4("model", cuchilloMatrix);
    bayonet.Draw(shader);
}
// Dibujar Skybox
void drawSkybox(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& skybox) {
    glm::mat4 skyboxMatrix = glm::mat4(1.0f);
    skyboxMatrix = glm::translate(skyboxMatrix, glm::vec3(1.0f, 0.0f, 1.0f));
    skyboxMatrix = glm::rotate(skyboxMatrix, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    skyboxMatrix = glm::rotate(skyboxMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    skyboxMatrix = glm::scale(skyboxMatrix, glm::vec3(800.0f));
    shader.setMat4("model", skyboxMatrix);
    skybox.Draw(shader);
}

// Dibujar Disparo Deagle
void drawShootDeagle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& shootDeagle) {
    glm::mat4 shootDeagleMatrix = glm::mat4(1.0f);
    shootDeagleMatrix = glm::translate(shootDeagleMatrix, glm::vec3(0.32f, -0.22f, -1.50f));
    shootDeagleMatrix = glm::rotate(shootDeagleMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shootDeagleMatrix = glm::rotate(shootDeagleMatrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shootDeagleMatrix = glm::scale(shootDeagleMatrix, glm::vec3(0.001f));
    shootDeagleMatrix = glm::inverse(view) * shootDeagleMatrix;
    shader.setMat4("model", shootDeagleMatrix);
    shootDeagle.Draw(shader);
}

// Dibujar Disparo M4
void drawShootM4(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& shootM4) {
    glm::mat4 shootM4Matrix = glm::mat4(1.0f);
    shootM4Matrix = glm::translate(shootM4Matrix, glm::vec3(0.27f, -0.20f, -1.65f));
    shootM4Matrix = glm::rotate(shootM4Matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shootM4Matrix = glm::rotate(shootM4Matrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shootM4Matrix = glm::scale(shootM4Matrix, glm::vec3(0.001f));
    shootM4Matrix = glm::inverse(view) * shootM4Matrix;
    shader.setMat4("model", shootM4Matrix);
    shootM4.Draw(shader);
}

// Dibujar Reticula
void drawReticle(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& reticle2d) {
    glm::mat4 reticleMatrix = glm::mat4(1.0f);
    reticleMatrix = glm::translate(reticleMatrix, glm::vec3(0.0f, 0.0f, -0.30f));
    reticleMatrix = glm::rotate(reticleMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    reticleMatrix = glm::rotate(reticleMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    reticleMatrix = glm::scale(reticleMatrix, glm::vec3(0.0015f));
    reticleMatrix = glm::inverse(view) * reticleMatrix;
    shader.setMat4("model", reticleMatrix);
    reticle2d.Draw(shader);
}

// Dibujar Logo
void drawLogo(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& logo) {
    glm::mat4 logoMatrix = glm::mat4(1.0f);
    logoMatrix = glm::translate(logoMatrix, glm::vec3(20.0f, 50.0f, 20.0f));
    logoMatrix = glm::scale(logoMatrix, glm::vec3(1000.0f));
    shader.setMat4("model", logoMatrix);
    logo.Draw(shader);
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


// Lampara 1
void drawLamp1(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& lamp1) {
    glm::mat4 lamp1Matrix = glm::mat4(1.0f);
    lamp1Matrix = glm::translate(lamp1Matrix, posLamp1);
    lamp1Matrix = glm::rotate(lamp1Matrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    lamp1Matrix = glm::rotate(lamp1Matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    lamp1Matrix = glm::rotate(lamp1Matrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lamp1Matrix = glm::scale(lamp1Matrix, glm::vec3(1.0f));
    shader.setMat4("model", lamp1Matrix);
    lamp1.Draw(shader);
}

// Lampara 2
void drawLamp2(Shader& shader, glm::mat4& view, glm::mat4& projection, Model& lamp2) {
    glm::mat4 lamp2Matrix = glm::mat4(1.0f);
    lamp2Matrix = glm::translate(lamp2Matrix, posLamp2);
    lamp2Matrix = glm::rotate(lamp2Matrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    lamp2Matrix = glm::rotate(lamp2Matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    lamp2Matrix = glm::rotate(lamp2Matrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lamp2Matrix = glm::scale(lamp2Matrix, glm::vec3(1.0f));
    shader.setMat4("model", lamp2Matrix);
    lamp2.Draw(shader);
}
