#include <defines.h>
#include <shader.h>


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


#include <picking.h>

PickingTexture* g_PickingTexture;
unsigned current_fbo;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;

        // Read the pixel at the mouse position in the picking texture
        auto res = g_PickingTexture->read((unsigned int)xpos, SCR_HEIGHT-(unsigned int)ypos);


        // std:cout <

    }
}



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    

    




    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader;
    const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform vec4 color;\n"
    "out vec4 ourColor;\n"
    "void main()"
    "{"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    "   if (color.x == 0.0 && color.y == 0.0 && color.z == 0.0 && color.w == 0.0) {"
    "       ourColor = vec4(aColor,1);"
    "   } else {"
    "       ourColor = color;"
    "   }"
    "}";

    const char* fragmentShaderSource = 
    "#version 330 core\n"
    "in vec4 ourColor;\n"
    "out vec4 FragColor;\n"
    "void main()"
    "{"
    "   FragColor = ourColor;"
    "}";


    glReadBuffer(GL_NONE);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    

    ourShader.initWithCode(vertexShaderSource, fragmentShaderSource);
    Shader ourShader2("shader/selector.vs", "shader/selector.fs");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 1.0f,  0.0f, 0.0f, 1.0f,   // top 
         
         0.2f, -0.2f, 0.0f,  0.0f, 1.0f, 1.0f,   // bottom right
        -0.2f, -0.2f, 0.0f,  1.0f, 0.0f, 1.0f,   // bottom left
         0.0f,  0.2f, 0.0f,  1.0f, 1.0f, 0.0f    // top 

    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // bind mouse button callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);



    g_PickingTexture = new PickingTexture;

    PickingTexture& pickingTexture = *g_PickingTexture;


    pickingTexture.init(SCR_WIDTH, SCR_HEIGHT);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        pickingTexture.activate();
            glViewport(0, 0, SCR_WIDTH/2, SCR_HEIGHT/2);
            glScissor(0, 0, SCR_WIDTH/2, SCR_HEIGHT/2);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ourShader2.use();
            ourShader2.setMat4("model", glm::mat4(1.0f));
            ourShader2.setMat4("camera",glm::mat4(1.0f));
            ourShader2.setVec3("ID", {1.0f, 2.0f, 3.0f});
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        pickingTexture.deactivate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    delete g_PickingTexture;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}