#include "game.h"

game::game()
{
    game_score_mode = SCORE_2_2_3_2;
    quit_flag = game_ended = game_paused = false;
    last_possession = possession = score_flag = ignore_ball_state = PEVENT_NONE;
    time_elapsed = 0.0;
    stuck_ball_counter = 0;
    controlled_player = PEVENT_NONE;
    timed_game = true;
    fast_sim_mode = false;
    game_period = 0;
    frames_per_second = 70;
    frame_start_timer = frames_per_second*2;
    shot_clock = 10.0;
    player1_scoreadd = player2_scoreadd = 2;
}

void game::initGameObjects()
{
    ball_obj = object(OBJ_BASKETBALL,point((double)WINDOW_WIDTH/2.0,0.0),texture_size[(int)OBJ_BASKETBALL],0);
    net1_obj = object(OBJ_NET,point(NET1_LOC_X,NET1_LOC_Y),texture_size[(int)OBJ_NET],4);
    net2_obj = object(OBJ_NET,point(NET2_LOC_X,NET2_LOC_Y),texture_size[(int)OBJ_NET],4);
    player1_obj = bplayer(OBJ_PLAYER1,point(PLAYER1_START_X,PLAYER1_START_Y),texture_size[(int)OBJ_PLAYER1],0);
    player2_obj = bplayer(OBJ_PLAYER2,point(PLAYER2_START_X,PLAYER2_START_Y),texture_size[(int)OBJ_PLAYER2],0);
    initBallPhysics(&ball_obj);
    initNetPhysics(&net1_obj,0);
    initNetPhysics(&net2_obj,1);
    player2_obj.getObject()->freezeXMotion();
    player1_obj.getObject()->freezeXMotion();
    player2_obj.getObject()->setOrientation(SDL_FLIP_HORIZONTAL);
}

void game::run()
{
    if(gfx.initSDL())
    {
        initGameObjects();
        primaryGameLoop();
    }
    else
    {
        std::cout << "Failed to initialize... press enter to terminate.";
        std::cin.get();
    }
}

void game::movePlayerTowardsBall(bplayer *bp, double threshold)
{
    if (ball_obj.getLoc().x() - bp->getLoc().x() > threshold)
        movePlayerRight(bp);
    else if (ball_obj.getLoc().x() - bp->getLoc().x() < -threshold)
        movePlayerLeft(bp);
}

void game::movePlayerInCurrentDirection(bplayer *bp)
{
    if (bp->getMoveState() == MOVEST_RIGHT)
        movePlayerRight(bp);
    else if (bp->getMoveState() == MOVEST_LEFT)
        movePlayerLeft(bp);
}

void game::turnPlayer(bplayer *bp)
{
    if (bp->getMoveState() == MOVEST_RIGHT)
        bp->setMoveState(MOVEST_LEFT);
    else if (bp->getMoveState() == MOVEST_LEFT)
        bp->setMoveState(MOVEST_RIGHT);
}

void game::executePlayerLooseBallAI(bplayer *bp)
{
    movePlayerTowardsBall(bp,50.0);
    if (rand1000(bp->getPlayerStats()->jump_chance_loose) || ball_obj.getVelocity().y() < 0.0)
        jumpPlayer(bp);
}

void game::executePlayerDefenseAI(bplayer *bp)
{
    if (rand1000(bp->getPlayerStats()->turn_chance_defense) && pointDist(player1_obj.getLoc(),player2_obj.getLoc()) < 150.0) {
        turnPlayer(bp);
        movePlayerInCurrentDirection(bp);
    }
    else {
        movePlayerTowardsBall(bp,100.0);
    }

    if (rand1000(bp->getPlayerStats()->jump_chance_defense) && pointDist(player1_obj.getLoc(),player2_obj.getLoc()) < 100.0)
        jumpPlayer(bp);
}

