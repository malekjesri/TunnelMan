#ifndef ACTOR_H_
#define ACTOR_H_
#include "GameConstants.h"
#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

class Actor : public GraphObject {// Base Class derived from GraphObject
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);//Constructor
    ~Actor();//Destructor
    StudentWorld* getWorld();
    virtual void doSomething() = 0;
    bool alive();
    void die();
    void moveTo(int x, int y);
    virtual void Annoyed(int hP) {}
    //All Characters may use some or all of these functions
    //As every character will do something, be alive or dead, move, and be annoyed
private:
    StudentWorld* m_world;
    bool m_alive;

};

class Earth :public Actor {//Class earth is derived from the base class
public:
    Earth(StudentWorld* world, int startX, int startY);//Constructor
    virtual void doSomething();
};

class Man :public Actor {
public://I addded another layer with functions that are specific to only the characters such as Tunnelman and the protestors
    Man(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp);//Constructor
    int getHP();
    void decHP(int points);
    virtual void moveInDirection(Direction direction) = 0;
    virtual void Annoyed(int hp)=0;
    //all of them have health, all of them move and get annoyed unlike the other classes in actor.h
private:
    int m_hP;
};

class Tunnelman :public Man{//Derived from class man that is derived from the base class Actor
public:
    Tunnelman(StudentWorld* world);//Constructor
    ~Tunnelman();
    void add(int id);
    int Water();
    int getSonar();
    int getGold();
    virtual void doSomething();
    void shoot();
    virtual void Annoyed(int hP);
    virtual void moveInDirection(Direction direction);
private:
    int m_water;
    int m_sonar;
    int m_gold;
};

class Protester : public Man{
public:
    Protester(StudentWorld* world, int imageID,int hP);//Constructor
    virtual void doSomething();
    virtual void moveInDirection(Direction direction);
    void getBribed();
    void randomNumToMove();
    virtual void Annoyed(int hP);
    void getStunned();
    bool isFacingPlayer();
    Direction directionToPlayer();
    bool straightPathToPlayer(Direction direction);
    Direction randomDirection();
    bool isAtIntersection();
    void pickViableDirectionToTurn();

private:
    bool m_leave;
    int m_numToMove;
    int m_tickSinceLastTurn;
    int m_tickNoYell;
    int ticksToWaitBetweenMoves;
};
class RegularProtester : public Protester {
public:
    RegularProtester(StudentWorld* world);//Constructor
};

class HardcoreProtester : public Protester {
public:
    HardcoreProtester(StudentWorld* world);//Constructor
};


class Boulder : public Actor {
public:
    Boulder(StudentWorld* world, int startX, int startY);//Constructor
    virtual void doSomething();
    void annoyMan();
private:
    bool isStable;
    bool isFalling;
    int m_ticks;

};
class Squirt : public Actor {
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction dir);//Constructor
    virtual void doSomething();
    bool annoyProtesters();
// bool annoyProtesters(int x, int y, int amt);
//virtual void doSomething(int amt);
private:
    //std::vector<Actor*> Actors;
    int m_travel;
};

class Pickupable :public Actor {
public:
    Pickupable(StudentWorld* world, int imageID, int startX, int startY);//Constructor
    virtual void doSomething() = 0;
    void Disappear(int ticks);
private:
    int m_tick;
};

class Barrel : public Pickupable {
public:
    Barrel(StudentWorld* world, int startX, int startY);//Constructor
    virtual void doSomething();
};
class Gold :public Pickupable {
public:
    Gold(StudentWorld* world, int startX, int startY, bool visible,bool dropped);//Constructor
    virtual void doSomething();
private:
    bool isDropped;
};

class Goodie : public Pickupable {
public:
    Goodie(StudentWorld* world, int imageID, int startX, int startY);//Constructor
    virtual void doSomething();
};

class Sonar :public Goodie {
public:
    Sonar(StudentWorld* world, int startX, int startY);//Constructor
};
class WaterPool: public Goodie {
public:
    WaterPool(StudentWorld* world, int startX, int startY);//Constructor
};



#endif // ACTOR_H_
