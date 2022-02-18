#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
    :GraphObject(imageID, startX, startY, dir, size, depth), m_world(world),m_alive(true)
{
    setVisible(true);
}
Actor::~Actor()//Destructor
{
    setVisible(false);
}
StudentWorld* Actor::getWorld() {
    return m_world;
}
bool Actor::alive() {
    return m_alive;
}
void Actor::die() {
    m_alive = false;
}
void Actor::moveTo(int x, int y)
{//if the the play tries to move outside of range we bring them back in the range
    if (x < 0) x = 0;
    if (x > 60) x = 60;
    if (y < 0) y = 0;
    if (y > 60)  y = 60;
    GraphObject::moveTo(x, y);
}



//Man
Man::Man(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp)
    :Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hP(hp) {}
int Man::getHP() {
    return m_hP;
}

void Man::decHP(int points)
{
    m_hP -= points;
}

//Tunnelman
Tunnelman::Tunnelman(StudentWorld* world)
    :Man(world, TID_PLAYER, 30, 60, right, 10), m_water(5), m_sonar(1), m_gold(0) {}
Tunnelman::~Tunnelman(){}

void Tunnelman::add(int id)
{//when the Tunnelman picks up gol, water or sonar we say how many to add
    if (id == TID_GOLD) m_gold++;
    if (id == TID_WATER_POOL) m_water+=5;
    if (id == TID_SONAR) m_sonar += 2;
}

void Tunnelman::doSomething()
{//Here I coded how the tunnelman moves and all his actuons
    if (!alive()) return;
    if (getWorld()->removeEarth(getX(), getY()))
        getWorld()->playSound(SOUND_DIG);
    int input;
    if (getWorld()->getKey(input) == true) {
        //We check if the user hit a key this tick and we code the actions accordingly
        switch (input)
        {
        case KEY_PRESS_ESCAPE:
            die();//First case is if the user decides to quit the game we treat it as if he died
            break;
        case KEY_PRESS_LEFT: moveInDirection(left); break;
        case KEY_PRESS_RIGHT:moveInDirection(right); break;
        case KEY_PRESS_UP:   moveInDirection(up); break;
        case KEY_PRESS_DOWN: moveInDirection(down); break;
        case KEY_PRESS_SPACE:
            if (m_water > 0) {
                m_water--;//If the user shoots we subract from the amount of water the user has the call the shoot functions
                shoot();
            }
            break;
        case 'z':
        case 'Z':
            if (m_sonar > 0)
            {//IF the user has any sonar left we subtract a sonar then trigger the "sonarSonared" function
                m_sonar--;
                getWorld()->sonarSonared(getX(), getY(), 12);
                getWorld()->playSound(SOUND_SONAR);
            }
            break;
        case KEY_PRESS_TAB:
            if (m_gold > 0) {//when we want to bribe the protesters we call the addActor function and add gold
                getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
                m_gold--;
            }
            break;
        }
    }
}

void Tunnelman::shoot() {
    switch (getDirection())
    {
    case left:
        if(!getWorld()->isThereEarth(getX()-4,getY())&& !getWorld()->isThereBoulder(getX()-4, getY()))
            getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
        break;
    case right:
        if (!getWorld()->isThereEarth(getX() + 4, getY()) && !getWorld()->isThereBoulder(getX()+4, getY()))
            getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
        break;
    case down:
        if (!getWorld()->isThereEarth(getX() , getY()-4) && !getWorld()->isThereBoulder(getX(), getY()-4))
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
        break;
    case up:
        if (!getWorld()->isThereEarth(getX(), getY() + 4) && !getWorld()->isThereBoulder(getX(), getY()+4 ))
        getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
        break;
    case none:return;
    }
    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}

void Tunnelman::Annoyed(int hP)
{//Whenever Annoyed is called the Actor loses health, if hitpoints is below 0 then the actor "dies"
    decHP(hP);
    if (getHP() <= 0) {
        die();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

void Tunnelman::moveInDirection(Direction direction)
{
    switch (direction) {//As long as nothing is in the wat we can move in that direction
    case left:
        if (getDirection() == left) {
            if (!getWorld()->isThereBoulder(getX() - 1, getY()))
                moveTo(getX() - 1, getY());
            else moveTo(getX(), getY());
        }
        else setDirection(left);
        break;
    case right:
        if (getDirection() == right) {
            if (!getWorld()->isThereBoulder(getX() + 1, getY()))
                moveTo(getX() + 1, getY());
            else moveTo(getX(), getY());
        }
        else setDirection(right);
        break;
    case up:
        if (getDirection() == up) {
            if (!getWorld()->isThereBoulder(getX(), getY() + 1))
                moveTo(getX(), getY() + 1);
            else moveTo(getX(), getY());
        }
        else setDirection(up);
        break;
    case down:
        if (getDirection() == down) {
            if (!getWorld()->isThereBoulder(getX(), getY() - 1))
                moveTo(getX(), getY() - 1);
            else moveTo(getX(), getY());
        }
        else setDirection(down);
        break;
    case none: return;
    }
}

int Tunnelman::Water()
{
    return m_water;
}

int Tunnelman::getSonar()
{
    return m_sonar;
}

int Tunnelman::getGold()
{
    return m_gold;
}

//Earth
Earth::Earth(StudentWorld* world, int startX, int startY)
    :Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}
void Earth::doSomething() {}  // dont do anything


//Boulder
Boulder::Boulder(StudentWorld* world, int startX, int startY)
    : Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1),isStable(true),m_ticks(0)
{
    world->removeEarth(startX, startY);
}