void game::executePlayerOffenseAI(bplayer *bp)
{
    int time_left = getTimeLeft(game_period,time_elapsed);

    if (bp->getObject()->getType() == OBJ_PLAYER2 && bp->getLoc().x() + bp->getArea().x() >= 880.0 && bp->getMoveState() == MOVEST_RIGHT)
        turnPlayer(bp);
    else if ((bp->getObject()->getType() == OBJ_PLAYER1 && bp->getLoc().x() < 120.0 && bp->getMoveState() == MOVEST_LEFT))
        turnPlayer(bp);
    else if (rand1000(bp->getPlayerStats()->turn_chance_offense))
        turnPlayer(bp);

    movePlayerInCurrentDirection(bp);
    if (rand1000(bp->getPlayerStats()->jump_chance_offense))
        jumpPlayer(bp);
    if (rand1000(bp->getPlayerStats()->inside_chance) && playerInInsideZone(bp))
    {
        shootBall(bp);
        return;
    }
    if (rand1000(bp->getPlayerStats()->regtwo_chance) && playerInRegTwoZone(bp))
    {
        shootBall(bp);
        return;
    }
    if (rand1000(bp->getPlayerStats()->longtwo_chance) && playerInLongTwoZone(bp))
    {
        shootBall(bp);
        return;
    }
    if (rand1000(bp->getPlayerStats()->three_chance) && playerInThreeZone(bp))
    {
        shootBall(bp);
        return;
    }
    if (timed_game && (time_left <= 1 || shot_clock <= 1.0))
    {
        shootBall(bp);
        return;
    }
}

void game::executePlayerAI(bplayer *bp)
{
    if (possession == PEVENT_NONE)
    {
        executePlayerLooseBallAI(bp);
    }
    else if (!playerTypeMatchesPossession(bp))
    {
        executePlayerDefenseAI(bp);
    }
    else
    {
        executePlayerOffenseAI(bp);
    }
}

void game::primaryGameLoop()
{
    while (!quit_flag)
    {
        frame_start_timer = SDL_GetTicks();
        pollInput();
        if (!game_paused)
        {
            executeAllPlayersAI();
            applyPlayerPhysics(player1_obj.getObject());
            applyPlayerPhysics(player2_obj.getObject());
            //checkPlayerPlayerCollision(player1_obj.getObject(), player2_obj.getObject());
            checkPlayerStationaryEvent(&player1_obj);
            checkPlayerStationaryEvent(&player2_obj);
            checkPlayerPickupEvent(&player1_obj);
            checkPlayerPickupEvent(&player2_obj);
            applyBallPhysics(&ball_obj);
            checkBallNetCollision(&ball_obj,&net1_obj);
            checkBallNetCollision(&ball_obj,&net2_obj);
            checkScoreCondition();
            renderTextures();
            updateAnimations();
            updateCounters();
        }
        delayGame();
    }
}

void game::executeAllPlayersAI()
{
    if (!game_ended && time_elapsed > 3.0)
    {
        if (controlled_player != PEVENT_PLAYER1)
            executePlayerAI(&player1_obj);
        if (controlled_player != PEVENT_PLAYER2)
            executePlayerAI(&player2_obj);
    }
}

void game::delayGame()
{
    if (!fast_sim_mode)
    {
        delay();
    }
    else
    {
        if (game_ended)
        {
            renderTextures();
            delay();
        }
    }
}

void game::delay()
{
    // Keep game running at consistent 70 fps.
    if((1000/frames_per_second)>(SDL_GetTicks()-frame_start_timer))
    {
        SDL_Delay((1000/frames_per_second)-(SDL_GetTicks()-frame_start_timer));
    }
}

double game::getShotClockStartTime()
{
    int time_left = getTimeLeft(game_period,time_elapsed);

    if (time_left > 60)
        return 10.0;
    else if (time_left > 50)
        return 9.0;
    else if (time_left > 40)
        return 8.0;
    else if (time_left > 30)
        return 7.0;
    else if (time_left > 15)
        return 6.0;

    return 5.0;
}

void game::shotClockViolation()
{
    if (possession == PEVENT_PLAYER1)
    {
        ball_obj.setPrevLoc(point(SCORE2_LOC_X-BALL_RADIUS,SCORE2_LOC_Y+BALL_RADIUS+100.0));
        ignore_ball_state = PEVENT_PLAYER1;
    }
    else
    {
        ball_obj.setPrevLoc(point(SCORE1_LOC_X-BALL_RADIUS,SCORE1_LOC_Y+BALL_RADIUS+100.0));
        ignore_ball_state = PEVENT_PLAYER2;
    }
    ball_obj.setLoc(ball_obj.getPrevLoc());
    ball_obj.freezeXMotion();
    ball_obj.setVelocity(point(0.0,0.0));
    ball_obj.setYMotion(true);
    possession = PEVENT_NONE;
    shot_clock = getShotClockStartTime();
}

