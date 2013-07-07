#define GL_GLEXT_PROTOTYPES
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <stdio.h>

GLFWwindow* glfwWindow = NULL;

float rwidth, rheight;
int mouseLocation;
int resolutionLocation;
int timeLocation;

int readfilecontents(const char *filename, char *out, size_t maxlen)
{
    FILE *fp;
    size_t len;
    fp = fopen(filename, "r");
    if(!fp)
        return -1;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if(len+1 > maxlen)
        return -2;
    fread(out, len, 1, fp);
    out[len] = 0;
    fclose(fp);
    return 0;
}

GLuint create_shader(const char *filename, GLenum type)
{
    char buffer[16384];
    const char *shader_code;
    GLuint shader;
    shader_code = buffer;
    readfilecontents(filename, buffer, sizeof(buffer));	
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_code, 0);
    glCompileShader(shader);
    return shader;
}

GLuint create_program(const char *vert_shader, const char *frag_shader)
{
    GLuint prog, vert, frag;
    prog = glCreateProgram();
    vert  = create_shader(vert_shader, GL_VERTEX_SHADER);
    frag = create_shader(frag_shader, GL_FRAGMENT_SHADER);
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glBindAttribLocation(prog, 0, "position");
    glLinkProgram(prog);
    timeLocation = glGetUniformLocation(prog, "time");
    resolutionLocation = glGetUniformLocation(prog, "resolution");
    mouseLocation = glGetUniformLocation(prog, "mouse");
    glUseProgram(prog);
    return prog;
}

void init()
{
    create_program("shader.vert", "shader.frag");
}

void render(void)
{
    GLfloat vertices[] = {
        -1.0, -1.0,  0.0,
        -1.0,  1.0,  0.0,
        1.0,  1.0,  0.0,
        1.0, -1.0,  0.0,
    };

    GLubyte indices[] = {
        0,1,2,  
        0,2,3
    };

    glClear(GL_COLOR_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glfwSwapBuffers(glfwWindow);
}

void resize(int width, int height)
{
    glViewport(0, 0, width, height);

    rwidth = width;
    rheight = height;
    glUniform2f(resolutionLocation, rwidth, rheight);
}

void mpoll()
{
    float time = glfwGetTime();
    glUniform1f(timeLocation, time);
    glfwPollEvents();
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void size_callback(GLFWwindow*w,int width, int height)
{
    resize(width, height);
}

static void cursor_callback(GLFWwindow*w,double x, double y)
{
    float mousex = x;
    float mousey = y;
    glUniform2f(mouseLocation, mousex/rwidth, (rheight-mousey)/rheight);
}


void setup_x11()
{
    Window window = glfwGetX11Window(glfwWindow);
    Display* display = glfwGetX11Display();

    // setup window states
    Atom wmState = XInternAtom(display,"_NET_WM_STATE",False);
    Atom stateAtoms[] = { 
        XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False), // maximized
        XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False), // maximized
        XInternAtom(display, "_NET_WM_STATE_STICKY", False), // on all desktops
        XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False), // no taskbar
        XInternAtom(display, "_NET_WM_STATE_BELOW", False), // always on bottom
    };
    XChangeProperty(display, window, wmState, XA_ATOM, 32, PropModeReplace, (unsigned char *)stateAtoms, 5);

    // set window type to desktop
    Atom wmType = XInternAtom(display,"_NET_WM_WINDOW_TYPE",False);
    Atom typeAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    XChangeProperty(display, window, wmType, XA_ATOM, 32, PropModeReplace, (unsigned char *)&typeAtom, 1);

    // move window to desktop layer
    Atom wmLayer = XInternAtom(display,"_WIN_LAYER",False);
    ulong layer = 0;
    XChangeProperty(display, window, wmLayer, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&layer, 1);

    // remove window decorations
    Atom wmMotif = XInternAtom(display, "_MOTIF_WM_HINTS", False);
    unsigned long motifHints[] = { 2, 0, 0, 0, 0};
    XChangeProperty(display, window, wmMotif, wmMotif, 32, PropModeReplace, (unsigned char *)motifHints, 5);

    // disable input focus on the window
    XWMHints hints = {};
    hints.flags = InputHint | StateHint;
    hints.input = False;
    hints.initial_state = 1;
    XSetWMHints(display, window, &hints);
}

int main()
{
    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) {
        fputs("failed to init glfw", stderr);
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    glfwWindow = glfwCreateWindow(640, 480, "glwall", NULL, NULL);
    if(!glfwWindow) {
        fputs("failed to create window", stderr);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    setup_x11();

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    glfwSetWindowSizeCallback(glfwWindow, size_callback);
    glfwSetCursorPosCallback(glfwWindow, cursor_callback);

    init();
    resize(640, 480);

    while(!glfwWindowShouldClose(glfwWindow)) {
        render();
        mpoll();
    }

    glfwDestroyWindow(glfwWindow);

    glfwTerminate();
    return 0;
}
