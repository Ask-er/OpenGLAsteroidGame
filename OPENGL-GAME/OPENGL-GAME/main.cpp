#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>


#define _USE_MATH_DEFINES 
#include <math.h>
#include <map>
#include <string>
#include <list>
#include <time.h>       

#define MAX_ASTEROIDS 5                               // GOD MODE
#define PLAYER_LIFE 3                                   //1000
#define ADD_HEALTH_PER_ASTEROID 10                       //1
#define BULLET_CHARGE 100                               //10000000
float DEGTORAD = 0.017453f;
const int W = 640;
const int H = 640;
int asteroids = 0;
int destroyedAsteroids = 0; // 10 kill 1 cana denk geliyor, eðer canýn 3 ise ekleme yapýlmayacak

std::map< int, bool > keys;

void special(int key, int x, int y)
{
    keys[key] = true;
}
void specialUp(int key, int x, int y)
{
    keys[key] = false;
}
float randomFloat(float HI, float LO) {

    return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}
class Entity
{
public:
    float x, y, dx, dy, angle, R;
    bool life;
    bool thrust;
    int Bullet_charge;

    std::string name;
    int healt;
    Entity() { life = 1; }

    void settings(float X, float Y, float Angle, float radius, int Healt = 1) {
        x = X; y = Y;
        angle = Angle;
        R = radius;
        healt = Healt;
    }

    virtual void update() {};
    virtual void draw() {};
    virtual void drawCircle() {};

    virtual ~Entity() {};

};
int arr[4][2] = { {1,1}, {1,-1},{ -1,1}, {-1,-1} };
class asteroid : public Entity
{
public:
    asteroid()
    {
        int rnd_index = rand() % 4;
        dx = arr[rnd_index][0] * randomFloat(0.005, 0.013);
        dy = arr[rnd_index][1] * randomFloat(0.00732f, 0.0125);
        name = "asteroid";
    }

    void update()
    {
        x += dx;
        y += dy;

        angle += 3.3;
        if (x > 2) x = -2;  if (x < -2) x = 2;
        if (y > 2) y = -2;  if (y < -2) y = 2;
    }

    void draw() {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glTranslatef(x, y, 0);
        glRotatef(angle, 0, 0, 1);

        glLineWidth(2.0);

        glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2d(0, 0);
        glVertex2d(-0.1, -0.1);
        glVertex2d(-0.17, -0.1);
        glVertex2d(-0.21, 0);
        glVertex2d(-0.125, 0.135);
        glVertex2d(0, 0.2);
        glVertex2d(0.15, 0.145);
        glVertex2d(0.21, 0);
        glVertex2d(0.17, -0.1);
        glVertex2d(0.1, -0.1);

        glEnd();
        glPopMatrix();
    }

    void drawCircle() {// For collection detection view
        int lineAmount = 100;
        float twicePi = 2.0f * M_PI;
        glColor3f(1, 0, 0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= lineAmount; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(lineAmount);
            float cx = R * cosf(theta);
            float cy = R * sinf(theta);
            glVertex2f(x + cx, y + cy);
        }
        glEnd();
    }
};

class bullet : public Entity
{
public:
    bullet()
    {
        name = "bullet";
    }

    void  update()
    {
        dx = cos(angle * DEGTORAD) * 0.1;
        dy = sin(angle * DEGTORAD) * 0.1;

        x += dx;
        y += dy;

        if (x > 2 || x < -2 || y>2 || y < -2) life = 0;
    }
    void draw() {

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();


        glTranslatef(x, y, 0);
        glLineWidth(1);
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_LOOP);
        glVertex2f(0.0, 0.00);
        glVertex2f(0.0f, 0.01f);
        glVertex2f(0.01f, 0.01f);

        glEnd();

        glPopMatrix();
    }
};
class player : public Entity
{
public:
    player()
    {
        name = "player";
    }
    void update()
    {
        if (thrust)
        {
            dx += cos(angle * DEGTORAD) * 0.02;
            dy += sin(angle * DEGTORAD) * 0.02;
        }
        else
        {
            dx *= 0.99;
            dy *= 0.99;
        }

        float maxSpeed = 0.02;
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }
        x += dx;
        y += dy;