void game::updateCounters()
{
    if (ignore_ball_state == PEVENT_NONE || last_possession == PEVENT_NONE)
        time_elapsed += 0.015;

    if (possession == PEVENT_NONE)
        stuck_ball_counter++;
    else if (!game_ended)
        shot_clock -= 0.015;

    if (shot_clock <= 0.0 && TOTAL_TIME_REMAINING(game_period, time_elapsed) > shot_clock)
    {
        shotClockViolation();
    }
    if (stuck_ball_counter == 800)
    {
        ignore_ball_state = PEVENT_NONE;
        ball_obj.activateMotion();
        ball_obj.freezeXMotion();
        ball_obj.setLoc(point((double)WINDOW_WIDTH/2.0,0.0));
        ball_obj.setVelocity(point(0.0,0.0));
    }
    player2_obj.incFatigue(5);
    player1_obj.incFatigue(5);
    checkGameEndCondition();
}

void game::updateAnimations()
{
    if (net1_obj.isAnimating())
    {
        if ((net1_obj.getLifetime()+1) % NET_ANIMATION_TICK_DELAY == 0)
            net1_obj.setCurrentFrame(net1_obj.getCurrentFrame()+1);
        net1_obj.incLifetime();
    }
    if (net2_obj.isAnimating())
    {
        if ((net2_obj.getLifetime()+1) % NET_ANIMATION_TICK_DELAY == 0)
            net2_obj.setCurrentFrame(net2_obj.getCurrentFrame()+1);
        net2_obj.incLifetime();
    }

    //updatePlayerAnimation(&player1_obj);
    //updatePlayerAnimation(&player2_obj);
}

void game::updatePlayerAnimation(bplayer *bp)
{
    if (std::abs(bp->getObject()->getVelocity().x()) > 0.0)
        bp->getObject()->setAnimationState(true);
    else
    {
        bp->getObject()->setAnimationState(false);
        bp->getObject()->setCurrentFrame(0);
    }

    if (!objAtLowerBoundary(bp->getObject()))
        bp->getObject()->setAnimationState(false);

    if (bp->getObject()->isAnimating())
    {
        if ((bp->getObject()->getLifetime()+1) % PLAYERMOVE_ANIMATION_TICK_DELAY == 0)
            bp->getObject()->setCurrentFrame(bp->getObject()->getCurrentFrame()+1);
        bp->getObject()->setAnimationState(true);
        bp->getObject()->incLifetime();
    }
}

// move to bottom
void incPlayerScore(bplayer * bp, bool wrong_basket, GameScoreMode gsm) {
     if (wrong_basket) {
         bp->incScore(gsm == SCORE_2_2_3_2 ? 2 : 6);
     }
     else {
         switch(bp->getShotZoneType()) {
                case(SHOTZONE_CLOSE): {
                     bp->incScore(gsm == SCORE_2_2_3_2 ? 2 : 3);
                     break;
                }
                case(SHOTZONE_MIDDLE): {
                     bp->incScore(gsm == SCORE_2_2_3_2 ? 2 : 5);
                     break;
                }
                default: {
                     bp->incScore(gsm == SCORE_2_2_3_2 ? 3 : 8);
                     break;
                }
         }
     }
}

