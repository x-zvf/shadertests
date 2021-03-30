//#define GLFW_INCLUDE_GLCOREARB 1

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
//#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>


static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


char *file_string(char *filepath) {
    
    int fd = open(filepath, O_RDONLY);
    if(fd < 0) {
        perror("Failed to open file");
        exit(1);
    }
    int len = lseek(fd, 0, SEEK_END);
    void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if(data == MAP_FAILED) {
        perror("Failed to load shader");
        exit(1);
    }
    return (char *)data;
}

int main(void)
{
    GLFWwindow *window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    
//    if(!glewInit())
//        exit(EXIT_FAILURE);
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Slime", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    fprintf(stderr, "Hello, line: %d\n", __LINE__);
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    //gladLoadGL(glfwGetProcAddress);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // ==================== shader stuff ================================
	GLuint pheromoneTexHandle;
	GLuint agentPosTexHandle;
	GLuint agentPosVelHandle;
    unsigned int pheromoneTexture = 1;
    unsigned int agentPosTexture = 2;
    unsigned int agentVelTexture = 3;
	glGenTextures(1, &pheromoneTexHandle);
	glActiveTexture(pheromoneTexture);
	glBindTexture(GL_TEXTURE_2D, pheromoneTexHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 640, 480, 0, GL_RED, GL_FLOAT, NULL);
	glBindImageTexture(pheromoneTexture, pheromoneTexHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glGenTextures(2, &pheromoneTexHandle);
	glActiveTexture(pheromoneTexture);
	glBindTexture(GL_TEXTURE_2D, pheromoneTexHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 640, 480, 0, GL_RED, GL_FLOAT, NULL);
	glBindImageTexture(pheromoneTexture, pheromoneTexHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);


    fprintf(stderr, "Hello, line: %d\n", __LINE__);
    GLint shaderStatus;
    
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    GLchar *computeShaderSource = file_string("computeShader.glsl");
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    glGetShaderiv(computeShader,GL_COMPILE_STATUS, &shaderStatus);
    if(shaderStatus != GL_TRUE) {
        GLchar glerr[8192];
        GLsizei len;
        glGetShaderInfoLog(computeShader, sizeof(glerr)-1, &len, glerr);
        fprintf(stderr,"Failed to compile compute shader:%s",glerr);
        exit(2);
    }
    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    glGetProgramiv(computeProgram,GL_LINK_STATUS, &shaderStatus);
    if(shaderStatus != GL_TRUE) {
        GLchar glerr[8192];
        GLsizei len;
        glGetProgramInfoLog(computeProgram, sizeof(glerr)-1, &len, glerr);
        fprintf(stderr,"Failed to link compute shader program:%s",glerr);
        exit(2);
    }
    glUseProgram(computeProgram);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *vertexShaderSource = file_string("vertexShader.glsl");
    GLchar *fragmentShaderSource = file_string("fragmentShader.glsl");

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader,GL_COMPILE_STATUS, &shaderStatus);
    if(shaderStatus != GL_TRUE) {
        GLchar glerr[8192];
        GLsizei len;
        glGetShaderInfoLog(vertexShader, sizeof(glerr)-1, &len, glerr);
        fprintf(stderr,"Failed to compile vertex shader:%s",glerr);
        exit(2);
    }
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS, &shaderStatus);
    if(shaderStatus != GL_TRUE) {
        GLchar glerr[8192];
        GLsizei len;
        glGetShaderInfoLog(fragmentShader, sizeof(glerr)-1, &len, glerr);
        fprintf(stderr,"Failed to compile fragment shader:%s",glerr);
        exit(2);
    }

    GLuint renderProgram = glCreateProgram();
    glAttachShader(renderProgram, vertexShader);
    glAttachShader(renderProgram, fragmentShader);

    glBindFragDataLocation(renderProgram, 0, "color");

    glLinkProgram(renderProgram);

    glGetProgramiv(renderProgram,GL_LINK_STATUS, &shaderStatus);
    if(shaderStatus != GL_TRUE) {
        GLchar glerr[8192];
        GLsizei len;
        glGetProgramInfoLog(renderProgram, sizeof(glerr)-1, &len, glerr);
        fprintf(stderr,"Failed to link render shader program:%s",glerr);
        exit(2);
    }
    
    //link compute shader and render shader textures
    glUniform1i(glGetUniformLocation(renderProgram, "phTex"), pheromoneTexture);
    glUniform1i(glGetUniformLocation(computeProgram, "destTex"), pheromoneTexture);

	GLuint vertArray;
    glGenVertexArrays(1, &vertArray);
	glBindVertexArray(vertArray);

	GLuint posBuf;
	glGenBuffers(1, &posBuf);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	float data[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, data, GL_STREAM_DRAW);
	GLint posPtr = glGetAttribLocation(renderProgram, "pos");
    glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posPtr);



    int i = 1;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        //fprintf(stderr, "Hello, line: %d\n", __LINE__);
        /* Render here */
        glClearColor(0.2f, 0.9f, 0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

         // launch compute shaders!
        glUseProgram(computeProgram);
       	glUniform1f(glGetUniformLocation(computeProgram, "roll"), (float)i*0.01f);
        glDispatchCompute(512/16, 512/16, 1); // 512^2 threads in blocks of 16^2

        // make sure writing to image has finished before read
        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        glUseProgram(renderProgram);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
        i++;
    }

    glfwTerminate();
    return 0;
}