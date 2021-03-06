#include "StudentWorld.h"
#include "Actor.h"
#include "GraphObject.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}
StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir),m_isFirstTick(true), m_tickSinceLast(0), m_protestersAlive(0), m_player(nullptr), m_barrelsLeft(0)
{
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::~StudentWorld()
{
    cleanUp();
}
Tunnelman * StudentWorld::getPlayer()
{
    return m_player;
}
void StudentWorld::updateDisplayText()
{
    int level = getLevel();
    int lives = getLives();
    int health = m_player->getHP();
    int squirts = m_player->Water();
    int gold = m_player->getGold();
    int sonar = m_player->getSonar();
    int score = getScore();
    
    string s = displayText(score, level, lives, health, squirts, gold, sonar, m_barrelsLeft);
    setGameStatText(s);

}
string StudentWorld::displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels)
{//We format the display and call all the values
    stringstream s;

    s.fill('0');
    s << "Scr: ";
    s << setw(6) << score;

    s.fill(' ');
    s << " Lvl: ";
    s << setw(2) << level;

    s << " Lives: ";
    s << setw(1) << lives;

    s << "  Hlth: ";
    s << setw(3) << health * 10;
    s << '%';

    s << "  Wtr: ";
    s << setw(2) << squirts;

    s << "  Gld: ";
    s << setw(2) << gold;

    s << "  Sonar: ";
    s << setw(2) << sonar;

    s << "  Oil Left: ";
    s << setw(2) << barrels;

    return s.str();
}
bool StudentWorld::removeEarth(int x, int y) {
    bool clear = false;//we check if there is any value at earth if there is a value at that postition we delete it and clear the it
    for (int i = x; i < x+4; i++)
        for (int j = y; j < y+4; j++)
        {
            if (m_earth[i][j] != nullptr) {
                if(m_earth[i][j] -> getX() >= x && m_earth[i][j] -> getX() <= x+3 && m_earth[i][j] -> getY() >= y && m_earth[i][j] -> getY() <= y+3)
                delete m_earth[i][j];
                m_earth[i][j] = nullptr;
                clear = true;
            }
        }
    return clear;
}

