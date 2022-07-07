#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <cmath>

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

int W_WIDTH = 1920;
int W_HEIGHT = 1080;
#define SIZE_FIELD 100
#define MAX_HEIGHT 30

static double SCALE_CUBE = 1.2;
static GLfloat corner_axios_z = 0;

static const GLfloat array_vertexes_cube[] = {
    0,0,0,  0,1,0,  1,1,0,  1,0,0,
    0,0,1,  0,1,1,  1,1,1,  1,0,1,
};
static const GLfloat array_normals_cube[] = {
    -1,-1,1,  1,-1,1,  1,1,1,  -1,1,1,
    -1,-1,-1,  1,-1,-1,  1,1,-1,  -1,1,-1
};
static const GLuint array_indexes_cube[] = {
    0,1,2,  2,3,0,
    4,5,6,  6,7,4,
    0,4,7,  7,3,0,
    1,5,6,  6,2,1,
    3,7,6,  6,2,3,
    0,4,5,  5,1,0
};
static GLuint vertex_VBO;
static GLuint normals_VBO;
static GLuint index_EBO;


static void __key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwDestroyWindow(window);
}
static void __scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(yoffset > 0)
        SCALE_CUBE += 0.025;
    else
        SCALE_CUBE -= 0.025;
}
static void __init_scene()
{
    float scaleX = W_WIDTH / (float)W_HEIGHT;

    glEnable(GL_DEPTH_TEST);
    glViewport(0,0,W_WIDTH,W_HEIGHT);

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-1*scaleX,1*scaleX,  -1,1,  1, SIZE_FIELD*1.4);
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenBuffers(1, &vertex_VBO);
    glGenBuffers(1, &normals_VBO);
    glGenBuffers(1, &index_EBO);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(array_vertexes_cube),
        array_vertexes_cube,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(array_normals_cube),
        array_normals_cube,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(array_indexes_cube),
        array_indexes_cube,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
static void __draw_cube()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
        glNormalPointer(GL_FLOAT,0,array_normals_cube);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawElements(GL_TRIANGLES, sizeof(array_indexes_cube)/sizeof(GLuint),
                   GL_UNSIGNED_INT, array_indexes_cube);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
static GLfloat function_harmonics(double x, double y, double i, double j)
{
    return cos(x)*sin(y)*MAX_HEIGHT/3 + MAX_HEIGHT/2 + 1;
}

static void __draw_scene()
{
    static GLfloat values[SIZE_FIELD][SIZE_FIELD];
    static double shiftX = 0;
    static double shiftY = 0;
    static const double diff_shift = 0.28;
    static int i;
    static int j;
    static int k;
    static int k_end;
    static float blue_clr;
    static float position[4] = {0,1,0,0};

    for(i = 0; i < SIZE_FIELD; i++)
        for(j = 0; j < SIZE_FIELD; j++)
            values[i][j] = function_harmonics((i+shiftX)/2.0, (j+shiftY)/2.0, i, j);

    glPushMatrix();

    glRotatef(-60,1,0,0);
    glRotatef(corner_axios_z,0,0,1);

    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glTranslatef(0, 0, -(MAX_HEIGHT*1.5));
    glTranslatef(sin(corner_axios_z*M_PI/180.0)*SIZE_FIELD*0.8,
                 cos(corner_axios_z*M_PI/180.0)*SIZE_FIELD*0.8, 0);


    for(i = -SIZE_FIELD/2; i < SIZE_FIELD/2; i++)
    {
        for(j = -SIZE_FIELD/2; j < SIZE_FIELD/2; j++)
        {
            k_end = values[i+SIZE_FIELD/2][j+SIZE_FIELD/2] - 1;

            for(k = k_end - 5 ; k < k_end; k++)
            {
                blue_clr = k/(float)(values[i+SIZE_FIELD/2][j+SIZE_FIELD/2]-1);

                glColor4f((i+SIZE_FIELD/2)/(float)(SIZE_FIELD-1),
                          (j+SIZE_FIELD/2)/(float)(SIZE_FIELD-1),
                          blue_clr, 0.95);

                glPushMatrix();
                    glTranslatef(i*SCALE_CUBE,j*SCALE_CUBE,k*SCALE_CUBE);
                    glScalef(SCALE_CUBE,SCALE_CUBE,SCALE_CUBE);
                    __draw_cube();
                glPopMatrix();
            }
        }
    }

    corner_axios_z = corner_axios_z + 0.25;
    if(corner_axios_z >= 360)
        corner_axios_z -= 360;

    shiftX += diff_shift;
    shiftY += diff_shift;
    glPopMatrix();
}


int main(void)
{
    static int state_l;
    static int state_r;
    static int state_u;
    static int state_d;
    static const struct timespec req = {0,100};
    static GLFWwindow * window;

    assert(glfwInit());

    GLFWmonitor *   monitor     =   glfwGetPrimaryMonitor();
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "waves", monitor, NULL);
    const GLFWvidmode * mode    =   glfwGetVideoMode(monitor);
    W_WIDTH = mode->width;
    W_HEIGHT = mode->height;

    if (!window) {glfwTerminate(); return -1;}

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, __key_callback);
    glfwSetScrollCallback(window, __scroll_callback);

    __init_scene();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1,1,1,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        __draw_scene();
        state_l = glfwGetKey(window, GLFW_KEY_LEFT);
        state_r = glfwGetKey(window, GLFW_KEY_RIGHT);
        state_u = glfwGetKey(window, GLFW_KEY_UP);
        state_d = glfwGetKey(window, GLFW_KEY_DOWN);

        if(state_l == GLFW_PRESS)
            corner_axios_z++;
        if(state_r == GLFW_PRESS)
            corner_axios_z-=1.5;


        glfwSwapBuffers(window);
        glfwPollEvents();
        nanosleep(&req,NULL);
    }

    glfwTerminate();
    return 0;
}