void game::checkScoreCondition()
{
    if (checkBallAtScorePosition(&ball_obj,point(SCORE1_LOC_X-SCORE_REGION_HALF_LENGTH,SCORE1_LOC_Y-SCORE_REGION_HALF_HEIGHT),
                                           point(SCORE1_LOC_X+SCORE_REGION_HALF_LENGTH,SCORE1_LOC_Y+SCORE_REGION_HALF_HEIGHT)))
    {
        if (score_flag == PEVENT_NONE)
        {
            score_flag = ignore_ball_state = PEVENT_PLAYER1;
            putBallInBasket();
            net1_obj.setAnimationState(true);
            incPlayerScore(&player1_obj, last_possession == PEVENT_PLAYER2, game_score_mode);
        }
    }
    else if (checkBallAtScorePosition(&ball_obj,point(SCORE2_LOC_X-SCORE_REGION_HALF_LENGTH,SCORE2_LOC_Y-SCORE_REGION_HALF_HEIGHT),
                                                point(SCORE2_LOC_X+SCORE_REGION_HALF_LENGTH,SCORE2_LOC_Y+SCORE_REGION_HALF_HEIGHT)))
    {
        if (score_flag == PEVENT_NONE)
        {
            score_flag = ignore_ball_state = PEVENT_PLAYER2;
            putBallInBasket();
            net2_obj.setAnimationState(true);
            incPlayerScore(&player2_obj, last_possession == PEVENT_PLAYER1, game_score_mode);
        }
    }
    else
    {
        score_flag = PEVENT_NONE;
    }
}

void game::pollInput()
{
    const Uint8 * key_states = SDL_GetKeyboardState( NULL );

    while( SDL_PollEvent( &evt ) != 0)
    {
        checkForQuitEvent();
        if (controlled_player != PEVENT_NONE && !game_ended && time_elapsed > 3.0 && !game_paused)
        {
            checkForMouseEvents();
        }
        if (evt.type == SDL_KEYDOWN)
        if (evt.key.keysym.sym == SDLK_PAUSE ||
            evt.key.keysym.sym == SDLK_p)
        {
            game_paused = !game_paused;
        }
    }
    if (controlled_player != PEVENT_NONE && !game_ended && time_elapsed > 3.0 && !game_paused)
    {
        checkForKeyMovementEvents();
    }
    if (key_states[SDL_SCANCODE_Q])
    {
        quit_flag = true;
    }
}

void game::checkForQuitEvent()
{
    if( evt.type == SDL_QUIT)
    {
       quit_flag = true;
    }
}

void game::checkForKeyMovementEvents()
{
    const Uint8 * key_states = SDL_GetKeyboardState( NULL );

    if (key_states[SDL_SCANCODE_A])
    {
       movePlayerLeft(getPlayerFromControllerFlag());
    }
    else if (key_states[SDL_SCANCODE_D])
    {
       movePlayerRight(getPlayerFromControllerFlag());
    }
    if (key_states[SDL_SCANCODE_SPACE])
    {
       jumpPlayer(getPlayerFromControllerFlag());
    }
}

void game::checkForMouseEvents()
{
    if (evt.type == SDL_MOUSEBUTTONDOWN)
    {
        if (evt.button.button == SDL_BUTTON_LEFT)
        {
            shootBall(getPlayerFromControllerFlag());
        }
    }
}

bplayer *game::getPlayerFromControllerFlag()
{
    if (controlled_player == PEVENT_PLAYER1)
        return &player1_obj;
    return &player2_obj;
}

void game::movePlayerLeft(bplayer *bp)
{
   bp->setMoveState(MOVEST_LEFT);
   //if (!objAtLowerBoundary(bp->getObject()) || bp->getObject()->collidingWithPlayer())
   //    return;
   bp->incFatigue(-5);
   bp->getObject()->setVelocity(point(-1.0*((double)bp->getFatigue()/95.0),bp->getObject()->getVelocity().y()));
   bp->getObject()->setOrientation(SDL_FLIP_NONE);
}

void game::movePlayerRight(bplayer *bp)
{
   bp->setMoveState(MOVEST_RIGHT);
   //if (!objAtLowerBoundary(bp->getObject()) || bp->getObject()->collidingWithPlayer())
   //    return;
   bp->incFatigue(-5);
   bp->getObject()->setVelocity(point(((double)bp->getFatigue()/95.0),bp->getObject()->getVelocity().y()));
   bp->getObject()->setOrientation(SDL_FLIP_HORIZONTAL);
}

void game::jumpPlayer(bplayer *bp)
{
   if (objAtLowerBoundary(bp->getObject()))
   {
       bp->incFatigue(-50);
       bp->getObject()->setVelocity(point(bp->getObject()->getVelocity().x(),-1.0*((double)bp->getFatigue()/105.0)));
       bp->getObject()->setYMotion(true);
   }
}