        if (x > 2) x = -2; if (x < -2) x = 2;
        if (y > 2) y = -2; if (y < -2) y = 2;
    }
    void draw() {

        glClearColor(0, 0, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glTranslatef(x, y, 0);
        glRotatef(angle, 0, 0, 1);
        glLineWidth(2.0);
        glBegin(GL_LINE_LOOP);

        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(0.0f, 0.0f);

        glVertex2f(-0.05, 0.1);
        glVertex2f(0.15, 0);
        glVertex2f(-0.05, -0.1);
        glEnd();
        glPopMatrix();

        // FOR LIFE BAR
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glBegin(GL_LINE_LOOP);
        glVertex2f(-1.98, 1.9);
        glVertex2f(-1.98, 1.98);
        glVertex2f(-1.78, 1.98);
        glVertex2f(-1.78, 1.9);
        glEnd();
        glPopMatrix();

        glColor3f(1.0f, 0, 0);
        if (healt >= 3) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glBegin(GL_QUADS);
            glVertex2f(-1.96, 1.92); glVertex2f(-1.96, 1.96); glVertex2f(-1.92, 1.96); glVertex2f(-1.92, 1.92);
            glEnd();
            glPopMatrix();
        }
        if (healt >= 2) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glBegin(GL_QUADS);
            glVertex2f(-1.9, 1.92); glVertex2f(-1.9, 1.96); glVertex2f(-1.86, 1.96); glVertex2f(-1.86, 1.92);
            glEnd();
            glPopMatrix();
        }
        if (healt >= 1) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glBegin(GL_QUADS);
            glVertex2f(-1.84, 1.92); glVertex2f(-1.84, 1.96); glVertex2f(-1.8, 1.96); glVertex2f(-1.8, 1.92);
            glEnd();
            glPopMatrix();
        }

        // FOR BULLET CHARGE BAR
        glColor3f(1, 1, 0);
        for (int i = 1; i <= Bullet_charge / 5; i++) {
            float add = i * 0.04;
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();

            glBegin(GL_QUADS);
            glVertex2f(1.96, -1.96 + add); glVertex2f(1.96, -1.94 + add); glVertex2f(1.92, -1.94 + add); glVertex2f(1.92, -1.96 + add);
            glEnd();
            glPopMatrix();
        }


    }
    void drawCircle() { // For collection detection view

        int lineAmount = 100;
        float twicePi = 2.0f * M_PI;
        glColor3f(1, 0, 0);

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= lineAmount; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(lineAmount);
            float cx = R * cosf(theta);
            float cy = R * sinf(theta);
            glVertex2f(x + cx, y + cy);
        }
        glEnd();
    }
};

std::list<Entity*> entities; // all entities

bool isCollide(Entity* a, Entity* b)
{
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R);
}

int bullettimer = 1;
void display() {


    std::list<Entity*>::iterator p = entities.begin();

    if ((*p)->name == "player") {
        bullettimer += 1;
        if (bullettimer % 25 == 0 && (*p)->Bullet_charge != 100) {
            (*p)->Bullet_charge += 5;

        }

        if (keys[GLUT_KEY_CTRL_L] && (*p)->Bullet_charge != 0) { // find space key code and bulluets are very fast spawn  !!!!!!!!!!!!!!
            bullet* b = new bullet();
            float x = (*p)->x;
            float y = (*p)->y;
            b->settings(x, y, (*p)->angle, 0.01);
            entities.push_back(b);
            (*p)->Bullet_charge -= 5;

        }
        if (keys[GLUT_KEY_LEFT]) (*p)->angle += 3;

        if (keys[GLUT_KEY_RIGHT]) (*p)->angle -= 3;

        if (keys[GLUT_KEY_UP]) (*p)->thrust = true;
        else  (*p)->thrust = false;
    }



    for (auto a : entities)
        for (auto b : entities)
        {
            if (a->name == "asteroid" && b->name == "bullet")
                if (isCollide(a, b))
                {
                    a->life = false;
                    b->life = false;

                    //Entity* e = new Entity();
                    //e->settings(a->x, a->y, 0, 0); // distroy astreoid
                    //e->name = "explosion";
                    //entities.push_back(e);
                }
            if (a->name == "player" && b->name == "asteroid")
                if (isCollide(a, b))
                {
                    b->life = false;
                    a->healt -= 1;
                    if (a->healt == 0) a->life = false;
                    else {

                        a->settings(0, 0, 0, 0.12, a->healt);
                        a->dx = 0; a->dy = 0;
                        a->Bullet_charge = 100;
                    }
                }
        }

    for (auto i = entities.begin(); i != entities.end();)
    {
        Entity* e = *i;
        e->update();
        if (e->life == false) {
            if (e->name == "asteroid") {
                asteroids -= 1;
                destroyedAsteroids += 1;
                if (destroyedAsteroids % ADD_HEALTH_PER_ASTEROID == 0) // add HEALTH
                    if ((*p)->healt <= 2)
                    {
                        (*p)->healt += 1;
                        destroyedAsteroids -= ADD_HEALTH_PER_ASTEROID;
                    }
            }

            i = entities.erase(i);
            delete e;
        }
        else i++;
    }
    for (auto i : entities) {
        i->draw();
        //i->drawCircle(); //circle view
    }

}

bool flag_player = true;
void update() {

    glClear(GL_COLOR_BUFFER_BIT);

    if (flag_player) {
        player* p = new player();
        p->settings(0, 0, 0, 0.12, PLAYER_LIFE);
        p->dx = 0;
        p->dy = 0;
        p->Bullet_charge = BULLET_CHARGE;
        entities.push_back(p);
        flag_player = false;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2, 2, -2, 2); // koordinant sistemi (-2, 2) weight, (-2,2) height þeklinde. W and H must be equal.



    for (int i = asteroids; i < MAX_ASTEROIDS; i++) {

        asteroid* a = new asteroid();
        a->settings(randomFloat(-2, 2), randomFloat(-2, 2), rand() % 360, 0.2);
        entities.push_back(a);
        asteroids++;

    }

    display();

    glutSwapBuffers();

    glFlush();
}



void timer(int value)
{
    glutTimerFunc(16, timer, 0);

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(5, 5);
    glutInitWindowSize(W, H);
    glutCreateWindow("ASTEROID");

    srand(static_cast <unsigned> (time(0)));
    glutDisplayFunc(update);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    return 0;
}