void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}
void StudentWorld::addGoodies()
{//We add the goodies to random locations in the earth
    int x, y;
    int G = (int)getLevel() + 300;
    if (int(rand() % G) + 1 == 1) {
        if (int(rand() % 5) + 1 == 1) {
            addActor(new Sonar(this, 0, 60));
        }
        else {
            do {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            } while (isThereEarth(x, y));
            addActor(new WaterPool(this, x, y));//While the level is still going on we add more waterpools
        }
    }
}
void StudentWorld::addProtesters()
{//We always add regular protesters but randomize the chance if we get hardcore protestor
    int T = max(25, 200 - (int)getLevel());
    int P = fmin(15, 2 + getLevel() * 1.5);
    int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
    if (m_isFirstTick || (m_tickSinceLast > T && m_protestersAlive < P)) {
        if (rand() % 100 + 1 < probabilityOfHardcore)
            addActor(new HardcoreProtester(this));
        else addActor(new RegularProtester(this));
        m_tickSinceLast = 0;
        m_protestersAlive++;
        m_isFirstTick = false;
    }
    m_tickSinceLast++;
}
void StudentWorld::decBarrel()
{//We use this if the player picks up the barrel
    m_barrelsLeft--;
}
void StudentWorld::decProtester(){
    m_protestersAlive--;
}
bool StudentWorld::withInRadius(int x1, int y1, int x2, int y2, int radius)
{//We define what is in the radius and check if it is in the radius and return true if it is
    if ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) <= radius * radius)
        return true;
    else return false;
}
bool StudentWorld::ActorsWithInRadius(int x, int y, int radius) {
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()){//When we want to pick up an object or take damage we check if the actors are within radius
        if (withInRadius(x, y, (*it)->getX(),(*it)->getY(), radius))
            return true;
        it++;
    }
    return false;
}
void StudentWorld::addBoulderorGoldorBarrel(int num, char actor) {
    int x, y;
    for (int i = 0; i < num; i++) {
        do {
            x = rand() % 60 + 1;
            if (actor == 'B') y = rand() % 36 + 1 + 20;
            else    y = rand() % 56 + 1;
        } while (ActorsWithInRadius(x, y, 6) || (x > 26 && x < 34 && y>0));
        switch (actor) {
            case 'B':
                addActor(new Boulder(this, x, y));
                break;
            case 'G':
                addActor(new Gold(this, x, y,false,false));
                break;
            case 'L':
                addActor(new Barrel(this, x, y));
                m_barrelsLeft++;
                break;
        }
    }
}
bool StudentWorld::isAboveEarth(int x, int y)
{//We check if the object if above earth
    for (int i = x; i < x + 4; i++)
    {
        if (m_earth[i][y] != nullptr)
            return true;
    }
    return false;
}
bool StudentWorld::isThereEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++) {

        for (int j = y; j <y + 4; j++) {

            if (m_earth[i][j] != nullptr)
                return true;
        }//When a protestor wants to move or the player wants to shoot we must check if there is anything blocking them such as the earth
    }
    return false;
}
bool StudentWorld::isThereBoulder(int x, int y,int radius)
{
    vector<Actor*>::iterator it;//Same with "is there earth" we use this check if there is a boulder in this spot
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getID() == TID_BOULDER && withInRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
            return true;
    }
    return false;
}
bool StudentWorld::notBlocked(int x, int y, GraphObject::Direction direction)
{//To check if there is nothing blocking a direction we must make sure its not at 0 or 60 on the x and y axis and if there is no boulder or earth
    switch (direction) {
    case GraphObject::left:
        return (x!=0 && !isThereEarth(x - 1, y) && !isThereBoulder(x, y));
    case GraphObject::right:
        return (x!=60&& !isThereEarth(x + 1, y) && !isThereBoulder(x + 1, y));
    case GraphObject::up:
        return (y!=60&& !isThereEarth(x, y + 1) && !isThereBoulder(x, y + 1));
    case GraphObject::down:
        return (y!=0&& !isThereEarth(x, y - 1) && !isThereBoulder(x, y - 1));
    case GraphObject::none:
            return false;
    }
    return false;
}
bool StudentWorld::isPlayerInRadius(Actor *actor, int radius)
{
    return withInRadius(actor->getX(), actor->getY(), m_player->getX(), m_player->getY(), radius);
}
Protester* StudentWorld::protesterInRadius(Actor * actor, int radius)
{//We call this funtion when we want to see if the protestor is touching either the tunnelmam or the gold
    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
            if (withInRadius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius))
                return dynamic_cast<Protester*> (*it);
    }
    return nullptr;
}
void StudentWorld::moveToExit(Protester * pr)
{//When the protestor gets annoyed he will move to the exit
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            m_maze[i][j]=0;
        }
    }
    int a =pr->getX();
    int b =pr->getY();
    queue<Grid> q;
    q.push(Grid(60,60));
    m_maze[60][60]=1;
    while (!q.empty()) {
        Grid c = q.front();
        q.pop();
        int x=c.x;
        int y=c.y;

        //left
        if(notBlocked(x,y, GraphObject::left)&& m_maze[x-1][y]==0){
            q.push(Grid(x-1,y));
            m_maze[x-1][y] =m_maze[x][y]+1;
        }
        //right
        if(notBlocked(x,y, GraphObject::right)&& m_maze[x+1][y]==0){
            q.push(Grid(x+1,y));
            m_maze[x+1][y] =m_maze[x][y]+1;
        }
        //up
        if(notBlocked(x,y, GraphObject::up)&& m_maze[x][y+1]==0){
            q.push(Grid(x,y+1));
            m_maze[x][y+1] =m_maze[x][y]+1;
        }
        // down
        if(notBlocked(x,y, GraphObject::down)&& m_maze[x][y-1]==0){
            q.push(Grid(x,y-1));
            m_maze[x][y-1] =m_maze[x][y]+1;
        }
    }
    if(notBlocked(a,b, GraphObject::left)&& m_maze[a-1][b]<m_maze[a][b])
        pr->moveInDirection(GraphObject::left);
    if(notBlocked(a,b, GraphObject::right)&& m_maze[a+1][b]<m_maze[a][b])
        pr->moveInDirection(GraphObject::right);
    if(notBlocked(a,b, GraphObject::up)&& m_maze[a][b+1]<m_maze[a][b])
        pr->moveInDirection(GraphObject::up);
    if(notBlocked(a,b, GraphObject::down)&&m_maze[a][b-1]<m_maze[a][b])
        pr->moveInDirection(GraphObject::down);
    return ;
}