point game::getShotPoint(bplayer *bp)
{
    point shot_point;
    point ball_center = point(ball_obj.getLoc().x() + BALL_RADIUS,ball_obj.getLoc().y() + BALL_RADIUS);
    if (bp->getObject()->getType() == OBJ_PLAYER1)
    {
        if (controlled_player == PEVENT_PLAYER1)
            shot_point = point((double)evt.button.x,(double)evt.button.y);
        else
            shot_point = point((((net1_obj.getLoc().x()+net1_obj.getArea().x())+ball_center.x())/2.0)-net1_obj.getArea().x()+25.0,std::min(randDouble(15.0,35.0),ball_center.x()*0.6) + randDouble(1.0,3.0));
    }
    else
    {
        if (controlled_player == PEVENT_PLAYER2)
            shot_point = point((double)evt.button.x,(double)evt.button.y);
        else
            shot_point = point(((net2_obj.getLoc().x()+ball_center.x())/2.0)+net1_obj.getArea().x()-25.0,std::min(randDouble(15.0,35.0),ball_center.x()*0.6) + randDouble(1.0,3.0));
    }
    return shot_point;
}

point game::getShotVelocity(bplayer *bp, point shot_point)
{
    point shot_velocity;
    point ball_center = point(ball_obj.getLoc().x() + BALL_RADIUS,ball_obj.getLoc().y() + BALL_RADIUS);
    point delta = point(shot_point.x()-ball_center.x(),shot_point.y()-ball_center.y());
    shot_velocity = point(delta.x()*0.022 + randDouble(-0.25,0.25),-1.0*sqrt(std::abs(delta.y())*0.6)+ randDouble(-0.25,0.25));
    return shot_velocity;
}

void game::flipPlayerTowardsShotDirection(bplayer *bp, point shot_velocity)
{
    if (shot_velocity.x() > 0.0)
    {
       bp->getObject()->setOrientation(SDL_FLIP_HORIZONTAL);
    }
    if (shot_velocity.x() < 0.0)
    {
       bp->getObject()->setOrientation(SDL_FLIP_NONE);
    }
}

void game::setThreePointLineFlags(bplayer *bp)
{
    if (bp->getLoc().x() >= THREE_POINT_LINE_LOC1)
    {
       player1_obj.setShotZoneType(SHOTZONE_FAR);
       player2_obj.setShotZoneType(SHOTZONE_CLOSE);
    }
    else if (bp->getLoc().x() < THREE_POINT_LINE_LOC2 - bp->getArea().x())
    {
       player1_obj.setShotZoneType(SHOTZONE_CLOSE);
       player2_obj.setShotZoneType(SHOTZONE_FAR);
    }
    else
    {
       player1_obj.setShotZoneType(SHOTZONE_MIDDLE);
       player2_obj.setShotZoneType(SHOTZONE_MIDDLE);
    }
}

void game::shootBall(bplayer *bp)
{
    point shot_velocity;
    if (possession != PEVENT_NONE)
    {
        player1_scoreadd = (int)std::max(2,(int)player1_obj.getLoc().x() - 63);
        player2_scoreadd = (int)std::max(2,904 - (int)player2_obj.getLoc().x());
        shot_velocity = getShotVelocity(bp,getShotPoint(bp));
        bp->setShootFlag(true);
        possession = PEVENT_NONE;
        flipPlayerTowardsShotDirection(bp,shot_velocity);
        setThreePointLineFlags(bp);
        ball_obj.setVelocity(shot_velocity);
        ball_obj.activateMotion();
    }
}

bool game::checkPlayerCorneringEvent(bplayer* bp)
{
    if (bp->getObject()->getType() == OBJ_PLAYER1 && bp->getLoc().x() <= 100.0 && bp->getMoveState() != MOVEST_RIGHT)
        return true;
    if (bp->getObject()->getType() == OBJ_PLAYER2 && bp->getLoc().x() >= 900.0 - bp->getArea().x() && bp->getMoveState() != MOVEST_LEFT)
        return true;
    return false;
}

