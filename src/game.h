#ifndef GAME_H_
#define GAME_H_

#include "physics.h"
#include "player.h"
#include "graphics.h"

enum GameScoreMode {
     SCORE_2_2_3_2,
     SCORE_3_5_8_6
};

class game
{
public:
    game();
    void run();
    void initGameObjects();
    void primaryGameLoop();
    void pollInput();
    void renderTextures();
    void putBallInBasket();
    void giveBallToPlayer(bplayer *);
    void checkScoreCondition();
    void updateAnimations();
    void checkPlayerPickupEvent(bplayer *);
    void executePlayerAI(bplayer *);
    void executePlayerLooseBallAI(bplayer *);
    void executePlayerDefenseAI(bplayer *);
    void executePlayerOffenseAI(bplayer *);
    void movePlayerLeft(bplayer *);
    void movePlayerRight(bplayer *);
    void jumpPlayer(bplayer *);
    void turnPlayer(bplayer *);
    void shootBall(bplayer *);
    void printStatusBar();
    void checkPlayerStationaryEvent(bplayer *);
    void checkForKeyMovementEvents();
    void checkForMouseEvents();
    void checkForQuitEvent();
    void updateCounters();
    void movePlayerTowardsBall(bplayer *,double);
    void movePlayerInCurrentDirection(bplayer *);
    void flipPlayerTowardsShotDirection(bplayer *, point);
    void setThreePointLineFlags(bplayer *);
    void checkGameEndCondition();
    void delayGame();
    void delay();
    void shotClockViolation();
    void executeAllPlayersAI();
    void updatePlayerAnimation(bplayer *);
    double getShotClockStartTime();
    bool checkPlayerCorneringEvent(bplayer *);
    bool playerTypeMatchesPossession(bplayer *);
    point getShotPoint(bplayer *);
    point getShotVelocity(bplayer *, point);
    bplayer *getPlayerFromControllerFlag();
    bplayer *getPlayerOnOffense();
private:
    object net1_obj;
    object net2_obj;
    object ball_obj;
    bplayer player2_obj;
    bplayer player1_obj;
    playerevent_state ignore_ball_state;
    playerevent_state score_flag;
    playerevent_state possession;
    playerevent_state last_possession;
    playerevent_state controlled_player;
    gfx_engine gfx;
    SDL_Event evt;
    bool quit_flag;
    bool game_ended;
    bool game_paused;
    bool timed_game;
    bool fast_sim_mode;
    Uint32 frames_per_second;
    Uint32 frame_start_timer;
    double time_elapsed;
    double shot_clock;
    int stuck_ball_counter;
    int game_period;
    int player1_scoreadd;
    int player2_scoreadd;
    GameScoreMode game_score_mode;
};

std::string int2String(int);
std::string double2String(double);
std::string getPeriodString(int);
std::string getTimeString(int,double);
std::string int2TimerString(int);

int randInt(int,int);
int getTimeLeft(int,double);
double randDouble(double,double);
bool rand1000(int);
bool rollDice(int);
bool playerApproachingEdge(bplayer *bp);
bool playerInInsideZone(bplayer *);
bool playerInRegTwoZone(bplayer *);
bool playerInLongTwoZone(bplayer *);
bool playerInThreeZone(bplayer *);
void incPlayerScore(bplayer *, bool, GameScoreMode);

#endif
