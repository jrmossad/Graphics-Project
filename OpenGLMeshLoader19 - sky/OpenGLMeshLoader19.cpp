#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"

#include <vector>
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <glut.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
using namespace std;

//=======================DEFINITIONS======================================
#define DEGTORAD(x) x*3.1415/180.0
#define GLUT_KEY_ESCAPE 27

enum Level {
    LEVEL_1, LEVEL_2
};

bool hasGameEnded = false;

//Textures
GLTexture level1tex_walls;
GLTexture level2tex_walls;
GLTexture level1tex_ground;
GLTexture level2tex_ground;
GLTexture tex_ground;
GLuint tex;

// Level
Level level = LEVEL_1;

class Vector;
class Room;
void drawUnitCube();
bool insideRectangle(Vector bottomLeft, Vector topRight, Vector p);
bool validMove(Vector map[], int n, Vector p);
void print(float x, float y, char* string);
void init();
void Display(void);
void mouseMovement(int x, int y);
void SpecialInput(int key, int x, int y);
void key(unsigned char k, int x, int y);
void Idle();
void loadAssets();
void drawTexturedFace(int type, GLTexture _tex, int rep);
void playMusic(int musicConstant);
void playSound(int soundConstant);
bool hasSteppedOnSpike();
bool hasCollidedWithMovingWall(Vector after);
bool isLevelDoorClosed();
void drawGameText();
void drawGameOver();
void RenderSanta(double x, double z);
void RenderPlayer2(double x, double z);
void drawSpikes();
void drawMovingWalls();
void drawLevelDoor();
void drawSun();
void drawMoon();
void setupLights();