void game::putBallInBasket()
{
    if (score_flag == PEVENT_PLAYER1)
        ball_obj.setPrevLoc(point(SCORE1_LOC_X-BALL_RADIUS,SCORE1_LOC_Y+BALL_RADIUS));
    else
        ball_obj.setPrevLoc(point(SCORE2_LOC_X-BALL_RADIUS,SCORE2_LOC_Y+BALL_RADIUS));
    ball_obj.setLoc(ball_obj.getPrevLoc());
    ball_obj.freezeXMotion();
    ball_obj.setVelocity(point(0.0,0.0));
    ball_obj.setYMotion(true);
}

void game::giveBallToPlayer(bplayer *bp)
{
    double hold_offset = (bp->getObject()->getOrientation() == SDL_FLIP_NONE ? -PLAYER1_HOLD_POSITION_OFFSET : PLAYER1_HOLD_POSITION_OFFSET);
    ball_obj.setPrevLoc(point(bp->getLoc().x()+(bp->getArea().x()/2.0)-BALL_RADIUS+hold_offset,
                              bp->getLoc().y()+(bp->getArea().y()/2.0)-BALL_RADIUS));
    ball_obj.setLoc(ball_obj.getPrevLoc());
    ball_obj.freezeXMotion();
    ball_obj.freezeYMotion();
}

void game::renderTextures()
{
    if (fast_sim_mode && !game_ended)
        return;
    gfx.clearScreen();
    gfx.addGFXObject(OBJ_BACKGROUND,point(0.0,0.0),texture_size[(int)OBJ_BACKGROUND],0,0.0,SDL_FLIP_NONE);
    printStatusBar();

    if (last_possession == PEVENT_PLAYER2)
    {
        gfx.addGFXObject(player1_obj.getObject()->getType(),player1_obj.getLoc(),player1_obj.getArea(),0,0.0,player1_obj.getObject()->getOrientation());
        gfx.addGFXObject(player2_obj.getObject()->getType(),player2_obj.getLoc(),player2_obj.getArea(),0,0.0,player2_obj.getObject()->getOrientation());
    }
    else
    {
        gfx.addGFXObject(player2_obj.getObject()->getType(),player2_obj.getLoc(),player2_obj.getArea(),0,0.0,player2_obj.getObject()->getOrientation());
        gfx.addGFXObject(player1_obj.getObject()->getType(),player1_obj.getLoc(),player1_obj.getArea(),0,0.0,player1_obj.getObject()->getOrientation());
    }

    gfx.addGFXObject(ball_obj.getType(),ball_obj.getLoc(),ball_obj.getArea(),0,ball_obj.getAngle(),SDL_FLIP_NONE);
    gfx.addGFXObject(net1_obj.getType(),net1_obj.getLoc(),net1_obj.getArea(),net1_obj.getCurrentFrame(),0.0,SDL_FLIP_NONE);
    gfx.addGFXObject(net2_obj.getType(),net2_obj.getLoc(),net2_obj.getArea(),net2_obj.getCurrentFrame(),0.0,SDL_FLIP_HORIZONTAL);
    //gfx.drawRectangle(color_white,point(SCORE2_LOC_X-SCORE_REGION_HALF_LENGTH,SCORE2_LOC_Y-SCORE_REGION_HALF_HEIGHT),point(29,17));
    gfx.updateScreen();
}

void game::checkPlayerPickupEvent(bplayer *bp)
{
    point ball_center = addPoints(ball_obj.getLoc(),ball_obj.getHitCircle(0).loc);
    if (possession == PEVENT_NONE)
    {
        if (inRange(ball_center,point(bp->getLoc().x()-BALL_RADIUS,bp->getLoc().y() - BALL_RADIUS),
                                point(bp->getLoc().x()+bp->getArea().x()+BALL_RADIUS,bp->getLoc().y()+bp->getArea().y()+BALL_RADIUS)))
        {
            if (!bp->getShootFlag())
            {
                if (bp->getObject()->getType() == OBJ_PLAYER1)
                {
                    if (ignore_ball_state != PEVENT_PLAYER1)
                    {
                        last_possession = possession = PEVENT_PLAYER1;
                        ignore_ball_state = PEVENT_NONE;
                        shot_clock = getShotClockStartTime();
                        stuck_ball_counter = 0;
                    }
                }
                else
                {
                    if (ignore_ball_state != PEVENT_PLAYER2)
                    {
                        last_possession = possession = PEVENT_PLAYER2;
                        ignore_ball_state = PEVENT_NONE;
                        shot_clock = getShotClockStartTime();
                        stuck_ball_counter = 0;
                    }
                }
            }
        }
        else
        {
            bp->setShootFlag(false);
        }
    }
    if ((possession == PEVENT_PLAYER1 && bp->getObject()->getType() == OBJ_PLAYER1) ||
        (possession == PEVENT_PLAYER2 && bp->getObject()->getType() == OBJ_PLAYER2))
    {
        giveBallToPlayer(bp);
    }
}

