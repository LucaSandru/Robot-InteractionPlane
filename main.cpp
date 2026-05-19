#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

int windowWidth = 1000;
int windowHeight = 700;


float cameraDistance = 12.0f;
float cameraAngleX = 25.0f;
float cameraAngleY = -35.0f;

int cameraMode = 4;
bool usePerspective = true;


bool animationRunning = true;
float animationTime = 0.0f;
float animationSpeed = 1.0f;


bool interactionsEnabled = true;

int lightingMode = 0;
bool lightingEnabled = true;


GLuint terrainTexture;
GLuint robotTexture;
bool texturesEnabled = true;

bool mouseDragging = false;
int lastMouseX = 0;
int lastMouseY = 0;


struct Vec3 {
    float x, y, z;
};

struct RobotState {
    Vec3 position;
    float pathT;
    float speed;
    float heading;
    bool walking;
    bool waving;
    int behaviorState;
};


RobotState robots[3];


void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDragging = true;
            lastMouseX = x;
            lastMouseY = y;

            cameraMode = 4; // switch to orbit camera
        } else {
            mouseDragging = false;
        }
    }

    // Mouse wheel zoom
    if (button == 3) {
        cameraDistance -= 0.5f;
        if (cameraDistance < 4.0f) {
            cameraDistance = 4.0f;
        }
    }

    if (button == 4) {
        cameraDistance += 0.5f;
        if (cameraDistance > 30.0f) {
            cameraDistance = 30.0f;
        }
    }

    glutPostRedisplay();
}


void mouseMotion(int x, int y) {
    if (mouseDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;

        cameraAngleY += dx * 0.4f;
        cameraAngleX += dy * 0.4f;

        if (cameraAngleX > 85.0f) {
            cameraAngleX = 85.0f;
        }

        if (cameraAngleX < -85.0f) {
            cameraAngleX = -85.0f;
        }

        lastMouseX = x;
        lastMouseY = y;

        cameraMode = 4; // orbit mode
        glutPostRedisplay();
    }
}


float surfaceHeight(float x, float z) {
    return 0.5f * sinf(0.4f * x) + 0.3f * cosf(0.5f * z);
}


Vec3 evaluatePath(int pathId, float t) {
    Vec3 p;

    // keep t between 0 and 1
    t = t - floorf(t);

    float angle = t * 2.0f * 3.14159f;

    if (pathId == 0) {
        p.x = 3.5f * cosf(angle);
        p.z = 1.5f * sinf(angle);
    }
    else if (pathId == 1) {
        p.x = 3.8f * cosf(angle + 1.5f);
        p.z = 2.0f * sinf(angle + 1.5f);
    }
    else {
        p.x = 2.5f * cosf(angle + 3.0f);
        p.z = 3.0f * sinf(angle + 3.0f);
    }

    p.y = surfaceHeight(p.x, p.z) + 1.225f;

    return p;
}


void createCheckerTexture() {
    const int size = 64;
    GLubyte image[size][size][3];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int checker = ((i / 8) + (j / 8)) % 2;

            if (checker) {
                image[i][j][0] = 80;
                image[i][j][1] = 160;
                image[i][j][2] = 80;
            } else {
                image[i][j][0] = 40;
                image[i][j][1] = 110;
                image[i][j][2] = 40;
            }
        }
    }

    glGenTextures(1, &terrainTexture);
    glBindTexture(GL_TEXTURE_2D, terrainTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        size,
        size,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        image
    );
}


void createRobotTexture() {
    const int size = 64;
    GLubyte image[size][size][3];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int stripe = (j / 8) % 2;

            if (stripe) {
                image[i][j][0] = 220;
                image[i][j][1] = 40;
                image[i][j][2] = 40;
            } else {
                image[i][j][0] = 240;
                image[i][j][1] = 240;
                image[i][j][2] = 240;
            }
        }
    }

    glGenTextures(1, &robotTexture);
    glBindTexture(GL_TEXTURE_2D, robotTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        size,
        size,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        image
    );
}