//===============================CLASSES=================================
class Vector
{
public:
    float x, y, z;
    Vector() : x(0.0), y(0.0), z(0.0) {}
    Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vector& operator += (const Vector& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vector& operator -= (const Vector& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    //operator overloading
    Vector& operator +=(float value)
    {
        x += value;
        y += value;
        z += value;

        return *this;
    }

    Vector& operator -=(float value)
    {
        x -= value;
        y -= value;
        z -= value;

        return *this;
    }

    Vector& operator + (Vector vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;

        return *this;
    }

    Vector& operator - (Vector vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;

        return *this;
    }

    Vector& operator = (Vector vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;

        return *this;
    }

    Vector& normalize() {
        float mag = sqrt(x * x + y * y + z * z);

        x /= mag;
        y /= mag;
        z /= mag;

        return *this;
    }

    Vector& set(float x, float y, float z) {

        this->x = x;
        this->y = y;
        this->z = z;

        return *this;
    }
};

enum Color {
    RED, GREEN, BLUE, NONE
};

class Ball
{
public:

    Vector pos;
    Color color;

    bool isPickedUp;

    Ball(Vector _position, Color _color) : pos(_position), color(_color) {}
};

class Slot
{
public:

    Vector pos;
    Color color;

    bool isBallPlaced;

    Slot(Vector _position, Color _color) : pos(_position), color(_color) {}
};

class Room
{
public:
    GLfloat  x, y, z, sx, sy, sz;
    int leftWall, rightWall, frontWall, backWall;
    float wallHeight;
    float doorPortions[4];

    Room(GLfloat _x, GLfloat _y, GLfloat _z
        , GLfloat _sx, GLfloat _sy, GLfloat _sz,
        float _wallHeight, float _doorPortions[4]) {

        x = _x;
        y = _y;
        z = _z;
        sx = _sx;
        sy = _sy;
        sz = _sz;
        leftWall = rightWall = frontWall = backWall = -1;
        wallHeight = _wallHeight;

        doorPortions[0] = _doorPortions[0];
        doorPortions[1] = _doorPortions[1];
        doorPortions[2] = _doorPortions[2];
        doorPortions[3] = _doorPortions[3];
    }

    void assignWall(int direction, int type) {
        switch (direction) {
        case 0:
            frontWall = type;
            break;
        case 1:
            rightWall = type;
            break;
        case 2:
            backWall = type;
            break;
        case 3:
            leftWall = type;
            break;
        default:
            frontWall = type;
        }
    }

    void drawFrontWall(int type) {
        if (type == 0) {
            glPushMatrix();
            glTranslated(x, y + wallHeight / 2.0 + (sy / 2.0), z - (sz / 2.0) + (sy / 2.0));
            glScaled(sx, wallHeight, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(2, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(2, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }

        if (type == 1) {
            //part 1
            glPushMatrix();
            glTranslated(x + sx / 2.0 - ((sx / 2.0) - (sx * (doorPortions[0] / 2.0))) / 2.0,
                y + wallHeight / 4.0 + (sy / 2.0),
                z - (sz / 2.0) + (sy / 2.0));
            glScaled((sx / 2.0) - (sx * (doorPortions[0] / 2.0)), 1, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(2, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(2, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x - sx / 2.0 + ((sx / 2.0) - (sx * (doorPortions[0] / 2.0))) / 2.0,
                y + wallHeight / 4.0 + (sy / 2.0),
                z - (sz / 2.0) + (sy / 2.0));
            glScaled((sx / 2.0) - (sx * (doorPortions[0] / 2.0)), wallHeight / 2.0, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(2, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(2, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x,
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z - (sz / 2.0) + (sy / 2.0));
            glScaled(sx, wallHeight / 2.0, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(2, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(2, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }
    }

    void drawBackWall(int type) {

        if (type == 0) {
            glPushMatrix();
            glTranslated(x, y + wallHeight / 2.0 + (sy / 2.0), z + (sz / 2.0) - (sy / 2.0));
            glScaled(sx, wallHeight, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(3, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(3, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }

        if (type == 1) {
            //part 1
            glPushMatrix();
            glTranslated(x + sx / 2.0 - ((sx / 2.0) - (sx * (doorPortions[2] / 2.0))) / 2.0,
                y + wallHeight / 4.0 + (sy / 2.0),
                z + (sz / 2.0) - (sy / 2.0));
            glScaled((sx / 2.0) - (sx * (doorPortions[2] / 2.0)), wallHeight / 2.0, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(3, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(3, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x - sx / 2.0 + ((sx / 2.0) - (sx * (doorPortions[2] / 2.0))) / 2.0,
                y + wallHeight / 4.0 + (sy / 2.0),
                z + (sz / 2.0) - (sy / 2.0));
            glScaled((sx / 2.0) - (sx * (doorPortions[2] / 2.0)), wallHeight / 2.0, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(3, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(3, level2tex_walls, 1);
            }
            drawTexturedFace(3, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x,
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z + (sz / 2.0) - (sy / 2.0));
            glScaled(sx, wallHeight / 2.0, sy);
            if (level == LEVEL_1) {
                drawTexturedFace(3, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(3, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }

    }

    void drawLeftWall(int type) {
        if (type == 0) {
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0), y + wallHeight / 2.0 + (sy / 2.0), z);
            glScaled(sy, wallHeight, sz);
            if (level == LEVEL_1) {
                drawTexturedFace(5, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(5, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }

        if (type == 1) {
            //part 1
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0),
                y + wallHeight / 4.0 + (sy / 2.0),
                z - sz / 2.0 + ((sz / 2.0) - (sz * (doorPortions[3] / 2.0))) / 2.0);
            glScaled(sy, wallHeight / 2.0, (sz / 2.0) - (sz * (doorPortions[3] / 2.0)));
            if (level == LEVEL_1) {
                drawTexturedFace(5, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(5, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0),
                y + wallHeight / 4.0 + (sy / 2.0),
                z + sz / 2.0 - ((sz / 2.0) - (sz * (doorPortions[3] / 2.0))) / 2.0);
            glScaled(sy, wallHeight / 2.0, (sz / 2.0) - (sz * (doorPortions[3] / 2.0)));
            if (level == LEVEL_1) {
                drawTexturedFace(5, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(5, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0),
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z);
            glScaled(sy, wallHeight / 2.0, sz);
            if (level == LEVEL_1) {
                drawTexturedFace(5, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(5, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }
    }

    void drawRightWall(int type) {
        if (type == 0) {
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0), y + wallHeight / 2.0 + (sy / 2.0), z);
            glScaled(sy, wallHeight, sz);
            if (level == LEVEL_1) {
                drawTexturedFace(4, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(4, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }

        if (type == 1) {
            //part 1
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0),
                y + wallHeight / 4.0 + (sy / 2.0),
                z - sz / 2.0 + ((sz / 2.0) - (sz * (doorPortions[1] / 2.0))) / 2.0);
            glScaled(sy, wallHeight / 2.0, (sz / 2.0) - (sz * (doorPortions[1] / 2.0)));
            if (level == LEVEL_1) {
                drawTexturedFace(4, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(4, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0),
                y + wallHeight / 4.0 + (sy / 2.0),
                z + sz / 2.0 - ((sz / 2.0) - (sz * (doorPortions[1] / 2.0))) / 2.0);
            glScaled(sy, wallHeight / 2.0, (sz / 2.0) - (sz * (doorPortions[1] / 2.0)));
            if (level == LEVEL_1) {
                drawTexturedFace(4, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(4, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0),
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z);
            glScaled(sy, wallHeight / 2.0, sz);
            if (level == LEVEL_1) {
                drawTexturedFace(4, level1tex_walls, 1);
            }
            else {
                drawTexturedFace(4, level2tex_walls, 1);
            }
            drawUnitCube();
            glPopMatrix();
        }
    }

    void render() {
        glDisable(GL_LIGHTING);
        glColor3f(0.6, 0.6, 0.6);
        //floor
        glPushMatrix();
        glTranslated(x, y, z);
        glScaled(sx, sy, sz);
        //drawTexturedFace(0, tex_ground, 5);
        if (level == LEVEL_1) {
            drawTexturedFace(0, level1tex_ground, 2);
        }
        else {
            drawTexturedFace(0, level2tex_ground, 2);
        }
        drawUnitCube();
        glPopMatrix();


        glColor3f(0.7, 0.7, 0.7);
        //walls
        drawFrontWall(frontWall);
        drawBackWall(backWall);
        drawLeftWall(leftWall);
        drawRightWall(rightWall);
    }
};
//=============================GLOBAL-VARS===============================

Vector normals[6] = { Vector(0, 1, 0),Vector(0, -1, 0), Vector(1, 0, 0),Vector(1, 0, 0),Vector(1, 0, 0), Vector(-1, 0, 0) };

//3D Models
Model_3DS christmas_tree;
Model_3DS santa;
Model_3DS pumpkin;
Model_3DS present;
Model_3DS player2;
Model_3DS skeleton;
Model_3DS skull;

//camera
Vector eye(0.0, 0.5, 0.0);
Vector ref(1.0, 0.5, 0.0);
Vector up(0.0, 1.0, 0.0);
bool firstPerson = true;
Vector playerPos(0.0, 0.5, 0.0);
Vector cameraRotation(0.0, 0.0, 0.0);
float camDistance = 0.4;

//movement speed
float speed = 0.2;

//screen
int screenW, screenH;

//mouse position
float mouseX, mouseY;

//wall/floor thickness
float thickness = 0.05;
float halfThickness = 0.025;

//the coordinates used for collision when the player collects all the balls
Vector winningRoom[] = {
    //winning room
    Vector(-0.25 + thickness, halfThickness , -2 + thickness), //bottom-left
    Vector(0.25 - thickness , halfThickness , -3), //top-right
};
//clock-wise array of bottom-left, top-right coordinates of rooms
Vector mapRectangles[] = {
    //main room
    Vector(-2 + thickness, halfThickness , 2 - thickness), //bottom-left
    Vector(2 - thickness, halfThickness , -2 + thickness), //top-right
    //right hallway
    Vector(2 - thickness, halfThickness , 0.25 - thickness), //bottom-left
    Vector(4 - thickness , halfThickness , -0.25 + thickness), //top-right
    //right room
    Vector(4 - thickness, halfThickness , 2 - thickness), //bottom-left
    Vector(6 - 3 * thickness , halfThickness , -2 + thickness), //top-right
    //rightBotttom hallway
    Vector(5 - thickness - 0.25, halfThickness , 6 - thickness), //bottom-left
    Vector(5 - 3 * thickness + 0.25, halfThickness , 2 - thickness), //top-right
    //bottom room
    Vector(2 - thickness, halfThickness , 9 - 3 * thickness), //bottom-left
    Vector(8 - 3 * thickness, halfThickness , 6 - thickness), //top-right
    //left hallway
    Vector(-4 + thickness, halfThickness , 0.25 - thickness), //bottom-left
    Vector(-2 + thickness, halfThickness , -0.25 + thickness), //top-right
    //left room
    Vector(-6 + 3 * thickness, halfThickness , 2 - thickness), //bottom-left
    Vector(-4 + thickness, halfThickness , -2 + thickness), //top-right
};

Vector spikes[] = {
    // Corner Room
    Vector(6.5, 0.0, 8.0),
    Vector(6.5, 0.0, 7.5),
    Vector(6.5, 0.0, 7.0),
    Vector(6.5, 0.0, 6.5),
           
    Vector(6.5, 0.0, 8.0),
    Vector(6.5, 0.0, 7.5),
    Vector(6.5, 0.0, 7.0),
    Vector(6.5, 0.0, 6.5),

    Vector(5.5, 0.0, 8.0),
    Vector(5.5, 0.0, 7.5),
    Vector(5.5, 0.0, 7.0),
    Vector(5.5, 0.0, 6.5),

    Vector(5.0, 0.0, 8.0),
    Vector(5.0, 0.0, 7.5),
    Vector(5.0, 0.0, 7.0),
    Vector(5.0, 0.0, 6.5),

    Vector(4.5, 0.0, 8.0),
    Vector(4.5, 0.0, 7.5),
    Vector(4.5, 0.0, 7.0),
    Vector(4.5, 0.0, 6.5),

    Vector(4.0, 0.0, 8.0),
    Vector(4.0, 0.0, 7.5),
    Vector(4.0, 0.0, 7.0),
    Vector(4.0, 0.0, 6.5),

    Vector(3.5, 0.0, 8.0),
    Vector(3.5, 0.0, 7.5),
    Vector(3.5, 0.0, 7.0),
    Vector(3.5, 0.0, 6.5),

    Vector(3.0, 0.0, 8.0),
    Vector(3.0, 0.0, 7.5),
    Vector(3.0, 0.0, 7.0),
    Vector(3.0, 0.0, 6.5),

    // Top Room
    Vector(5.0, 0.0, -1.0),
    Vector(5.0, 0.0, -0.5),
    Vector(5.0, 0.0, 0.0),
    Vector(5.0, 0.0, 0.5),
    Vector(5.0, 0.0, 1.0),

    Vector(4.5, 0.0, -1.0),
    Vector(4.5, 0.0, -0.5),
    Vector(4.5, 0.0, 0.0),
    Vector(4.5, 0.0, 0.5),
    Vector(4.5, 0.0, 1.0),

    // Middle Room
    Vector(-1.0, 0.0, -1.0),
    Vector(-1.0, 0.0, -0.5),
    Vector(-1.0, 0.0, 0.0),
    Vector(-1.0, 0.0, 0.5),
    Vector(-1.0, 0.0, 1.0),

    Vector(-0.5, 0.0, -1.0),
    Vector(-0.5, 0.0, -0.5),
    Vector(-0.5, 0.0, 0.5),
    Vector(-0.5, 0.0, 1.0),
    
    Vector(0.0, 0.0, -1.0),
    Vector(0.0, 0.0, 1.0),

    Vector(0.5, 0.0, -1.0),
    Vector(0.5, 0.0, -0.5),
    Vector(0.5, 0.0, 0.5),
    Vector(0.5, 0.0, 1.0),

    Vector(1.0, 0.0, -1.0),
    Vector(1.0, 0.0, -0.5),
    Vector(1.0, 0.0, 0.0),
    Vector(1.0, 0.0, 0.5),
    Vector(1.0, 0.0, 1.0),

    // Bottom Room
    Vector(-5.0, 0.0, -1.0),
    Vector(-5.0, 0.0, -0.5),
    Vector(-5.0, 0.0, 0.0),
    Vector(-5.0, 0.0, 0.5),
    Vector(-5.0, 0.0, 1.0),

    Vector(-4.5, 0.0, -1.0),
    Vector(-4.5, 0.0, -0.5),
    Vector(-4.5, 0.0, 0.0),
    Vector(-4.5, 0.0, 0.5),
    Vector(-4.5, 0.0, 1.0)
};

Vector movingWalls[] = {
    // Corner Hallway
    Vector(4.9, 0.0, 5.9),
    Vector(4.9, 0.0, 2.0),

    // Top Hallway
    Vector(3.9, 0.0, 0.0),
    Vector(2.0, 0.0, 0.0),

    // Bottom Hallway
    Vector(-2.0, 0.0, 0.0),
    Vector(-3.9, 0.0, 0.0)
};

// Light Source Rotation
double lightRotation = 0;
bool incrementLightRotation = false;

// Moving Walls Translation
double wallsTranslation = 0.0;
bool areWallsMovingUp = false;

Vector levelDoor = Vector(0, thickness, -2);

// Balls
Ball redBall = Ball(Vector(5.5, 0, 0), RED);
Ball greenBall = Ball(Vector(-5.5, 0, 0), GREEN);
Ball blueBall = Ball(Vector(4.9, 0, 8.5), BLUE);

// Slots
Slot redSlot = Slot(Vector(-1.45, 0, -1.7), RED);
Slot greenSlot = Slot(Vector(-1.1, 0, -1.7), GREEN);
Slot blueSlot = Slot(Vector(-0.75, 0, -1.7), BLUE);

// Picked Ball
Color pickedBall = NONE;

//score
int score = 0;
int timeBonus = 180;

//predefined door sizes
float eighth[4] = { 0.125, 0.125, 0.125, 0.125 };
float eighthBackHalf[4] = { 0.125, 0.125, 0.25, 0.125 };
float eighthFrontTwelfth[4] = { 1.0 / 12.0, 0.125, 0.125, 0.125 };

//Room coordinates and dimensions
Room mainRoom(0, 0, 0, 4, thickness, 4, 2, eighth);
Room rightHallWay(3 - thickness, 0, 0, 2, thickness, 0.5, 1, eighth);
Room rightRoom(5 - 2 * thickness, 0, 0, 2, thickness, 4, 2, eighthBackHalf);
Room rightBottomHallway(5 - 2 * thickness, 0, 4 - thickness, 0.5, thickness, 4, 1, eighth);
Room bottomRoom(5 - 2 * thickness, 0, 7.5 - 2 * thickness, 6, thickness, 3, 2, eighthFrontTwelfth);
Room leftHallWay(-3 + thickness, 0, 0, 2, thickness, 0.5, 1, eighth);
Room leftRoom(-5 + 2 * thickness, 0, 0, 2, thickness, 4, 2, eighth);

//===============================HELPER-Functions==================================

//draws the player
void drawPlayer(float x, float y, float z) {
    glPushMatrix();

    if (level == LEVEL_1) {
        glTranslated(x, thickness + 0.2, z);
    }
    else {
        glTranslated(x, thickness + 0.05, z);
    }

    glScaled(0.2, 0.4, 0.2);
    glRotated(-cameraRotation.x + 90, 0, 1, 0);
    if (level == LEVEL_1) {
        RenderSanta(playerPos.x, playerPos.z);
    } else {
        RenderPlayer2(playerPos.x, playerPos.z);
    }

    glPopMatrix();
}

//draws 1x1x1 cube
void drawUnitCube() {
    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
    // Top face (y = 0.5f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    glNormal3f(normals[0].x, normals[0].y, normals[0].z);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    // Bottom face (y = -0.5f)
    glNormal3f(normals[1].x, normals[1].y, normals[1].z);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Front face  (z = 0.5f)
    glNormal3f(normals[2].x, normals[2].y, normals[2].z);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Back face (z = -0.5f)
    glNormal3f(normals[3].x, normals[3].y, normals[3].z);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Left face (x = -0.5f)
    glNormal3f(normals[4].x, normals[4].y, normals[4].z);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face (x = 0.5f)
    glNormal3f(normals[5].x, normals[5].y, normals[5].z);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();
}

void drawTexturedFace(int type, GLTexture _tex, int rep) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tex.texture[0]);	// Bind the ground texture
    glBegin(GL_QUADS);
    glNormal3f(normals[type].x, normals[type].y, normals[type].z);

    switch (type)
    {
    case 0:
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(rep, 0.0f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(rep, rep);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, rep);
        glVertex3f(0.5f, 0.5f, 0.5f);
        break;
    case 1:
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(rep, 0.0f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(rep, rep);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, rep);
        glVertex3f(0.5f, -0.5f, -0.5f);
        break;
    case 2:
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(rep, 0.0f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(rep, rep);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(0.0f, rep);
        glVertex3f(0.5f, -0.5f, 0.5f);
        break;
    case 3:
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(rep, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(rep, rep);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, rep);
        glVertex3f(0.5f, 0.5f, -0.5f);
        break;
    case 4:
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(rep, 0.0f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(rep, rep);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, rep);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        break;
    default:
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(rep, 0.0f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(rep, rep);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(0.0f, rep);
        glVertex3f(0.5f, -0.5f, -0.5f);
        break;
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

//if point is inside a rectangle characterized by bottom-left/top-right points
bool insideRectangle(Vector bottomLeft, Vector topRight, Vector p)
{
    if (p.x > bottomLeft.x && p.x < topRight.x
        && p.z > topRight.z && p.z < bottomLeft.z
        && p.y >= halfThickness && p.y < 2) {
        return true;
    }
    return false;
}

// Returns true if the point p lies inside the map
bool validMove(Vector map[], int n, Vector p)
{
    //n is odd
    if (n & 1) return false;

    bool valid = false;
    for (int i = 0; i < n - 1; i += 2) {
        valid |= insideRectangle(map[i], map[i + 1], p);
    }

    if (!isLevelDoorClosed()) {
        valid |=  insideRectangle(winningRoom[0], winningRoom[1], p);
    }

    return valid;
}

//print text
void print(float x, float y, char* string)
{
    int len, i;

    glRasterPos2f(x, y);

    len = (int)strlen(string);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, screenW, 0.0, screenH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    for (int i = 0; i < len; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void second(int val)
{
    timeBonus--;
    if (timeBonus == 0) {
        hasGameEnded = true;
        playSound(8);
    }
    else {
        glutTimerFunc(1000, second, 0);
    }

    glutPostRedisplay();
}


//any initialization is done here
void init() {
    mainRoom.assignWall(0, 1);
    mainRoom.assignWall(1, 1);
    mainRoom.assignWall(2, 0);
    mainRoom.assignWall(3, 1);

    rightHallWay.assignWall(0, 0);
    rightHallWay.assignWall(2, 0);

    rightRoom.assignWall(0, 0);
    rightRoom.assignWall(1, 0);
    rightRoom.assignWall(2, 1);
    rightRoom.assignWall(3, 1);

    rightBottomHallway.assignWall(1, 0);
    rightBottomHallway.assignWall(3, 0);

    bottomRoom.assignWall(0, 1);
    bottomRoom.assignWall(1, 0);
    bottomRoom.assignWall(2, 0);
    bottomRoom.assignWall(3, 0);

    leftHallWay.assignWall(0, 0);
    leftHallWay.assignWall(2, 0);

    leftRoom.assignWall(0, 0);
    leftRoom.assignWall(1, 1);
    leftRoom.assignWall(2, 0);
    leftRoom.assignWall(3, 0);

    //start score timer
    glutTimerFunc(1000, second, 0);

    // Play background music.
    if (level == LEVEL_1) {
        playMusic(0);
    }
    else {
        playMusic(1);
    }
}

void RenderPumpkin(double x = 0.0, double z = 0.0) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 0.3, z);
    //glRotated(-90, 1, 0, 0);
    glScaled(0.001, 0.001, 0.001);
    pumpkin.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void RenderTree(double x = 0.0, double z = 0.0) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 1.25, z);
    glScaled(0.001, 0.001, 0.001);
    christmas_tree.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void RenderSanta(double x = 0.0, double z = 0.0) {
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glScaled(0.0075, 0.0075, 0.0075);
    santa.Draw();
    glPopMatrix();
    //glEnable(GL_LIGHTING);
}

void RenderPresent(double x = 0.0, double z = 0.0) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 0.2, z);
    glScaled(0.001, 0.001, 0.001);
    present.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void RenderPlayer2(double x = 0.0, double z = 0.0) {
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glScaled(0.02, 0.02, 0.02);
    player2.Draw();
    glPopMatrix();
    //glEnable(GL_LIGHTING);
}

void RenderSkeleton(double x = 0.0, double z = 0.0) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 0.2, z);
    glScaled(0.15, 0.15, 0.15);
    skeleton.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void RenderSkull(double x = 0.0, double z = 0.0) {
    glColor3f(0.6, 0.6, 0.6);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 0.25, z);
    glScaled(0.25, 0.25, 0.25);
    skull.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glColor3f(1, 1, 1);
}

void RenderGround()
{
    glDisable(GL_LIGHTING);	// Disable lighting

    glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

    glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

    glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

    glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);	// Set quad normal direction.
    glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
    glVertex3f(-20, 0, -20);
    glTexCoord2f(5, 0);
    glVertex3f(20, 0, -20);
    glTexCoord2f(5, 5);
    glVertex3f(20, 0, 20);
    glTexCoord2f(0, 5);
    glVertex3f(-20, 0, 20);
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

    glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void drawSpikes() {
    int spikesCount = sizeof(spikes) / sizeof(spikes[0]);

    glColor3f(0.9, 0.6, 0.0);

    for (int i = 0; i < spikesCount; i++) {
        glPushMatrix();

        glTranslated(spikes[i].x, thickness + spikes[i].y, spikes[i].z);
        glRotated(-90, 1, 0, 0);
        glutSolidCone(0.1, 0.2, 30, 60);

        glPopMatrix();
    }
}

bool hasSteppedOnSpike() {
    int spikesCount = sizeof(spikes) / sizeof(spikes[0]);

    for (int i = 0; i < spikesCount; i++) {
        if (spikes[i].x - 0.1 <= playerPos.x && playerPos.x <= spikes[i].x + 0.1
            && spikes[i].z - 0.1 <= playerPos.z && playerPos.z <= spikes[i].z + 0.1) {
            return true;
        }
    }

    return false;
}

void drawMovingWalls() {
    int movingWallsCount = sizeof(movingWalls) / sizeof(movingWalls[0]);

    glColor3f(0.35, 0.25, 0.15);

    for (int i = 0; i < movingWallsCount; i++) {
        glPushMatrix();

        glTranslated(movingWalls[i].x, thickness + movingWalls[i].y + wallsTranslation, movingWalls[i].z);
        if (i <= 1) {
            // Corner Room
            glScaled(10, 40, 1);
        }
        else {
            // Other Rooms
            glScaled(1, 40, 10);
        }
        glutSolidCube(thickness);

        glPopMatrix();
    }
}

bool hasCollidedWithMovingWall(Vector after) {
    if (level == LEVEL_1) return false;

    if (wallsTranslation <= -1.0) return false;

    int movingWallsCount = sizeof(movingWalls) / sizeof(movingWalls[0]);

    for (int i = 0; i < movingWallsCount; i++) {
        if (i <= 1) {
            // Corner Room
            if (playerPos.z <= movingWalls[i].z && movingWalls[i].z <= after.z) {
                return true;
            }
            if (playerPos.z >= movingWalls[i].z && movingWalls[i].z >= after.z) {
                return true;
            }
        }
        else {
            // Other Rooms
            if (playerPos.x <= movingWalls[i].x && movingWalls[i].x <= after.x) {
                return true;
            }
            if (playerPos.x >= movingWalls[i].x && movingWalls[i].x >= after.x) {
                return true;
            }
        }
    }

    return false;
}

void drawBalls() {
    // Red Ball
    if (!redBall.isPickedUp && !redSlot.isBallPlaced) {
        glColor3f(0.8, 0.2, 0.2);
        glPushMatrix();
        glTranslated(redBall.pos.x, redBall.pos.y + thickness + 0.1, redBall.pos.z);
        glutSolidSphere(0.1, 30, 60);
        glPopMatrix();
    }

    // Green Ball
    if (!greenBall.isPickedUp && !greenSlot.isBallPlaced) {
        glColor3f(0.2, 0.8, 0.2);
        glPushMatrix();
        glTranslated(greenBall.pos.x, greenBall.pos.y + thickness + 0.1, greenBall.pos.z);
        glutSolidSphere(0.1, 30, 60);
        glPopMatrix();
    }

    // Blue Ball
    if (!blueBall.isPickedUp && !blueSlot.isBallPlaced) {
        glColor3f(0.2, 0.2, 0.8);
        glPushMatrix();
        glTranslated(blueBall.pos.x, blueBall.pos.y + thickness + 0.1, blueBall.pos.z);
        glutSolidSphere(0.1, 30, 60);
        glPopMatrix();
    }
}

void drawSlots() {
    // Red Slot
    glPushMatrix();
    glTranslated(redSlot.pos.x, redSlot.pos.y + thickness + 0.1, redSlot.pos.z);

    glColor3f(0.6, 0.6, 0.6);
    glPushMatrix();
    glTranslated(0, -0.05, 0);
    glScaled(1.5, 1, 1.5);
    glutSolidCube(0.05);
    glPopMatrix();

    glColor3f(0.8, 0.2, 0.2);
    glPushMatrix();
    glTranslated(0, 0.1, 0);
    if (redSlot.isBallPlaced) {
        glutSolidSphere(0.1, 30, 60);
    }
    else {
        glutWireSphere(0.1, 30, 10);
    }
    glPopMatrix();

    glPopMatrix();

    // Green Slot
    glColor3f(0.2, 0.8, 0.2);
    glPushMatrix();
    glTranslated(greenSlot.pos.x, greenSlot.pos.y + thickness + 0.1, greenSlot.pos.z);

    glColor3f(0.6, 0.6, 0.6);
    glPushMatrix();
    glTranslated(0, -0.05, 0);
    glScaled(1.5, 1, 1.5);
    glutSolidCube(0.05);
    glPopMatrix();

    glColor3f(0.2, 0.8, 0.2);
    glPushMatrix();
    glTranslated(0, 0.1, 0);
    if (greenSlot.isBallPlaced) {
        glutSolidSphere(0.1, 30, 60);
    }
    else {
        glutWireSphere(0.1, 30, 10);
    }
    glPopMatrix();

    glPopMatrix();

    // Blue Slot
    glColor3f(0.2, 0.2, 0.8);
    glPushMatrix();
    glTranslated(blueSlot.pos.x, blueSlot.pos.y + thickness + 0.1, blueSlot.pos.z);
    
    glColor3f(0.6, 0.6, 0.6);
    glPushMatrix();
    glTranslated(0, -0.05, 0);
    glScaled(1.5, 1, 1.5);
    glutSolidCube(0.05);
    glPopMatrix();

    glColor3f(0.2, 0.2, 0.8);
    glPushMatrix();
    glTranslated(0, 0.1, 0);
    if (blueSlot.isBallPlaced) {
        glutSolidSphere(0.1, 30, 60);
    }
    else {
        glutWireSphere(0.1, 30, 10);
    }
    glPopMatrix();

    glPopMatrix();
}

void drawLevelDoor() {
    glColor3f(0.25, 0.1, 0.35);

    glPushMatrix();
    glTranslated(levelDoor.x, levelDoor.y, levelDoor.z);
    glScaled(10, 40, 1);
    glutSolidCube(thickness);
    glPopMatrix();
}

bool isLevelDoorClosed() {
    return !redSlot.isBallPlaced || !greenSlot.isBallPlaced || !blueSlot.isBallPlaced;
}

void drawGameText() {
    if (level == LEVEL_1) {
        glColor3f(0.0, 0.0, 0.0);
    }
    else {
        glColor3f(1.0, 1.0, 1.0);
    }

    //draw score
    char* p0s[20];
    sprintf((char*)p0s, "Score: %d", score);
    print(10, screenH - 30, (char*)p0s);

    //draw time bonus
    char* p1s[20];
    sprintf((char*)p1s, "Time Bonus: %d", timeBonus);
    print(10, screenH - 60, (char*)p1s);

    //draw player pos
    char* p2s[20];
    sprintf((char*)p2s, "X: %f, Y: %f, Z: %f", playerPos.x, playerPos.y, playerPos.z);
    print(10, screenH - 90, (char*)p2s);
}

void drawGameOver() {
    glColor3f(1.0, 1.0, 1.0);

    print(screenW / 2 - 60, screenH / 2 + 30, "Game Over!");

    char* p0s[20];
    sprintf((char*)p0s, "Score: %d", score);
    print(screenW / 2 - 60, screenH / 2 - 30, (char*)p0s);
}

//===============================DISPLAY=================================

void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    if (hasGameEnded) {
        drawGameOver();
    }
    else {
        //camera initialization
        if (firstPerson) {
            glRotated(cameraRotation.y, 1, 0, 0);
            glRotated(cameraRotation.x, 0, 1, 0);

            gluLookAt(eye.x,
                eye.y,
                eye.z,
                eye.x + 1,
                eye.y,
                eye.z,
                up.x,
                up.y,
                up.z);
        }
        else {
            gluLookAt(eye.x - cos(DEGTORAD(cameraRotation.x)) * camDistance,
                eye.y - sin(DEGTORAD(cameraRotation.y)) * 0.3,
                eye.z - sin(DEGTORAD(cameraRotation.x)) * camDistance,
                playerPos.x,
                playerPos.y,
                playerPos.z,
                up.x,
                up.y,
                up.z);

            glColor3f(1, 1, 1);
            drawPlayer(playerPos.x, playerPos.y, playerPos.z);
        }

        setupLights();

        glColor3f(1, 1, 1);

        if (level == LEVEL_1) {
            RenderPresent(0.2, -0.2);
            RenderPresent(-0.2, -0.2);
            RenderPresent(-0.2, 0.2);

            RenderTree();
        }
        else {
            RenderSkull(0.5, 0.5);

            RenderPumpkin(0.2, -0.2);
            RenderPumpkin(-0.2, -0.2);
            RenderPumpkin(-0.2, 0.2);

            RenderSkeleton(-0.5, 0.5);
        }

        RenderGround();

        mainRoom.render();

        rightHallWay.render();
        leftHallWay.render();
        rightRoom.render();
        rightBottomHallway.render();
        bottomRoom.render();
        leftRoom.render();

        drawSpikes();

        glDisable(GL_LIGHTING);	// Disable lighting
        glPushMatrix();

        if (level == LEVEL_2) {
            drawMovingWalls();
        }

        drawBalls();
        drawSlots();

        if (isLevelDoorClosed()) {
            drawLevelDoor();
        }

        // Sky color
        if (level == LEVEL_1) {
            drawSun();
            glColor3f(0.118, 0.565, 1);
        }
        else {
            drawMoon();
            glColor3f(0.2, 0.2, 0.2);
        }

        GLUquadricObj* qobj;
        qobj = gluNewQuadric();
        glTranslated(50, 0, 0);
        glRotated(90, 1, 0, 1);
        glBindTexture(GL_TEXTURE_2D, tex);
        gluQuadricTexture(qobj, true);
        gluQuadricNormals(qobj, GL_SMOOTH);
        gluSphere(qobj, 100, 100, 100);
        gluDeleteQuadric(qobj);
        glPopMatrix();
        glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

        drawGameText();
    }

    glFlush();
}

//===============================MOUSE=================================

void mouseMovement(int x, int y)
{
    int diffX = x - mouseX;
    int diffY = y - mouseY;

    mouseX = x;
    mouseY = y;

    //if camera runs out of space
    if (mouseX <= 0 || mouseX >= screenW - 1)
        glutWarpPointer(screenW / 2, y);

    if (mouseY <= 0 || mouseY >= screenH - 1)
        glutWarpPointer(x, screenH / 2);

    cameraRotation.x += diffX;

    //if camera rotates more than 90 degrees up or down
    if (cameraRotation.y + diffY >= 90) {
        cameraRotation.y = 90;
    }
    else if (cameraRotation.y + diffY <= -90) {
        cameraRotation.y = -90;
    }
    else {
        cameraRotation.y += diffY;
    }

    glutPostRedisplay();
}

//===============================KEYBOARD=================================

void SpecialInput(int key, int x, int y)
{
    if (!hasGameEnded) {
        Vector after(playerPos.x, playerPos.y, playerPos.z);
        Vector Movement(0.0, 0.0, 0.0);
        switch (key)
        {
        case GLUT_KEY_UP:
            Movement.x += cos(DEGTORAD(cameraRotation.x)) * speed;
            Movement.z += sin(DEGTORAD(cameraRotation.x)) * speed;
            break;
        case GLUT_KEY_DOWN:
            Movement.x -= cos(DEGTORAD(cameraRotation.x)) * speed;
            Movement.z -= sin(DEGTORAD(cameraRotation.x)) * speed;
            break;
        case GLUT_KEY_LEFT:
            Movement.x += sin(DEGTORAD(cameraRotation.x)) * speed;
            Movement.z -= cos(DEGTORAD(cameraRotation.x)) * speed;
            break;
        case GLUT_KEY_RIGHT:
            Movement.x -= sin(DEGTORAD(cameraRotation.x)) * speed;
            Movement.z += cos(DEGTORAD(cameraRotation.x)) * speed;
            break;
        }

        after += Movement;
        int n = sizeof(mapRectangles) / sizeof(mapRectangles[0]);
        if (!hasCollidedWithMovingWall(after) && validMove(mapRectangles, n, after)) {
            playerPos = after;
            eye += Movement;

            if (hasSteppedOnSpike()) {
                score = max(0, score - 2);
                playSound(2);
            }
            else {
                playSound(0);
            }

            if (!isLevelDoorClosed() && levelDoor.z >= playerPos.z && levelDoor.x - 0.2 <= playerPos.x && playerPos.x <= levelDoor.x + 0.2) {
                if (level == LEVEL_1) {
                    level = LEVEL_2;

                    playerPos = Vector(0.0, 0.5, 0.0);
                    eye = playerPos;

                    timeBonus = 180;

                    lightRotation = 0.0;
                    incrementLightRotation = false;

                    wallsTranslation = 0.0;
                    areWallsMovingUp = false;

                    redBall.pos = Vector(5.5, 0, 0);
                    greenBall.pos = Vector(-5.5, 0, 0);
                    blueBall.pos = Vector(4.9, 0, 8.5);

                    redBall.isPickedUp = false;
                    greenBall.isPickedUp = false;
                    blueBall.isPickedUp = false;

                    redSlot.isBallPlaced = false;
                    greenSlot.isBallPlaced = false;
                    blueSlot.isBallPlaced = false;

                    pickedBall = NONE;

                    playSound(6);
                    playMusic(1);
                }
                else {
                    hasGameEnded = true;
                    playSound(7);
                }
            }
        }
        else {
            //collision logic
            score = max(0, score - 2);
            playSound(1);
        }

        glutPostRedisplay();
    }
}

void dropPickedUpBall() {
    switch (pickedBall) {
    case RED:
        redBall.pos.x = playerPos.x;
        redBall.pos.z = playerPos.z;
        redBall.isPickedUp = false;
        break;
    case GREEN:
        greenBall.pos.x = playerPos.x;
        greenBall.pos.z = playerPos.z;
        greenBall.isPickedUp = false;
        break;
    case BLUE:
        blueBall.pos.x = playerPos.x;
        blueBall.pos.z = playerPos.z;
        blueBall.isPickedUp = false;
        break;
    }
    pickedBall = NONE;
}

void drawSun() {
    glColor3f(0.9, 0.7, 0.15);
    glPushMatrix();
    glRotated(lightRotation, 0, 0, 1);
    glTranslated(0, 30, 0);
    glutSolidSphere(4, 30, 60);
    glPopMatrix();
}

void drawMoon() {
    glColor3f(0.9, 0.9, 0.9);
    glPushMatrix();
    glRotated(lightRotation, 0, 0, 1);
    glTranslated(0, 30, 0);
    glutSolidSphere(2, 30, 60);
    glPopMatrix();
}

void key(unsigned char k, int x, int y)
{
    if (k == GLUT_KEY_ESCAPE)
        exit(EXIT_SUCCESS);

    if (!hasGameEnded) {
        //distance between camera, player
        if (!firstPerson) {
            if (k == 'z' || k == 'Z') {
                camDistance = max(0.3, camDistance - 0.1);
            }

            if (k == 'x' || k == 'X') {
                camDistance = min(0.8, camDistance + 0.1);
            }
        }

        //toggle first person
        if (k == 'c' || k == 'C') {
            if (!firstPerson) {
                eye = playerPos;
                cameraRotation.y = 0;
            }

            firstPerson = !firstPerson;
        }

        // Pick up a ball.
        if (k == 'p' || k == 'P') {
            if (redBall.pos.x - 0.4 <= playerPos.x && playerPos.x <= redBall.pos.x + 0.4
                && redBall.pos.z - 0.4 <= playerPos.z && playerPos.z <= redBall.pos.z + 0.4
                && !redSlot.isBallPlaced) {
                dropPickedUpBall();
                redBall.isPickedUp = true;
                pickedBall = RED;
                playSound(3);
            }
            else if (greenBall.pos.x - 0.4 <= playerPos.x && playerPos.x <= greenBall.pos.x + 0.4
                && greenBall.pos.z - 0.4 <= playerPos.z && playerPos.z <= greenBall.pos.z + 0.4
                && !greenSlot.isBallPlaced) {
                dropPickedUpBall();
                greenBall.isPickedUp = true;
                pickedBall = GREEN;
                playSound(3);
            }
            else if (blueBall.pos.x - 0.4 <= playerPos.x && playerPos.x <= blueBall.pos.x + 0.4
                && blueBall.pos.z - 0.4 <= playerPos.z && playerPos.z <= blueBall.pos.z + 0.4
                && !blueSlot.isBallPlaced) {
                dropPickedUpBall();
                blueBall.isPickedUp = true;
                pickedBall = BLUE;
                playSound(3);
            }
        }

        // Drop the picked ball.
        if (k == 'd' || k == 'D') {
            switch (pickedBall) {
            case RED:
                if (redSlot.pos.x - 0.4 <= playerPos.x && playerPos.x <= redSlot.pos.x + 0.4
                    && redSlot.pos.z - 0.4 <= playerPos.z && playerPos.z <= redSlot.pos.z + 0.4) {
                    redSlot.isBallPlaced = true;
                    score += timeBonus;
                    if (!isLevelDoorClosed()) playSound(5);
                }
                redBall.pos.x = playerPos.x;
                redBall.pos.z = playerPos.z;
                redBall.isPickedUp = false;
                if (isLevelDoorClosed()) playSound(4);
                break;
            case GREEN:
                if (greenSlot.pos.x - 0.4 <= playerPos.x && playerPos.x <= greenSlot.pos.x + 0.4
                    && greenSlot.pos.z - 0.4 <= playerPos.z && playerPos.z <= greenSlot.pos.z + 0.4) {
                    greenSlot.isBallPlaced = true;
                    score += timeBonus;
                    if (!isLevelDoorClosed()) playSound(5);
                }
                greenBall.pos.x = playerPos.x;
                greenBall.pos.z = playerPos.z;
                greenBall.isPickedUp = false;
                if (isLevelDoorClosed()) playSound(4);
                break;
            case BLUE:
                if (blueSlot.pos.x - 0.4 <= playerPos.x && playerPos.x <= blueSlot.pos.x + 0.4
                    && blueSlot.pos.z - 0.4 <= playerPos.z && playerPos.z <= blueSlot.pos.z + 0.4) {
                    blueSlot.isBallPlaced = true;
                    score += timeBonus;
                    if (!isLevelDoorClosed()) playSound(5);
                }
                blueBall.pos.x = playerPos.x;
                blueBall.pos.z = playerPos.z;
                blueBall.isPickedUp = false;
                if (isLevelDoorClosed()) playSound(4);
                break;
            }
            pickedBall = NONE;
        }

        glutPostRedisplay();
    }
}

//===============================LIGHT=================================

void setupLights() {
    glDisable(GL_LIGHTING);

    //float light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    if (level == LEVEL_1) {
        GLfloat l0Ambient[] = { 1.0f, 1.0f, 0.1f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
        //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    }
    else {
        GLfloat l0Ambient[] = { 1.0f, 1.0f, 0.9f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
        //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    }
    GLfloat l0Position[] = { -30 * sin(DEGTORAD(lightRotation)), 30 * cos(DEGTORAD(lightRotation)), 0.0f, 1 };
    // cout << l0Position[0] << endl;
    // cout << l0Position[1] << endl;

    glLightfv(GL_LIGHT0, GL_POSITION, l0Position);

    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);
}

//===============================MUSIC=================================

void playMusic(int musicConstant) {
    mciSendString("close mp3", NULL, 0, NULL);
    switch (musicConstant) {
    case 0:
        mciSendString("open \"assets/music/music_1.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
        break;
    case 1:
        mciSendString("open \"assets/music/music_2.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
        break;
    }
    mciSendString("play mp3 repeat", NULL, 0, NULL);
}

//===============================SOUND FX=================================

void playSound(int soundConstant) {
    switch (soundConstant) {
    case 0:
        PlaySound("assets/sfx/footstep.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 1:
        PlaySound("assets/sfx/collision.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 2:
        PlaySound("assets/sfx/hurt.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 3:
        PlaySound("assets/sfx/lift_up.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 4:
        PlaySound("assets/sfx/drop.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 5:
        PlaySound("assets/sfx/door_open.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 6:
        PlaySound("assets/sfx/teleport.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 7:
        PlaySound("assets/sfx/game_won.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 8:
        PlaySound("assets/sfx/game_lost.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    }
}

//===============================IDLE/ANIM=================================

void Idle()
{
    if (incrementLightRotation) {
        lightRotation += 0.05;
        if (lightRotation >= 60) incrementLightRotation = false;
    }
    else {
        lightRotation -= 0.05;
        if (lightRotation <= -60) incrementLightRotation = true;
    }

    if (areWallsMovingUp) {
        wallsTranslation += 0.01;
        if (wallsTranslation >= 0.0) areWallsMovingUp = false;
    }
    else {
        wallsTranslation -= 0.01;
        if (wallsTranslation <= -1.2) areWallsMovingUp = true;
    }
    glutPostRedisplay();
}

//===============================ASSETS=================================
void loadAssets() {
    santa.Load("assets/models/Toy Santa/Toy Santa.3ds");
    christmas_tree.Load("assets/models/Christmas tree/Christmas tree N221218.3ds");
    pumpkin.Load("assets/models/Pumpkin Halloween Yoda/Pumpkin Halloween Yoda.3ds");
    present.Load("assets/models/Present/Box christmas present.3ds");
    player2.Load("assets/models/Player2/Handmaiden star wars.3DS");
    skeleton.Load("assets/models/Skelet/Skelet.3ds");
    skull.Load("assets/models/Skull/dino--55k.3ds");

    level1tex_ground.Load("assets/textures/Christmas_red.bmp");
    level1tex_walls.Load("assets/textures/christmas-snowmen.bmp");
    level2tex_ground.Load("assets/textures/halloween-textures2.bmp");
    level2tex_walls.Load("assets/textures/halloween-texture1.bmp");

    tex_ground.Load("assets/textures/ground0.bmp");

    loadBMP(&tex, "assets/textures/blu-sky-3.bmp", true);
}

//===============================MAIN=================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    screenW = glutGet(GLUT_SCREEN_WIDTH);
    screenH = glutGet(GLUT_SCREEN_HEIGHT);

    glutCreateWindow("RGB Collector");

    glutDisplayFunc(Display);
    glutIdleFunc(Idle); //constantly running
    glutKeyboardFunc(key); //keys
    glutSpecialFunc(SpecialInput); //special keys
    loadAssets();
    //mouse
    glutPassiveMotionFunc(mouseMovement);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    //background color
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(95.0f, screenW / screenH, 0.1f, 500.0f);

    //camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x,
        eye.y,
        eye.z,
        eye.x + 1,
        eye.y,
        eye.z,
        up.x,
        up.y,
        up.z);

    glutFullScreen();

    //initialization
    init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}
