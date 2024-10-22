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

//=======================DEFINITIONS======================================
#define DEGTORAD(x) x*3.1415/180.0
#define GLUT_KEY_ESCAPE 27

//Textures
GLTexture level1tex_walls;
GLTexture level2tex_walls;
GLTexture level2tex_ground;
GLTexture tex_ground;
GLuint tex;

class Vector;
class Room;
void drawUnitCube();
bool insideRectangle(Vector bottomLeft, Vector topRight, Vector p);
bool validMove(Vector map[], int n, Vector p);
void print(float x, float y, float z, char* string);
void init();
void Display(void);
void mouseMovement(int x, int y);
void SpecialInput(int key, int x, int y);
void key(unsigned char k, int x, int y);
void Idle();
void loadAssets();
void drawTexturedFace(int type, GLTexture _tex, int rep);
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

    Vector& normalize(){
        float mag = sqrt(x*x + y*y + z*z);

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
            //drawTexturedFace(2, level1tex_walls, 1);
            drawTexturedFace(2, level2tex_walls, 4);
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
            //drawTexturedFace(2, level1tex_walls, 1);
            drawTexturedFace(2, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x - sx / 2.0 + ((sx / 2.0) - (sx * (doorPortions[0] / 2.0))) / 2.0,
                y + wallHeight / 4.0 + (sy / 2.0),
                z - (sz / 2.0) + (sy / 2.0));
            glScaled((sx / 2.0) - (sx * (doorPortions[0] / 2.0)), wallHeight / 2.0, sy);
            //drawTexturedFace(2, level1tex_walls, 1);
            drawTexturedFace(2, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x,
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z - (sz / 2.0) + (sy / 2.0));
            glScaled(sx, wallHeight / 2.0, sy);
            //drawTexturedFace(2, level1tex_walls, 1);
            drawTexturedFace(2, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();
        }
    }



    void drawBackWall(int type) {

        if (type == 0) {
            glPushMatrix();
            glTranslated(x, y + wallHeight / 2.0 + (sy / 2.0), z + (sz / 2.0) - (sy / 2.0));
            glScaled(sx, wallHeight, sy);
            //drawTexturedFace(3, level1tex_walls, 1);
            drawTexturedFace(3, level2tex_walls, 1);
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
            //drawTexturedFace(3, level1tex_walls, 1);
            drawTexturedFace(3, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x - sx / 2.0 + ((sx / 2.0) - (sx * (doorPortions[2] / 2.0))) / 2.0,
                y + wallHeight / 4.0 + (sy / 2.0),
                z + (sz / 2.0) - (sy / 2.0));
            glScaled((sx / 2.0) - (sx * (doorPortions[2] / 2.0)), wallHeight / 2.0, sy);
            //drawTexturedFace(3, level1tex_walls, 1);
            drawTexturedFace(3, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x,
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z + (sz / 2.0) - (sy / 2.0));
            glScaled(sx, wallHeight / 2.0, sy);
            //drawTexturedFace(3, level1tex_walls, 1);
            drawTexturedFace(3, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();
        }

    }

    void drawLeftWall(int type) {
        if (type == 0) {
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0), y + wallHeight / 2.0 + (sy / 2.0), z);
            glScaled(sy, wallHeight, sz);
            //drawTexturedFace(5, level1tex_walls, 1);
            drawTexturedFace(5, level2tex_walls, 1);
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
            //drawTexturedFace(5, level1tex_walls, 1);
            drawTexturedFace(5, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0),
                y + wallHeight / 4.0 + (sy / 2.0),
                z + sz / 2.0 - ((sz / 2.0) - (sz * (doorPortions[3] / 2.0))) / 2.0);
            glScaled(sy, wallHeight / 2.0, (sz / 2.0) - (sz * (doorPortions[3] / 2.0)));
            //drawTexturedFace(5, level1tex_walls, 1);
            drawTexturedFace(5, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x - (sx / 2.0) + (sy / 2.0),
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z);
            glScaled(sy, wallHeight / 2.0, sz);
            //drawTexturedFace(5, level1tex_walls, 1);
            drawTexturedFace(5, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();
        }
    }

    void drawRightWall(int type) {
        if (type == 0) {
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0), y + wallHeight / 2.0 + (sy / 2.0), z);
            glScaled(sy, wallHeight, sz);
            //drawTexturedFace(4, level1tex_walls, 1);
            drawTexturedFace(4, level2tex_walls, 1);
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
            //drawTexturedFace(4, level1tex_walls, 1);
            drawTexturedFace(4, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 2
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0),
                y + wallHeight / 4.0 + (sy / 2.0),
                z + sz / 2.0 - ((sz / 2.0) - (sz * (doorPortions[1] / 2.0))) / 2.0);
            glScaled(sy, wallHeight / 2.0, (sz / 2.0) - (sz * (doorPortions[1] / 2.0)));
            //drawTexturedFace(4, level1tex_walls, 1);
            drawTexturedFace(4, level2tex_walls, 1);
            drawUnitCube();
            glPopMatrix();

            //part 3
            glPushMatrix();
            glTranslated(x + (sx / 2.0) - (sy / 2.0),
                y + wallHeight / 2.0 + wallHeight / 4.0 + (sy / 2.0),
                z);
            glScaled(sy, wallHeight / 2.0, sz);
            //drawTexturedFace(4, level1tex_walls, 1);
            drawTexturedFace(4, level2tex_walls, 1);
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
        drawTexturedFace(0, level2tex_ground, 2);
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
//3D Models
Model_3DS christmas_tree;
Model_3DS santa;
Model_3DS model_house;
Model_3DS pumpkin;
Model_3DS present;
Model_3DS player2;
Model_3DS skeleton;
Model_3DS skull;