void init() {

    robots[0].position.x = -3.0f;
    robots[0].position.z = -1.0f;
    robots[0].position.y = surfaceHeight(robots[0].position.x, robots[0].position.z) + 1.225f;
    robots[0].pathT = 0.0f;
    robots[0].speed = 1.0f;
    robots[0].heading = 20.0f;
    robots[0].walking = true;
    robots[0].waving = true;
    robots[0].behaviorState = 0;

    robots[1].position.x = 0.0f;
    robots[1].position.z = 1.0f;
    robots[1].position.y = surfaceHeight(robots[1].position.x, robots[1].position.z) + 1.225f;
    robots[1].pathT = 0.5f;
    robots[1].speed = 0.8f;
    robots[1].heading = -20.0f;
    robots[1].walking = true;
    robots[1].waving = false;
    robots[1].behaviorState = 0;

    robots[2].position.x = 3.0f;
    robots[2].position.z = -0.5f;
    robots[2].position.y = surfaceHeight(robots[2].position.x, robots[2].position.z) + 1.225f;
    robots[2].pathT = 0.75f;
    robots[2].speed = 1.2f;
    robots[2].heading = -45.0f;
    robots[2].walking = true;
    robots[2].waving = false;
    robots[2].behaviorState = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glShadeModel(GL_SMOOTH);

    glClearColor(0.1f, 0.12f, 0.16f, 1.0f);

    for (int i = 0; i < 3; i++) {
    robots[i].position = evaluatePath(i, robots[i].pathT);
    }

    createCheckerTexture();
    createRobotTexture();

    std::cout << "Controls:\n";
    std::cout << "ESC - exit\n";
    std::cout << "W - start/stop animation\n";
    std::cout << "+ / - change speed\n";
    std::cout << "I - enable/disable interactions\n";
    std::cout << "L - cycle lighting mode\n";std::cout << "1 - front camera view\n";
    std::cout << "2 - side camera view\n";
    std::cout << "3 - top camera view\n";
    std::cout << "4 - orbit camera view\n";
    std::cout << "P - perspective projection\n";
    std::cout << "O - orthographic projection\n";
    std::cout << "R - reset scene\n";
    std::cout << "T - toggle textures\n";
    }


void setupProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)windowWidth / (float)windowHeight;

    if (usePerspective) {
        gluPerspective(60.0, aspect, 0.1, 100.0);
    } else {
        float viewSize = 8.0f;

        if (aspect >= 1.0f) {
            glOrtho(-viewSize * aspect, viewSize * aspect,
                    -viewSize, viewSize,
                    -100.0, 100.0);
        } else {
            glOrtho(-viewSize, viewSize,
                    -viewSize / aspect, viewSize / aspect,
                    -100.0, 100.0);
        }
    }

    glMatrixMode(GL_MODELVIEW);
}


void setupCamera() {
    glLoadIdentity();

    if (cameraMode == 1) {
        // Front view
        gluLookAt(
            0.0, 4.0, 12.0,
            0.0, 0.5, 0.0,
            0.0, 1.0, 0.0
        );
    }
    else if (cameraMode == 2) {
        // Side view
        gluLookAt(
            12.0, 4.0, 0.0,
            0.0, 0.5, 0.0,
            0.0, 1.0, 0.0
        );
    }
    else if (cameraMode == 3) {
        // Top / isometric view
        gluLookAt(
            0.0, 13.0, 0.1,
            0.0, 0.0, 0.0,
            0.0, 0.0, -1.0
        );
    }
    else {
        // Orbit/free view
        float radX = cameraAngleX * 3.14159f / 180.0f;
        float radY = cameraAngleY * 3.14159f / 180.0f;

        float camX = cameraDistance * cos(radX) * sin(radY);
        float camY = cameraDistance * sin(radX);
        float camZ = cameraDistance * cos(radX) * cos(radY);

        gluLookAt(
            camX, camY, camZ,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0
        );
    }
}