void game::printStatusBar()
{
    gfx.addBitmapString(color_red,"R",point(436.0, 42.0));
    gfx.addBitmapString(color_yellow,"Y",point(436.0, 82.0));
    gfx.addBitmapString(color_light_gray,int2String(player1_obj.getScore()),point(484.0,42.0));
    gfx.addBitmapString(color_light_gray,int2String(player2_obj.getScore()),point(484.0,82.0));
    if (game_ended)
    {
        if (player1_obj.getScore() > player2_obj.getScore())
            gfx.addBitmapString(color_white,"Red Guy wins!",point((double)WINDOW_WIDTH/2.0-10*TILE_WIDTH,170.0));
        else
            gfx.addBitmapString(color_white,"Yellow Guy wins!",point((double)WINDOW_WIDTH/2.0-10*TILE_WIDTH,170.0));
    }
    if (time_elapsed < 3.0)
    {
        gfx.addBitmapString(color_white,"Starting in " + int2String((int)(3.0 - time_elapsed)+1.0), point(320.0,135.0));
    }

    if (!game_ended && timed_game)
    {
        if (possession == PEVENT_PLAYER1) {
            if (TOTAL_TIME_REMAINING(game_period, time_elapsed) > shot_clock)
                gfx.addBitmapString(color_red,int2String((int)std::min(10,(int)shot_clock + 1)),point(850,50));
            else
                gfx.addBitmapString(color_red,"OFF",point(850,50));
        }
        if (possession == PEVENT_PLAYER2) {
            if (TOTAL_TIME_REMAINING(game_period, time_elapsed) > shot_clock)
                gfx.addBitmapString(color_yellow,int2String((int)std::min(10,(int)shot_clock + 1)),point(100,50));
            else
                gfx.addBitmapString(color_yellow,"OFF",point(100,50));
        }
    }

    if (timed_game && time_elapsed >= 3.0)
    {
        gfx.addBitmapString(color_white,getPeriodString(game_period) + getTimeString(game_period,time_elapsed),point(((double)WINDOW_WIDTH/2.0)-3*TILE_WIDTH,135.0));
    }
}

void game::checkGameEndCondition()
{
    if (timed_game)
    {
        if (!ball_obj.getXMotionFlag() && !ball_obj.getYMotionFlag())
        {
            if (TOTAL_TIME_REMAINING(game_period, time_elapsed) <= 0)
            {
                if (player1_obj.getScore() == player2_obj.getScore())
                {
                    game_period++;
                }
                else
                {
                    game_ended = true;
                }
            }
        }
    }
    else
    {
        if (player1_obj.getScore() >= END_SCORE || player2_obj.getScore() >= END_SCORE)
        {
            game_ended = true;
        }
    }
}

void game::checkPlayerStationaryEvent(bplayer *bp)
{
    if(bp->getObject()->collidingWithPlayer())
       bp->incFatigue(-20*(int)(std::abs(bp->getObject()->getVelocity().x())+1.0));

    if(playerTypeMatchesPossession(bp) && bp->getObject()->collidingWithPlayer())
    if(rand1000(bp->getPlayerStats()->loseball_chance))
    {
       possession = PEVENT_NONE;
       bp->setShootFlag(true);
       ball_obj.setXMotion(true);
       ball_obj.setYMotion(true);
       ball_obj.setVelocity(point(randDouble(-4.0,4.0),randDouble(-4.0,4.0)));
    }
}