//camera
Vector eye(0.0,0.5,0.0);
Vector ref(1.0,0.5,0.0);
Vector up(0.0,1.0,0.0);
bool firstPerson = true;
Vector playerPos(0.0, 0.5, 0.0);
Vector cameraRotation(0.0,0.0,0.0);
float camDistance = 0.4;

//movement speed
float speed = 0.3;

//screen
int width = 1000;
int height = 1000;

//mouse position
float mouseX, mouseY;

//wall/floor thickness
float thickness = 0.05;
float halfThickness = 0.025;

//the coordinates used for collision when the player collects all the balls
Vector winningRoom[] = {
        //winning room
        Vector(- 0.25 + thickness, halfThickness , -2 + thickness), //bottom-left
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

//score
int score = 0;
int timeBonus = 501;

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
void drawPlayer(float x, float y, float z){
    glPushMatrix();

    //TODO: replace the 0.2 with half the player's height
    glTranslated(x, thickness + 0.2, z);

    //TODO: adjust the scaling factors
    // until you get a reasonable size for the player
    glScaled(0.2,0.4, 0.2);
    //TODO: load model instead of cube
    //RenderSanta(); if level1
    //RenderPlayer2(); if level2
    glRotated(-cameraRotation.x, 0, 1, 0);
    drawUnitCube();
    glPopMatrix();
}

//draws 1x1x1 cube
void drawUnitCube() {
    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
    // Top face (y = 0.5f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    // Bottom face (y = -0.5f)
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Front face  (z = 0.5f)
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Back face (z = -0.5f)
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Left face (x = -0.5f)
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face (x = 0.5f)
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
    float margin = 0.02;
    if (p.x > bottomLeft.x + margin && p.x < topRight.x - margin
        && p.z > topRight.z + margin && p.z < bottomLeft.z - margin
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

//    TODO: when player collects all the balls
//    this will allow the player to walk into the winning door
//    if(/*win*/){
//        valid |=  insideRectangle(winningRoom[0], winningRoom[1], p);
//    }

    return valid;
}

//print text
void print(float x, float y, float z, char* string)
{
    int len, i;

    //TODO: set the position of the text in the window using the x and y coordinates
    // if glWindowpos3f doesn't work try glRasterPos2d()
    //glWindowPos3f(x, y, z);

    //get the length of the string to display
    len = (int)strlen(string);

    //loop to display character by character
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
}

void second(int val)
{
    timeBonus = max(timeBonus - 1, 0);
    glutPostRedisplay();						// redraw
    glutTimerFunc(1000, second, 0);					//recall the time function after 1000 ms and pass a zero value as an input to the time func.
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
    second(0);
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
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 0.4, z);
    glScaled(0.004, 0.004, 0.004);
    santa.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
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
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslated(x, 0.2, z);
    glScaled(0.005, 0.005, 0.005);
    player2.Draw();
    glPopMatrix();
    glEnable(GL_LIGHTING);
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

//===============================DISPLAY=================================

void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();


    //camera initialization
    if(firstPerson) {
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
    } else{

        gluLookAt(eye.x - cos(DEGTORAD(cameraRotation.x)) * camDistance,
                  eye.y - sin(DEGTORAD(cameraRotation.y)) * 0.3,
                  eye.z - sin(DEGTORAD(cameraRotation.x)) * camDistance,
                  playerPos.x,
                  playerPos.y,
                  playerPos.z,
                  up.x,
                  up.y,
                  up.z);

        //TODO: render the player here
        glColor3f(1, 1, 1);
        drawPlayer(playerPos.x, playerPos.y, playerPos.z);
    }

    glColor3f(1, 1, 1);
//    RenderPlayer2(-1.0, 0.5);
    RenderSkull(0.5, 0.5);
//    RenderSanta(0.5, -0.5);
//    RenderPresent(0.2,-0.2);
//    RenderPresent(-0.2, -0.2);
//    RenderPresent(-0.2, 0.2);

//    RenderTree();

    RenderPumpkin(0.2, -0.2);
    RenderPumpkin(-0.2, -0.2);
    RenderPumpkin(-0.2, 0.2);


    RenderSkeleton(-0.5,0.5);

    RenderGround();

    mainRoom.render();

    rightHallWay.render();
    leftHallWay.render();
    rightRoom.render();
    rightBottomHallway.render();
    bottomRoom.render();
    leftRoom.render();

    //draw score
    glColor3f(0.118, 0.565, 1);
    char* p0s[20];
    sprintf((char*)p0s, "Score: %d", score);
    print(10, width - 130, 0, (char*)p0s);

    //draw time bonus
    char* p1s[20];
    sprintf((char*)p1s, "Time bonus: %d", timeBonus);
    print(10, width - 155, 0, (char*)p1s);

    glDisable(GL_LIGHTING);	// Disable lighting
    glPushMatrix();

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
    if (mouseX <= 0 || mouseX >= width - 1)
        glutWarpPointer(width / 2, y);

    if (mouseY <= 0 || mouseY >= height - 1)
        glutWarpPointer(x, height / 2);

    cameraRotation.x += diffX;

    //if camera rotates more than 90 degrees up or down
    if(cameraRotation.y  + diffY >= 90){
        cameraRotation.y = 90;
    }
    else if(cameraRotation.y + diffY <= -90){
        cameraRotation.y = -90;
    }
    else{
        cameraRotation.y += diffY;
    }


    glutPostRedisplay();
}

//===============================KEYBOARD=================================

void SpecialInput(int key, int x, int y)
{
    Vector after(playerPos.x,playerPos.y,playerPos.z);
    Vector Movement(0.0,0.0,0.0);
    switch(key)
    {
        case GLUT_KEY_UP:
            Movement.x += cos(DEGTORAD(cameraRotation.x))*speed;
            Movement.z += sin(DEGTORAD(cameraRotation.x))*speed;
            break;
        case GLUT_KEY_DOWN:
            Movement.x -= cos(DEGTORAD(cameraRotation.x))*speed;
            Movement.z -= sin(DEGTORAD(cameraRotation.x))*speed;
            break;
        case GLUT_KEY_LEFT:
            Movement.x += sin(DEGTORAD(cameraRotation.x))*speed;
            Movement.z -= cos(DEGTORAD(cameraRotation.x))*speed;
            break;
        case GLUT_KEY_RIGHT:
            Movement.x -= sin(DEGTORAD(cameraRotation.x))*speed;
            Movement.z += cos(DEGTORAD(cameraRotation.x))*speed;
            break;
    }

    after += Movement;
    int n = sizeof(mapRectangles) / sizeof(mapRectangles[0]);
    if(validMove(mapRectangles, n, after)){
        playerPos = after;
        eye += Movement;
    }
    else{
        //collision logic
        score = std::max(0, score - 2);
        //TODO: add collision sound here
    }

    glutPostRedisplay();
}

void key(unsigned char k, int x, int y)
{
    if (k == GLUT_KEY_ESCAPE)
        exit(EXIT_SUCCESS);

    //distance between camera, player
    if(!firstPerson) {
        if (k == 'z' || k == 'Z') {
            camDistance = std::max(0.3,camDistance - 0.1);

        }

        if (k == 'x' || k == 'X') {
            camDistance = std::min(0.8,camDistance + 0.1);
        }
    }

    //toggle first person
    if (k == 'c' || k == 'C'){
        if(!firstPerson){
            eye = playerPos;
            cameraRotation.y = 0;
        }

        firstPerson = !firstPerson;
    }

    glutPostRedisplay();
}

//===============================IDLE/ANIM=================================

void Idle()
{
    glutPostRedisplay();

}

//===============================ASSETS=================================
void loadAssets() {
    santa.Load("Models/Toy Santa/Toy Santa.3ds");
    christmas_tree.Load("Models/Christmas tree/Christmas tree N221218.3ds");
    model_house.Load("Models/house/house.3DS");
    pumpkin.Load("Models/Pumpkin Halloween Yoda/Pumpkin Halloween Yoda.3ds");
    present.Load("Models/Present/Box christmas present.3ds");
    player2.Load("Models/Player2/Handmaiden star wars.3DS");
    skeleton.Load("Models/Skelet/Skelet.3ds");
    skull.Load("Models/Skull/dino--55k.3ds");

    level1tex_walls.Load("Textures/christmas-snowmen.bmp");
    level2tex_ground.Load("Textures/halloween-textures2.bmp");
    level2tex_walls.Load("Textures/halloween-texture1.bmp");

    tex_ground.Load("Textures/ground0.bmp");

    loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//===============================MAIN=================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutCreateWindow("to be named");

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
    gluPerspective(95.0f, width / height, 0.1f, 500.0f);

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