void setupLighting(int mode) {
    if (!lightingEnabled) {
        glDisable(GL_LIGHTING);
        return;
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambient[] = {0.25f, 0.25f, 0.25f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    if (mode == 0) {
        // Mode 0: directional sunlight
        GLfloat lightPosition[] = {-3.0f, 6.0f, 4.0f, 0.0f};
        GLfloat diffuse[] = {0.9f, 0.9f, 0.85f, 1.0f};
        GLfloat specular[] = {0.8f, 0.8f, 0.8f, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    }
    else {
        // Mode 1: moving point light
        float x = 4.0f * cosf(animationTime);
        float z = 4.0f * sinf(animationTime);

        GLfloat lightPosition[] = {x, 5.0f, z, 1.0f};
        GLfloat diffuse[] = {0.7f, 0.85f, 1.0f, 1.0f};
        GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
    }
}


Vec3 surfaceNormal(float x, float z) {
    float delta = 0.1f;

    float hL = surfaceHeight(x - delta, z);
    float hR = surfaceHeight(x + delta, z);
    float hD = surfaceHeight(x, z - delta);
    float hU = surfaceHeight(x, z + delta);

    Vec3 normal;
    normal.x = hL - hR;
    normal.y = 2.0f * delta;
    normal.z = hD - hU;

    float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

    if (length != 0.0f) {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    return normal;
}


float radiansToDegrees(float radians) {
    return radians * 180.0f / 3.14159f;
}


void drawPath(int pathId) {
    //glDisable(GL_LIGHTING);

    if (pathId == 0)
        glColor3f(1.0f, 1.0f, 0.0f);
    else if (pathId == 1)
        glColor3f(1.0f, 0.4f, 0.0f);
    else
        glColor3f(0.0f, 1.0f, 1.0f);

    glBegin(GL_LINE_LOOP);

    for (int i = 0; i < 100; i++) {
        float t = i / 100.0f;
        Vec3 p = evaluatePath(pathId, t);

        glVertex3f(p.x, surfaceHeight(p.x, p.z) + 0.03f, p.z);
    }

    glEnd();

    //glEnable(GL_LIGHTING);
}


void drawUnitCube(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}


void setMatteMaterial() {
    GLfloat specular[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat shininess[] = {5.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void setPlasticMaterial() {
    GLfloat specular[] = {0.35f, 0.35f, 0.35f, 1.0f};
    GLfloat shininess[] = {35.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void setShinyMaterial() {
    GLfloat specular[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat shininess[] = {90.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}


void drawJoint() {
    setShinyMaterial();
    glColor3f(0.15f, 0.15f, 0.15f);
    glutSolidSphere(0.22, 16, 16);
}


void drawHead()
{
    glPushMatrix();

    setPlasticMaterial();
    glColor3f(0.95f, 0.85f, 0.55f);
    glutSolidCube(1.0);

    setMatteMaterial();
    glColor3f(0.1f, 0.05f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.675f, -0.05f);
    glScalef(1.15f, 0.35f, 1.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    setShinyMaterial();
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, -0.05f);
    glScalef(0.05f, 0.6f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    //glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, -0.05f);
    glutSolidSphere(0.15, 15, 15);
    glPopMatrix();
    //glEnable(GL_LIGHTING);

    setMatteMaterial();
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.25f, 0.20f, 0.51f);
    glutSolidCube(0.12f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25f, 0.20f, 0.51f);
    glutSolidCube(0.12f);
    glPopMatrix();

    setPlasticMaterial();
    glColor3f(0.85f, 0.75f, 0.45f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.55f);
    glScalef(0.1f, 0.2f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.95f, 0.85f, 0.55f);

    glPushMatrix();
    glTranslatef(-0.55f, 0.0f, 0.0f);
    glScalef(0.1f, 0.3f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.55f, 0.0f, 0.0f);
    glScalef(0.1f, 0.3f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    setMatteMaterial();
    glColor3f(0.8f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(0.0f, -0.25f, 0.51f);
    glScalef(0.3f, 0.05f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.15f, -0.20f, 0.51f);
    glScalef(0.05f, 0.1f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, -0.20f, 0.51f);
    glScalef(0.05f, 0.1f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}


void drawTorso()
{
    setPlasticMaterial();

    if (texturesEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, robotTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.25f, 0.45f, 0.85f);
    }

    drawUnitCube(2.2f, 3.0f, 1.2f);

    glDisable(GL_TEXTURE_2D);
}


void drawUpperArm()
{
    setPlasticMaterial();
    glColor3f(0.80f, 0.20f, 0.20f);

    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);
    drawUnitCube(0.55f, 1.5f, 0.55f);
    glPopMatrix();
}


void drawForearm()
{
    setMatteMaterial();
    glColor3f(0.90f, 0.55f, 0.25f);

    glPushMatrix();
    glTranslatef(0.0f, -0.65f, 0.0f);
    drawUnitCube(0.45f, 1.3f, 0.45f);
    glPopMatrix();
}


void drawUpperLeg()
{
    setMatteMaterial();
    glColor3f(0.20f, 0.65f, 0.30f);

    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    drawUnitCube(0.7f, 2.0f, 0.7f);
    glPopMatrix();
}


void drawLowerLeg()
{
    setShinyMaterial();
    glColor3f(0.80f, 0.80f, 0.80f);

    glPushMatrix();
    glTranslatef(0.0f, -0.9f, 0.0f);
    drawUnitCube(0.6f, 1.8f, 0.6f);

    setMatteMaterial();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(0.0f, -0.9f, 0.15f);
    drawUnitCube(0.8f, 0.3f, 1.0f);

    glPopMatrix();
}


void drawCurvedSurface() {
    float size = 10.0f;
    int samples = 50;
    float step = size / samples;

    glColor3f(1.0f, 1.0f, 1.0f);

    if (texturesEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, terrainTexture);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.25f, 0.65f, 0.35f);
    }

    for (int i = 0; i < samples; i++) {
        float x1 = -size / 2.0f + i * step;
        float x2 = x1 + step;

        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j <= samples; j++) {
            float z = -size / 2.0f + j * step;

            float y1 = surfaceHeight(x1, z);
            Vec3 n1 = surfaceNormal(x1, z);
            glNormal3f(n1.x, n1.y, n1.z);
            glTexCoord2f((x1 + size / 2.0f) * 0.25f, (z + size / 2.0f) * 0.25f);
            glVertex3f(x1, y1, z);

            float y2 = surfaceHeight(x2, z);
            Vec3 n2 = surfaceNormal(x2, z);
            glNormal3f(n2.x, n2.y, n2.z);
            glTexCoord2f((x2 + size / 2.0f) * 0.25f, (z + size / 2.0f) * 0.25f);
            glVertex3f(x2, y2, z);
        }

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}


void drawRobot(const RobotState& robot)
{
    float walkAngle = 0.0f;
    float waveAngle = 0.0f;

    if (robot.walking)
        walkAngle = 30.0f * sinf(animationTime * 4.0f);

    if (robot.waving)
        waveAngle = 35.0f * sinf(3.0f * animationTime);

    glPushMatrix();

    glTranslatef(robot.position.x, robot.position.y + 0.75f, robot.position.z);
    glRotatef(robot.heading, 0.0f, 1.0f, 0.0f);

    // Scale the old lab robot smaller so it fits the terrain
    glScalef(0.35f, 0.35f, 0.35f);

    drawTorso();

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 2.2f, 0.0f);
    drawHead();
    glPopMatrix();

    // Left arm
    glPushMatrix();
    glTranslatef(-1.45f, 1.2f, 0.0f);
    glRotatef(-walkAngle, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawUpperArm();

    glTranslatef(0.0f, -1.5f, 0.0f);
    glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawForearm();
    glPopMatrix();

    // Right arm
    glPushMatrix();
    glTranslatef(1.45f, 1.2f, 0.0f);

    float rightShoulder = walkAngle;
    if (robot.waving)
        rightShoulder = -65.0f;

    glRotatef(rightShoulder, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawUpperArm();

    glTranslatef(0.0f, -1.5f, 0.0f);

    if (robot.waving)
        glRotatef(waveAngle, 0.0f, 0.0f, 1.0f);
    else
        glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);

    drawJoint();
    drawForearm();
    glPopMatrix();

    // Left leg
    glPushMatrix();
    glTranslatef(-0.65f, -1.5f, 0.0f);
    glRotatef(walkAngle, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawUpperLeg();

    glTranslatef(0.0f, -2.0f, 0.0f);
    glRotatef(-fabs(walkAngle) * 0.5f, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawLowerLeg();
    glPopMatrix();

    // Right leg
    glPushMatrix();
    glTranslatef(0.65f, -1.5f, 0.0f);
    glRotatef(-walkAngle, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawUpperLeg();

    glTranslatef(0.0f, -2.0f, 0.0f);
    glRotatef(-fabs(walkAngle) * 0.5f, 1.0f, 0.0f, 0.0f);
    drawJoint();
    drawLowerLeg();
    glPopMatrix();

    glPopMatrix();
}


float distanceXZ(const Vec3& a, const Vec3& b) {
    float dx = a.x - b.x;
    float dz = a.z - b.z;
    return sqrtf(dx * dx + dz * dz);
}


void handleRobotInteractions() {
    robots[0].waving = false;
    robots[1].waving = false;
    robots[2].waving = false;

    robots[0].speed = 1.0f;
    robots[1].speed = 0.8f;
    robots[2].speed = 1.2f;

    robots[0].behaviorState = 0;
    robots[1].behaviorState = 0;
    robots[2].behaviorState = 0;

    if (!interactionsEnabled) {
        return;
    }

    float d01 = distanceXZ(robots[0].position, robots[1].position);
    float d12 = distanceXZ(robots[1].position, robots[2].position);
    float d02 = distanceXZ(robots[0].position, robots[2].position);

    // Interaction 1: Robot 0 and Robot 1 greet
    if (d01 < 2.4f) {
    robots[0].waving = true;
    robots[1].waving = true;

    robots[0].speed = 0.0f;
    robots[1].speed = 0.8f;

    robots[0].behaviorState = 1;
    robots[1].behaviorState = 1;
    }

    // Interaction 2: Robot 1 avoids Robot 2
    if (d12 < 2.8f) {
    robots[1].speed = 0.0f;
    robots[2].speed = 1.2f;

    robots[1].waving = true;
    robots[2].waving = true;

    robots[1].behaviorState = 2;
    robots[2].behaviorState = 2;
    }

    // Extra safety: avoid Robot 0 and Robot 2 overlapping
    // Interaction 3: Robot 0 and Robot 2 greet
    if (d02 < 2.4f) {
        robots[0].waving = true;
        robots[2].waving = true;

        robots[0].speed = 0.0f;
        robots[2].speed = 1.2f;

        robots[0].behaviorState = 3;
        robots[2].behaviorState = 3;
    }
}


void updateRobots(float dt) {

    handleRobotInteractions();

    for (int i = 0; i < 3; i++) {
        if (!robots[i].walking) {
            continue;
        }

        robots[i].pathT += dt * robots[i].speed * 0.08f;

        if (robots[i].pathT > 1.0f) {
            robots[i].pathT -= 1.0f;
        }

        Vec3 currentPos = evaluatePath(i, robots[i].pathT);
        Vec3 nextPos = evaluatePath(i, robots[i].pathT + 0.01f);

        robots[i].position = currentPos;

        float dx = nextPos.x - currentPos.x;
        float dz = nextPos.z - currentPos.z;

        robots[i].heading = radiansToDegrees(atan2f(dx, dz));
    }

}


void drawAxes() {
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(5, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 5, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 5);

    glEnd();
}


void drawSpeechText(float x, float y, float z, const char* text) {
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y, z);

    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }

    if (lightingEnabled) {
        glEnable(GL_LIGHTING);
    }
}


void drawText(float x, float y, float z, const char* text) {
    glDisable(GL_LIGHTING);

    glRasterPos3f(x, y, z);

    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }

    if (lightingEnabled) {
        glEnable(GL_LIGHTING);
    }
}


void drawScene() {
    drawAxes();

    drawCurvedSurface();

    drawPath(0);
    drawPath(1);
    drawPath(2);

    for (int i = 0; i < 3; i++) {
    drawRobot(robots[i]);

    if (i == 0)
        glColor3f(1.0f, 0.0f, 0.0f);
    else if (i == 1)
        glColor3f(0.0f, 1.0f, 0.0f);
    else
        glColor3f(0.0f, 0.5f, 1.0f);

    float tx = robots[i].position.x;
    float ty = robots[i].position.y + 2.8f + i * 0.25f;
    float tz = robots[i].position.z;

    if (i == 0)
        drawText(tx, ty, tz, "Robot 0");
    else if (i == 1)
        drawText(tx, ty, tz, "Robot 1");
    else
        drawText(tx, ty, tz, "Robot 2");

    if (robots[i].behaviorState == 1) {
    float sx = robots[i].position.x;
    float sy = robots[i].position.y + 3.25f + i * 0.25f;
    float sz = robots[i].position.z;

    if (i == 0)
        drawSpeechText(sx, sy, sz, "Hi, Robot 1!");
    else if (i == 1)
        drawSpeechText(sx, sy, sz, "Hi, Robot 0!");
}

if (robots[i].behaviorState == 2) {
    float sx = robots[i].position.x;
    float sy = robots[i].position.y + 3.25f + i * 0.25f;
    float sz = robots[i].position.z;

    if (i == 1)
        drawSpeechText(sx, sy, sz, "Hi, Robot 2!");
    else if (i == 2)
        drawSpeechText(sx, sy, sz, "Hi, Robot 1!");
}

    if (robots[i].behaviorState == 3) {
        float sx = robots[i].position.x;
        float sy = robots[i].position.y + 3.25f + i * 0.25f;
        float sz = robots[i].position.z;

        if (i == 0)
            drawSpeechText(sx, sy, sz, "Hi, Robot 2!");
        else if (i == 2)
            drawSpeechText(sx, sy, sz, "Hi, Robot 0!");
    }
    }

}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupProjection();
    setupCamera();

    setupLighting(lightingMode);

    drawScene();

    glutSwapBuffers();
}


void reshape(int w, int h) {
    if (h == 0) h = 1;

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;

        case 'w':
        case 'W':
            animationRunning = !animationRunning;
            break;

        case '+':
            animationSpeed += 0.2f;
            break;

        case '-':
            animationSpeed -= 0.2f;
            if (animationSpeed < 0.2f) animationSpeed = 0.2f;
            break;

        case 'i':
        case 'I':
            interactionsEnabled = !interactionsEnabled;
            break;

        case 'l':
        case 'L':
            lightingMode = (lightingMode + 1) % 2;
            break;

        case '1':
            cameraMode = 1;
            break;

        case '2':
            cameraMode = 2;
            break;

        case '3':
            cameraMode = 3;
            break;

        case '4':
            cameraMode = 4;
            break;

        case 'p':
        case 'P':
            usePerspective = true;
            break;

        case 'o':
        case 'O':
            usePerspective = false;
            break;

        case 'r':
        case 'R':
            cameraMode = 4;
            usePerspective = true;
            cameraDistance = 12.0f;
            cameraAngleX = 25.0f;
            cameraAngleY = -35.0f;
            animationTime = 0.0f;

            robots[0].pathT = 0.0f;
            robots[1].pathT = 0.5f;
            robots[2].pathT = 0.75f;

            for (int i = 0; i < 3; i++) {
                robots[i].position = evaluatePath(i, robots[i].pathT);
            }
            break;
        case 't':
        case 'T':
            texturesEnabled = !texturesEnabled;
            break;
        }

    glutPostRedisplay();
}


void timer(int value) {
    if (animationRunning) {
        float dt = 0.016f * animationSpeed;

        animationTime += dt;
        updateRobots(dt);
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Robot Interaction Arena - Milestone 1");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