void Boulder::doSomething() {
    if (!alive()) return;
    if (isStable) {//Here we code the action of the boulder and make sure it is above earth
        if (getWorld()->isAboveEarth(getX(), getY() - 1))
            return;
        else isStable = false;
    }
    if (m_ticks == 30) {
        isFalling = true;
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
    m_ticks++;
    if (isFalling) {
        if (getWorld()->isAboveEarth(getX(), getY()-1)|| getWorld()->isThereBoulder(getX(), getY()-4, 0) || getY()==0)
            die();
        else moveTo(getX(), getY() - 1);
        annoyMan();//When the boulder falls straight down it checks if it hits earth or an actor
    }
}

void Boulder::annoyMan()
{//if player is in radius it hits the player or protester and "Annoys him"
    if (getWorld()->isPlayerInRadius(this, 3)) {
        getWorld()->getPlayer()->Annoyed(100);
    }
    Protester* p = getWorld()->protesterInRadius(this, 3);
    if (p != nullptr) {
        p->Annoyed(100);
    }
}

//Squirt
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir)
    :Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1),m_travel(0) {}
void Squirt::doSomething() {
    if (!alive()) return;
    if (annoyProtesters()|| m_travel == 4 ) {
        die();
        return;//If the water hits a protestors it disapear
    }
    switch (getDirection()) {
    case left://Now if the squirt hits the earth or a boulder from any direction it disapears
        if (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereBoulder(getX() - 1, getY())) {
            die(); return;
        }
        else   moveTo(getX() - 1, getY());
        break;
    case right:
        if (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereBoulder(getX() + 1, getY())) {
            die(); return;
        }
        else   moveTo(getX() + 1, getY());
        break;
    case up:
        if (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereBoulder(getX(), getY() + 1)) {
            die(); return;
        }
        else moveTo(getX(), getY() + 1);
        break;
    case down:
        if (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 1)) {
            die(); return;
        }
        else moveTo(getX(), getY() - 1);
        break;
    case none : return;
    }
    m_travel++;
}

bool Squirt::annoyProtesters()
{
    Protester *p= getWorld()->protesterInRadius(this,3);
    if (p != nullptr) {
        p->Annoyed(2);
        return true;
    }//If protestor is in radius of the squirt then we annoy him
    return false;
}


//Pickupable
Pickupable::Pickupable(StudentWorld* world, int imageID, int startX, int startY)
    :Actor(world, imageID, startX, startY, right, 1.0, 2) {}

void Pickupable::Disappear(int ticks)
{
    if (m_tick == ticks) die();
    else m_tick++;
}

//Barrel
Barrel::Barrel(StudentWorld* world, int startX, int startY)
    :Pickupable(world, TID_BARREL, startX, startY)
{
    setVisible(false);
}
void Barrel::doSomething() {
    if (!alive()) return;
    if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
        setVisible(true);//when the player is near we make the barrel visable
        return;
    }
    if (getWorld()->isPlayerInRadius(this, 3)) {
        die();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->decBarrel();
        return;//if the player picks up the barrel then it disapears, a sound is played, the score is increased the scoreboard says there is one less barrel
    }
}

//Gold
Gold::Gold(StudentWorld* world, int startX, int startY, bool isVisible, bool dropped)
    :Pickupable(world, TID_GOLD, startX, startY), isDropped(dropped)
{
    setVisible(isVisible);
}
void Gold::doSomething() {//Gold is basically the same as the oil barrel except for the fact that you can use it bribe the protestors
    if (!alive()) return;
    if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
        setVisible(true);
        return;
    }
    if (!isDropped && getWorld()->isPlayerInRadius(this, 3)) {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->add(getID());
        getWorld()->increaseScore(10);
        return;
    }
    if (isDropped) {
        Protester* p = (getWorld()->protesterInRadius(this, 3));
        if (p != nullptr) {
            die();
            p->getBribed();
        };
        Disappear(100);//if the protestor walks over the gold he takes it and gets bribed
    }
}

