#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *faceShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";
const char *fragmentShader1Source = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

const char *edgeShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);\n"
    "   ourColor = vec3(0.0, 0.0, 0.0);\n"
    "}\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // we skipped compile log checks this time for readability (if you do encounter issues, add the compile-checks! see previous code samples)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int faceFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
    unsigned int edgeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // the second fragment shader that outputs the color yellow
    unsigned int faceShaderProgram = glCreateProgram(), edgeShaderProgram = glCreateProgram();

    glShaderSource(vertexShader, 1, &faceShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(faceFragmentShader, 1, &fragmentShader1Source, NULL);
    glCompileShader(faceFragmentShader);
    // link the first program object
    glAttachShader(faceShaderProgram, vertexShader);
    glAttachShader(faceShaderProgram, faceFragmentShader);
    glLinkProgram(faceShaderProgram);

    // glShaderSource(vertexShader, 1, &edgeShaderSource, NULL);
    // glCompileShader(vertexShader);
    // glShaderSource(edgeFragmentShader, 1, &fragmentShader1Source, NULL);
    // glCompileShader(edgeFragmentShader);
    // // link the second program object using a fragment shader that outputs the color yellow
    // glAttachShader(edgeShaderProgram, vertexShader);
    // glAttachShader(edgeShaderProgram, edgeFragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float faces[] = {
        -0.9, -0.5, 0.0, 1.0, 1.0,
        -0.0, -0.5, 1.0, 0.0, 1.0,
        -0.45, 0.5, 1.0, 1.0, 0.0,

    };
    float edges[] = {
        -0.9, -0.5, -0.0, -0.5,
        -0.45, 0.5, -0.9, -0.5,
        -0.45, 0.5, -0.0, -0.5,
    };

    unsigned int VBO[2], VAO[2];
    glGenVertexArrays(2, VAO); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(2, VBO);

    // faces setup
    // --------------------
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // edges setup
    // --------------------
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // now when we draw the triangle we first use the vertex and orange fragment shader from the first program
        glUseProgram(faceShaderProgram);
        // draw the first triangle using the data from our first VAO
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(faces) / sizeof(float));
        // then we draw the second triangle using the data from the second VAO
        // when we draw the second triangle we want to use a different shader program so we switch to the shader program with our yellow fragment shader.
        glUseProgram(edgeShaderProgram);
        // draw the first triangle using the data from our first VAO
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_LINES, 0, sizeof(edges) / sizeof(float));
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
    glDeleteProgram(faceShaderProgram);
    // glDeleteProgram(shaderProgramYellow);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}