bool game::playerTypeMatchesPossession(bplayer *bp)
{
    if (possession == PEVENT_PLAYER1 && bp->getObject()->getType() == OBJ_PLAYER1)
        return true;
    if (possession == PEVENT_PLAYER2 && bp->getObject()->getType() == OBJ_PLAYER2)
        return true;
    return false;
}

bplayer * game::getPlayerOnOffense() {
    if (possession == PEVENT_PLAYER1) {
        return &player1_obj;
    }
    return &player2_obj;
}

std::string getPeriodString(int game_period)
{
    if (game_period == 0)
        return "R: ";
    return "OT" + int2String(game_period) + ": ";
}

std::string int2String(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

std::string double2String(double d)
{
    std::string s;
    std::stringstream ss;
    ss << d;
    s = ss.str();
    s.pop_back();
    s.pop_back();
    return s;
}

std::string getTimeString(int game_period, double time_elapsed)
{
    int time_left = getTimeLeft(game_period,time_elapsed);
    if (time_left <= 0)
        return "BUZZER";
    else
        return int2TimerString(time_left);
}

int getTimeLeft(int game_period, double time_elapsed)
{
    return (int)std::max(TOTAL_TIME_REMAINING(game_period,time_elapsed) + 1.0,0.0);
}

bool rollDice(int val)
{
    return (randInt(0,val) == 0);
}

bool rand1000(int val)
{
    return randInt(0,999) < val;
}

bool playerInInsideZone(bplayer *bp)
{
    if (bp->getLoc().x() < 200 && bp->getObject()->getType() == OBJ_PLAYER1)
        return true;
    if (bp->getLoc().x() >= 800 - bp->getArea().x() && bp->getObject()->getType() == OBJ_PLAYER2)
        return true;
    return false;
}

bool playerInRegTwoZone(bplayer *bp)
{
    if (bp->getLoc().x() >= 200 && bp->getLoc().x() < 500 && bp->getObject()->getType() == OBJ_PLAYER1)
        return true;
    if (bp->getLoc().x() < 800 - bp->getArea().x() && bp->getLoc().x() >= 500 - bp->getArea().x() && bp->getObject()->getType() == OBJ_PLAYER2)
        return true;
    return false;
}

bool playerInLongTwoZone(bplayer *bp)
{
    if (bp->getLoc().x() >= 500 && bp->getLoc().x() < THREE_POINT_LINE_LOC1 && bp->getObject()->getType() == OBJ_PLAYER1)
        return true;
    if (bp->getLoc().x() < 500 - bp->getArea().x() && bp->getLoc().x() >= THREE_POINT_LINE_LOC2 - bp->getArea().x() && bp->getObject()->getType() == OBJ_PLAYER2)
        return true;
    return false;
}

bool playerInThreeZone(bplayer *bp)
{
    if (bp->getLoc().x() >= THREE_POINT_LINE_LOC1 && bp->getObject()->getType() == OBJ_PLAYER1)
        return true;
    if (bp->getLoc().x() < THREE_POINT_LINE_LOC2 - bp->getArea().x() && bp->getObject()->getType() == OBJ_PLAYER2)
        return true;
    return false;
}


double randDouble(double low, double high)
{
    if (high < low)
        return 1.0;

    double normalized = (double)rand() / RAND_MAX;
    return low + normalized * (high - low);
}

int randInt(int low, int high)
{
    // check boundary
    if (low < 0 || high < 0)
        return 0;

    if (low == 0 && high == 0)
        return 0;

    return (rand() % (high - low + 1)) + low;
}

std::string int2TimerString(int t)
{
	std::string ret_val = "";

	uint ut = (uint)std::abs(t);

	uint days = (uint)(ut/86400);

	uint hours = (uint)((ut % 86400) / 3600);

	uint minutes = (uint)((ut % 3600) / 60);

	uint seconds = (ut % 60);

	if (t < 0)
            ret_val += "-";

	if (ut >= 86400)
            ret_val += int2String((int)days) + ":";

	if (ut >= 3600)
	{
            if (hours <= 9)
                ret_val += "0";
            ret_val += int2String((int)hours) + ":";
	}

	if (minutes <= 9)
            ret_val += "0";

        ret_val += int2String((int)minutes) + ":";

        if (seconds <= 9)
            ret_val += "0";

        ret_val += int2String((int)seconds);

        return ret_val;
}