//Goodie
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY)
    :Pickupable(world, imageID, startX, startY) {}
void Goodie::doSomething(){
    if (!alive()) return;
    if (getWorld()->isPlayerInRadius(this, 3)) {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->add(getID());
        getWorld()->increaseScore(75);
        return;
    }//similair code to the previous two goodies
    Disappear(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

//Sonar
Sonar::Sonar(StudentWorld* world, int startX, int startY)
    :Goodie(world, TID_SONAR, startX, startY) {}

//WaterPool
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
    :Goodie(world, TID_WATER_POOL, startX, startY) {}


//Protester
Protester::Protester(StudentWorld* world, int imageID, int hP)
    : Man(world, imageID, 60, 60, left, hP), m_leave(false), m_tickSinceLastTurn(200), m_tickNoYell(15)
{
    randomNumToMove();
    ticksToWaitBetweenMoves = max(0, 3 - (int)getWorld()->getLevel() / 4);
}
void Protester::randomNumToMove()
{
    m_numToMove = rand() % 53 + 8;
}
void Protester::doSomething()//The protestors follow 9 steps
{
    if (!alive()) return;//First we check if the protestor is alive
    if (ticksToWaitBetweenMoves > 0) {
        ticksToWaitBetweenMoves--;//next we make sure the protestor is always moving
        return;
    }
    else {
        ticksToWaitBetweenMoves = max(0, 3 - (int)getWorld()->getLevel() / 4);
        m_tickSinceLastTurn++;
        m_tickNoYell++;
    }
    if (m_leave) {
        if (getX() == 60 && getX() == 60) {
            die();
            getWorld()->decProtester();
            return;//Third If the protestor gets to annoyed and dies he moves up to the top right corner
        }
        getWorld()->moveToExit(this);// move one square closer to exit
        return;
    }

    if (getWorld()->isPlayerInRadius(this, 4) && isFacingPlayer()) {
        if (m_tickNoYell > 15) {//Fourth if the protestor is in the radius of the player he screams at the player annoying him
            getWorld()->getPlayer()->Annoyed(2);
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            m_tickNoYell = 0;
            return;
        }
        return;
    }
    // only for hardCore protester
    if(getID()==TID_HARD_CORE_PROTESTER){
        int M =16 + int(getWorld()->getLevel());
        Direction s= getWorld()->senseSignalFromPlayer(this, M);
        if(s != none){
            moveInDirection(s);
            return;
        }
    }
    Direction d = directionToPlayer(); //The fifth step is protestor needs a line of sight to see the tunnelman
    if (d != none && straightPathToPlayer(d) && (!getWorld()->isPlayerInRadius(this, 4))) {
        setDirection(d);
        moveInDirection(d);
        m_numToMove = 0;
        return;
    }
    m_numToMove--;
    if (m_numToMove <= 0) {
        Direction k = none;
        k = randomDirection();
        while (true) {//If the protestor does not know where the tunnelman is then the protestor moves in random directions
            k = randomDirection();
            if (getWorld()->notBlocked(getX(),getY(), k)) break;
        }
        setDirection(k);
        randomNumToMove();
    }
    //if the protestor is at an intersection we check what a viable direction is then go that way
    else if (isAtIntersection() && m_tickSinceLastTurn > 200) {
        pickViableDirectionToTurn();
        m_tickSinceLastTurn = 0;
        randomNumToMove();
    }
    //we then move in that new direction until something blocks the movement
    moveInDirection(getDirection());
    if (!getWorld()->notBlocked(getX(),getY(),getDirection())) {
        m_numToMove = 0;
    }
}

void Protester::moveInDirection(Direction direction)
{
    switch (direction) {
    case left:
        if (getDirection() == left) {//If the protestor moves all the way to left he turns around
            if (getX() == 0) setDirection(right);
                moveTo(getX() - 1, getY());
        }
        else setDirection(left);
        break;
    case right://If the protestor moves all the way to right he turns around
        if (getDirection() == right) {
            if (getX() == 60) setDirection(left);
                moveTo(getX() + 1, getY());
        }
        else setDirection(right);
        break;
    case up:
        if (getDirection() == up) {//If the protestor moves all the way up he turns around
            if (getY() == 60) setDirection(down);
            moveTo(getX(), getY() + 1);
        }
        else setDirection(up);
        break;
    case down://If the protestor moves all the way down he turns around
        if (getDirection() == down) {
            if (getY() == 0) setDirection(up);
            moveTo(getX(), getY() - 1);
        }
        else setDirection(down);
        break;
    case none: return;
    }
}



GraphObject:: Direction Protester::directionToPlayer()
{//If the protestor has line of sight to the Tunnelman he goes in that direction
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();
    if (getY() == playerY && getX() == playerX)
    {
        return getDirection();
    }
    if (getX() == playerX) {
        if (getY() < playerY)
            return up;
        if (getY() > playerY)
            return down;
    }
    if (getY() == playerY) {
        if (getX() > playerX)
            return left;
        if (getY() < playerY)
            return right;
    }
    
    return none;   //he the protestor does not see the player he does nothing
}

bool Protester::straightPathToPlayer(Direction direction)
{//now to check if there is line of sight to the tunnelman we check if there is anyhting blocking the line of sight to the tunnelman such as earth or a boulder
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getX();
    switch (direction) {
    case left:
        for (int i = getX(); i >= playerX; i--) {
            if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
                return false;
        }
        return true;
        break;
    case right:
        for (int i = getX(); i <= playerX; i++) {
            if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
                return false;
        }
        return true;
        break;
    case up:
        for (int j = getY(); j <= playerY; j++) {
            if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
                return false;
        }
        return true;
        break;
    case down:
        for (int j = getY(); j >= playerY; j--) {
            if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
                return false;
        }
        return true;
        break;
    case none:
        return false;
    }
}
GraphObject::Direction Protester::randomDirection()
{
    int r;
    r = rand() % 4;
    switch (r) {
        case 0: return left;
        case 1: return  right;
        case 2: return up;
        case 3: return down;
    }//if the protestor doesnt know where to go he just moves in a random direction
    return none;
}
bool Protester::isAtIntersection()
{
    if (getDirection() == up || getDirection() == down) {
        return (getWorld()->notBlocked(getX(),getY(), left) || getWorld()->notBlocked(getX(),getY(), right));
    }
    else
        return (getWorld()->notBlocked(getX(),getY(), up) || getWorld()->notBlocked(getX(),getY(), down));
}
void Protester::pickViableDirectionToTurn()
{//we check if the right is viable to turn to, if it is we turn there f not then we randomize
    if (getDirection() == up || getDirection() == down) {
        if (!getWorld()->notBlocked(getX(),getY(), left)) setDirection(right);
        else if (!getWorld()->notBlocked(getX(),getY(), right)) setDirection(left);
        else {
            switch (rand() % 2) {
            case 0: setDirection(left);
            case 1: setDirection(right);
            }
        }
    }
    else {
        if (!getWorld()->notBlocked(getX(),getY(), up)) setDirection(down);
        else if (!getWorld()->notBlocked(getX(),getY(), down)) setDirection(up);
        else {
            switch (rand() % 2) {
            case 0: setDirection(up);
            case 1: setDirection(down);
            }
        }
    }

}
void Protester::Annoyed(int hP)
{//When the protestor gets hit we play a sound and take away hitpoints until he runs out of health then he gives up
    if(m_leave) return;
    decHP(hP);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    getStunned();
    if (getHP() <= 0) {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_leave = true;
        ticksToWaitBetweenMoves=0;
        if(hP==100) getWorld()->increaseScore(500);
        else if(getID()==TID_PROTESTER) getWorld()->increaseScore(100);
        else getWorld()->increaseScore(250);
    }
}
void Protester::getStunned()
{//The protestor pauses after geting stunned
    ticksToWaitBetweenMoves = max(50, 100 - (int)getWorld()->getLevel()*10);
}
void Protester::getBribed()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    if(getID()==TID_PROTESTER) {//Protestor gets bribed he decides to leave
        getWorld()->increaseScore(25);
        m_leave = true;
    }
    else {
        getWorld()->increaseScore(50);
        ticksToWaitBetweenMoves = max(50,100- int(getWorld()->getLevel())*10);
    }
}

bool Protester::isFacingPlayer()
{
    switch (getDirection()) {
        case left:
            return getWorld()->getPlayer()->getX() <= getX();
        case right:
            return getWorld()->getPlayer()->getX() >= getX();
        case up:
            return getWorld()->getPlayer()->getY() >= getY();
        case down:
            return getWorld()->getPlayer()->getY() <= getY();
        case none:
            return false;
    }
    return false;
}

//RegularProtester
RegularProtester::RegularProtester(StudentWorld* world)
    : Protester(world, TID_PROTESTER, 5) {}



//HardcoreProtester
HardcoreProtester::HardcoreProtester(StudentWorld* world)
    : Protester(world, TID_HARD_CORE_PROTESTER, 20) {}
