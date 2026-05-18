#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

int windowWidth = 1000;
int windowHeight = 700;


float cameraDistance = 12.0f;
float cameraAngleX = 25.0f;
float cameraAngleY = -35.0f;


bool animationRunning = true;
float animationTime = 0.0f;
float animationSpeed = 1.0f;


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
    robots[1].pathT = 0.3f;
    robots[1].speed = 0.8f;
    robots[1].heading = -20.0f;
    robots[1].walking = true;
    robots[1].waving = false;
    robots[1].behaviorState = 0;

    robots[2].position.x = 3.0f;
    robots[2].position.z = -0.5f;
    robots[2].position.y = surfaceHeight(robots[2].position.x, robots[2].position.z) + 1.225f;
    robots[2].pathT = 0.6f;
    robots[2].speed = 1.2f;
    robots[2].heading = -45.0f;
    robots[2].walking = true;
    robots[2].waving = false;
    robots[2].behaviorState = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glDisable(GL_LIGHTING);

    glShadeModel(GL_SMOOTH);

    glClearColor(0.1f, 0.12f, 0.16f, 1.0f);

    for (int i = 0; i < 3; i++) {
    robots[i].position = evaluatePath(i, robots[i].pathT);
    }

    std::cout << "Controls:\n";
    std::cout << "ESC - exit\n";
    std::cout << "W - start/stop animation\n";
    std::cout << "+ / - change speed\n";
}


void setupProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)windowWidth / (float)windowHeight;
    gluPerspective(60.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}


void setupCamera() {
    glLoadIdentity();

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


void setupLighting(int mode);


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
    // Temporary placeholder
}

void setPlasticMaterial() {
    // Temporary placeholder
}

void setShinyMaterial() {
    // Temporary placeholder
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
    glColor3f(0.25f, 0.45f, 0.85f);
    drawUnitCube(2.2f, 3.0f, 1.2f);
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

    glColor3f(0.25f, 0.65f, 0.35f);

    for (int i = 0; i < samples; i++) {
        float x1 = -size / 2.0f + i * step;
        float x2 = x1 + step;

        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j <= samples; j++) {
            float z = -size / 2.0f + j * step;

            float y1 = surfaceHeight(x1, z);
            Vec3 n1 = surfaceNormal(x1, z);
            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(x1, y1, z);

            float y2 = surfaceHeight(x2, z);
            Vec3 n2 = surfaceNormal(x2, z);
            glNormal3f(n2.x, n2.y, n2.z);
            glVertex3f(x2, y2, z);
        }

        glEnd();
    }
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

    glTranslatef(robot.position.x, robot.position.y, robot.position.z);
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


void updateRobots(float dt) {
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


void handleRobotInteractions() {
    // Will be implemented later
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


void drawScene() {
    drawAxes();

    drawCurvedSurface();

    drawPath(0);
    drawPath(1);
    drawPath(2);

    for (int i = 0; i < 3; i++) {
        drawRobot(robots[i]);
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupProjection();
    setupCamera();

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
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