void StudentWorld::sonarSonared(int x, int y, int radius)
{
    int a, b;
    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
        {
            a = (*it)->getX(); b = (*it)->getY();
            if ((x - a)*(x - a) + (y - b)*(y - b) <= radius * radius)
                (*it)->setVisible(true);//If the tunnelman uses the sonar in teh radius the barrel or gold becomes visable
        }
    }
}
GraphObject::Direction StudentWorld::senseSignalFromPlayer(Protester * pr, int M){
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            m_maze[i][j]=0;
        }
    }
    int a =pr->getX();
    int b =pr->getY();
    queue<Grid> q;
    q.push(Grid(getPlayer()->getX(),getPlayer()->getY()));
    m_maze[getPlayer()->getX()][getPlayer()->getY()]=1;
    while (!q.empty()) {
        Grid c = q.front();
        q.pop();
        int x=c.x;
        int y=c.y;
        
        //left
        if(notBlocked(x,y, GraphObject::left)&& m_maze[x-1][y]==0){
            q.push(Grid(x-1,y));
            m_maze[x-1][y] =m_maze[x][y]+1;
        }
        //right
        if(notBlocked(x,y, GraphObject::right)&& m_maze[x+1][y]==0){
            q.push(Grid(x+1,y));
            m_maze[x+1][y] =m_maze[x][y]+1;
        }
        //up
        if(notBlocked(x,y, GraphObject::up)&& m_maze[x][y+1]==0){
            q.push(Grid(x,y+1));
            m_maze[x][y+1] =m_maze[x][y]+1;
        }
        // down
        if(notBlocked(x,y, GraphObject::down)&& m_maze[x][y-1]==0){
            q.push(Grid(x,y-1));
            m_maze[x][y-1] =m_maze[x][y]+1;
        }
    }
    if(m_maze[a][b] <= M+1){
        if(notBlocked(a,b, GraphObject::left)&& m_maze[a-1][b]<m_maze[a][b])
            return GraphObject::left;
        if(notBlocked(a,b, GraphObject::right)&& m_maze[a+1][b]<m_maze[a][b])
            return GraphObject::right;
        if(notBlocked(a,b, GraphObject::up)&& m_maze[a][b+1]<m_maze[a][b])
            return GraphObject::up;
        if(notBlocked(a,b, GraphObject::down)&&m_maze[a][b-1]<m_maze[a][b])
            return GraphObject::down;
    }
    return GraphObject::none;
}

int StudentWorld::init()
{
    //reset variables
    m_barrelsLeft = 0;
    m_isFirstTick = true;
    m_tickSinceLast = 0;
    m_protestersAlive = 0;
    // create oil field
    for (int x = 0; x< 64 ; x++)
    {
        for (int y = 0;  y<60; y++)
        {
            if(x<30||x>33||y<4)
            m_earth[x][y] = new Earth(this,x,y);
        }
    }
    m_player = new Tunnelman(this);  // create tunnelman
    // create boulders, gold and barrel of oil
    int B = min( (int)getLevel()/2 + 2, 9);
    int G = max((int)getLevel()/2, 2);
    int L = min( 2 + (int)getLevel(), 21);
    addBoulderorGoldorBarrel(B, 'B');
    addBoulderorGoldorBarrel(L, 'L');
    addBoulderorGoldorBarrel(G, 'G');

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    updateDisplayText();

    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it!=m_actors.end(); it++) {
        if ((*it)->alive()) {
            (*it)->doSomething();
        }
        if (!m_player->alive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_barrelsLeft == 0) {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    m_player->doSomething();

    addGoodies();
    addProtesters();

    // clean dead actors
    for (it = m_actors.begin(); it != m_actors.end();) {
        if (!(*it)->alive()) {
            delete *it;
            it = m_actors.erase(it);
        }
        else it++;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 60; y++)
        {
            delete m_earth[x][y];
        }
    }
    vector<Actor*>::iterator it;
    for (it = m_actors.begin();it != m_actors.end();it++)
    {
        delete *it;
    }
    m_actors.clear();
    delete m_player;

}
