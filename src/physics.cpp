#include "physics.h"

void applyBallPhysics(object *ball)
{
    applyGravity(ball);
    applyXResistance(ball,XBALL_GROUND_RESISTANCE,XBALL_AIR_RESISTANCE);
    rotateBall(ball);
    offsetObjPosition(ball);
    checkObjectWindowBoundary(ball);
    checkObjectXMotionFlags(ball);
}

void applyPlayerPhysics(object* bp_obj)
{
    applyGravity(bp_obj);
    applyXResistance(bp_obj,XPLAYER_GROUND_RESISTANCE,XPLAYER_AIR_RESISTANCE);
    offsetObjPosition(bp_obj);
    checkObjectWindowBoundary(bp_obj);
    checkObjectXMotionFlags(bp_obj);
    checkAndSetObjectLowerBoundary(bp_obj);
}

// for aesthetic purposes for now...
void rotateBall(object *obj)
{
    double angle_increment = obj->getVelocity().x() * 36.0;
    switch(obj->getRotationType()) {
        case(ROTATION_FIXEDRIGHT): {
             angle_increment = std::abs(angle_increment);
             break;
        }
        case(ROTATION_FIXEDLEFT): {
             angle_increment = -1.0 * std::abs(angle_increment);
             break;
        }
        default:
             break;
    }

    obj->setAngle(angle_increment + obj->getAngle());
    if (obj->getAngle() >= 360.0)
        obj->setAngle(obj->getAngle() - 360.0);
}

void checkObjectXMotionFlags(object * obj)
{
    if (std::abs(obj->getVelocity().x()) < getEndVelocityFromType(obj->getType()))
    {
        obj->freezeXMotion();
    }
    else
    {
        obj->setXMotion(true);
    }
}

double getEndVelocityFromType(obj_type ot)
{
    if (ot == OBJ_BASKETBALL)
    {
        return END_BALL_X_VELOCITY;
    }
    return END_PLAYER_X_VELOCITY;
}

void offsetObjPosition(object *obj)
{
    if (obj->getYMotionFlag() || obj->getXMotionFlag())
    {
        obj->setPrevLoc(obj->getLoc());
        obj->offLoc(obj->getVelocity());
    }
}

void applyGravity(object *obj)
{
    if (obj->getYMotionFlag())
    {
        obj->offVelocity(point(0.0,GRAVITY_INCREMENT));
    }
}

void applyXResistance(object *obj, double ground_offset, double air_offset)
{
    double applied_x_resistance;
    if (obj->getXMotionFlag())
    {
        if (objAtLowerBoundary(obj))
            applied_x_resistance = (obj->getVelocity().x() >= 0.0 ? -ground_offset : ground_offset);
        else
            applied_x_resistance = (obj->getVelocity().x() >= 0.0 ? -air_offset : air_offset);

        obj->offVelocity(point(applied_x_resistance,0.0));
    }
}

void checkObjectFloorCollision(object *obj)
{
    if (obj->getLoc().y() + obj->getArea().y() >= (double)WINDOW_HEIGHT - WINDOW_OFFSET)
    {
        obj->setLoc(point(obj->getLoc().x(),(double)WINDOW_HEIGHT - WINDOW_OFFSET - obj->getArea().y()));
        checkObjectPossibleFloorBounce(obj);
        updatePlayerFloorBoundaryFlags(obj);
    }
    else
    {
        obj->activateMotion();
    }
}

void checkObjectPossibleFloorBounce(object *obj)
{
    if (obj->getVelocity().y() > END_BALL_Y_VELOCITY)
    {
        if (obj->getType() == OBJ_BASKETBALL)
        {
            bounceBallOnBoundary(obj,false);
        }
    }
    else
    {
        obj->freezeYMotion();
    }
}

void updatePlayerFloorBoundaryFlags(object *obj)
{
    if (obj->getType() == OBJ_PLAYER1 || obj->getType() == OBJ_PLAYER2)
    {
        obj->freezeYMotion();
    }
}

void setObjectToXWindowBoundary(object *obj)
{
    if (objAtRightBoundary(obj))
    {
        obj->setLoc(point((double)WINDOW_WIDTH - WINDOW_OFFSET - obj->getArea().x(),obj->getLoc().y()));
    }
    else
    {
        obj->setLoc(point(WINDOW_OFFSET,obj->getLoc().y()));
    }
}

void checkObjectWallCollision(object *obj)
{
    if (objAtRightBoundary(obj) || objAtLeftBoundary(obj))
    {
        setObjectToXWindowBoundary(obj);
        if (obj->getType() == OBJ_BASKETBALL)
        {
            bounceBallOnBoundary(obj,true);
        }
    }
}

void checkObjectWindowBoundary(object *obj)
{
    if (obj->getYMotionFlag())
    {
        checkObjectFloorCollision(obj);
    }
    if (obj->getXMotionFlag())
    {
        checkObjectWallCollision(obj);
    }
}

void bounceBallOnBoundary(object *obj, bool wall)
{
    if (wall)
    {
        if (!checkBackboardBallCollision(obj))
        {
            obj->setVelocity(point(-1.0 * obj->getVelocity().x(),obj->getVelocity().y()));
        }
        else
        {
            obj->setVelocity(point(-BACKBOARD_DAMPENING_X_FACTOR*obj->getVelocity().x(),
                                    BACKBOARD_DAMPENING_Y_FACTOR*obj->getVelocity().y()));
        }
    }
    else
    {
        obj->setVelocity(point(obj->getVelocity().x(),-1.0*FLOOR_DAMPENING_FACTOR*obj->getVelocity().y()));
    }
}

void initBallPhysics(object *obj)
{
    circle c = {point(BALL_RADIUS,BALL_RADIUS),BALL_RADIUS};
    obj->addHitCircle(c);
}

void initNetPhysics(object *net, int net_num)
{
    circle c1,c2;
    line l;

    if (net_num == 0)
    {
        l.p1 = point(17.0,66.0);
        l.p2 = point(59.0,66.0);
        c1.loc = point(17.0,51.0);
        c2.loc = point(59.0,51.0);
        c1.rad = c2.rad = RIM_EDGE_CIRCLE_RADIUS;
    }
    else
    {
        l.p1 = point(3.0,66.0);
        l.p2 = point(45.0,66.0);
        c1.loc = point(3.0,51.0);
        c2.loc = point(45.0,51.0);
        c1.rad = c2.rad = RIM_EDGE_CIRCLE_RADIUS;
    }
    net->addHitCircle(c1);
    net->addHitCircle(c2);
    net->addHitLine(l);
    net->freezeMotion();
}

bool checkBackboardBallCollision(object *ball)
{
    // equivalent to NET2_LOC_Y
    double backboard_global_y = BACKBOARD_X_LOC;
    double backboard_area_y = BACKBOARD_LENGTH;
    if (ball->getLoc().y() <= backboard_global_y + backboard_area_y && ball->getLoc().y() >= backboard_global_y)
    {
        return true;
    }
    return false;

}

void checkBallNetCollision(object *ball, object *net)
{
    if (ball->getLoc().x() > 120 && ball->getLoc().x() < 880 - ball->getArea().x())
        return;
    checkBallHoopCollision(ball,net);
    checkBallNetBottomCollision(ball,net);
}

void checkBallNetBottomCollision(object *ball, object *net)
{
    if (ballCollidingWithNetBottom(ball,net))
        bounceBallOnNetBottom(ball);
}

void checkBallHoopCollision(object *ball, object *net)
{
    if (ballCollidingWithHoop(ball,net)) {
        bounceBallOnHoop(ball);
    }
    else {
        ball->setRotationType(ROTATION_FREE);
    }
}

void bounceBallOnNetBottom(object *ball)
{
    if (ball->getVelocity().y() < 0.0)
        ball->setLoc(ball->getPrevLoc());
    ball->setVelocity(point(-RIM_DAMPENING_X_FACTOR*ball->getVelocity().x()+((-1)*getSign(ball->getVelocity().x())*RIM_DAMPENING_X_OFFSET),
                      std::abs(ball->getVelocity().y())));
}

bool ballCollidingWithNetBottom(object *ball, object *net)
{
    point ball_center = addPoints(ball->getLoc(),ball->getHitCircle(0).loc);
    line rim_tip = {addPoints(net->getHitLine(0).p1,net->getLoc()),addPoints(net->getHitLine(0).p2,net->getLoc())};
    if (ball_center.y() >= rim_tip.p1.y() - BALL_RADIUS &&
        ball_center.y() <= rim_tip.p2.y() + BALL_RADIUS)
    {
        if (ball_center.x() >= rim_tip.p1.x() - BALL_RADIUS &&
            ball_center.x() <= rim_tip.p2.x() + BALL_RADIUS)
        {
            return true;
        }
    }
    return false;
}

void bounceBallOnHoop(object *ball)
{
    int roll_delta;
    bool first_half;
    bool near_front;
    ball->setLoc(ball->getPrevLoc());
    if (std::abs(ball->getVelocity().y()) > 0.25 || std::abs(ball->getVelocity().x()) > 0.25)
        ball->setVelocity(point((-((double)(rand()%74) + 25.0)/100.0)*ball->getVelocity().x()+((-1)*getSign(ball->getVelocity().x())*RIM_DAMPENING_X_OFFSET),
                                (-((double)(rand()%74) + 25.0)/100.0)*ball->getVelocity().y()));
    else {
        first_half = (ball->getLoc().x() < WINDOW_WIDTH / 2);

        if (first_half) {
            near_front = (ball->getLoc().x() < 63.0 - (ball->getArea().x() / 2.0)) && ball->getLoc().x() > 20.0;
            roll_delta = near_front ? -1.0 : 1.0;
        }
        else {
            near_front = (ball->getLoc().x() > NET2_LOC_X - (ball->getArea().x() / 2.0)) && ball->getLoc().x() < 980.0;
            roll_delta = near_front ? 1.0 : -1.0;
        }

        ball->setRotationType(roll_delta >= 0.0 ? ROTATION_FIXEDRIGHT : ROTATION_FIXEDLEFT);
        ball->setVelocity(point(ball->getVelocity().x() + roll_delta, ball->getVelocity().y()));
    }
}

bool ballCollidingWithHoop(object *ball, object *net)
{
    point ball_center_curr = addPoints(ball->getLoc(),ball->getHitCircle(0).loc);
    point ball_center_prev = addPoints(ball->getPrevLoc(),ball->getHitCircle(0).loc);
    point ball_center_midpoint = getMidpoint(ball_center_curr,ball_center_prev);
    point rim_circle_loc;
    double radius_squared;
    for (int i = 0; i < net->getNumHitCircles(); ++i)
    {
        radius_squared = net->getHitCircle(i).rad + ball->getHitCircle(0).rad;
        radius_squared = radius_squared * radius_squared;
        rim_circle_loc = addPoints(net->getLoc(),net->getHitCircle(i).loc);
        if (distanceSquared(ball_center_curr,rim_circle_loc) < radius_squared ||
            distanceSquared(ball_center_midpoint,rim_circle_loc) < radius_squared)
        {
            return true;
        }
    }
    return false;
}

bool objAtLeftBoundary(object *obj)
{
    return obj->getLoc().x() <= WINDOW_OFFSET;
}

bool objAtRightBoundary(object *obj)
{
    return obj->getLoc().x() >= (double)WINDOW_WIDTH - WINDOW_OFFSET - obj->getArea().x();
}

bool objAtLowerBoundary(object *obj)
{
    return obj->getLoc().y() >= (double)WINDOW_HEIGHT - WINDOW_OFFSET - obj->getArea().y();
}

void checkAndSetObjectLowerBoundary(object *obj)
{
    if (objAtLowerBoundary(obj))
    {
        obj->setLoc(point(obj->getLoc().x(),(double)WINDOW_HEIGHT - WINDOW_OFFSET - obj->getArea().y()));
    }
}

bool checkBallAtScorePosition(object *ball, point loc1, point loc2)
{
    point ball_center = addPoints(ball->getLoc(),ball->getHitCircle(0).loc);
    if (inRange(ball_center,point(loc1.x(),loc1.y()),point(loc2.x(),loc2.y())))
    {
        if (ball->getVelocity().y() > 0.0)
        {
            return true;
        }
    }
    return false;
}

bool objectAreasColliding(object *bp1_obj, object *bp2_obj)
{
    point bp1_obj_center = point(bp1_obj->getLoc().x() + bp1_obj->getArea().x()/2.0,bp1_obj->getLoc().y() + bp1_obj->getArea().y()/2.0);
    point bp2_obj_center = point(bp2_obj->getLoc().x() + bp2_obj->getArea().x()/2.0,bp2_obj->getLoc().y() + bp2_obj->getArea().y()/2.0);
    if (std::abs(bp1_obj_center.x() - bp2_obj_center.x()) * 2.0 < bp1_obj->getArea().x() + bp2_obj->getArea().x() &&
        std::abs(bp1_obj_center.y() - bp2_obj_center.y()) * 2.0 < bp1_obj->getArea().y() + bp2_obj->getArea().y())
        {
            return true;
        }
    return false;
}

void checkPlayerPlayerCollision(object *bp1_obj, object *bp2_obj)
{
    point new_velocity_bp1_obj;
    point new_velocity_bp2_obj;
    if (objectAreasColliding(bp1_obj,bp2_obj))
    {
        new_velocity_bp1_obj = point(-1.0*std::abs(bp2_obj->getVelocity().x()),-4.0);
        new_velocity_bp2_obj = point(1.0*std::abs(bp1_obj->getVelocity().x()),-4.0);
        bp1_obj->setVelocity(new_velocity_bp1_obj);
        bp2_obj->setVelocity(new_velocity_bp2_obj);
        bp1_obj->setLoc(bp1_obj->getPrevLoc());
        bp2_obj->setLoc(bp2_obj->getPrevLoc());
        bp1_obj->offLoc(bp1_obj->getVelocity());
        bp2_obj->offLoc(bp2_obj->getVelocity());
        bp1_obj->setPlayerCollisionFlag(true);
        bp2_obj->setPlayerCollisionFlag(true);
    }
    else
    {
        bp1_obj->setPlayerCollisionFlag(false);
        bp2_obj->setPlayerCollisionFlag(false);
        if (!objAtLowerBoundary(bp1_obj))
        {
            bp1_obj->setYMotion(true);
        }
        if (!objAtLowerBoundary(bp2_obj))
        {
            bp2_obj->setYMotion(true);
        }
    }
}

double getSign(double val)
{
    if (val > 0.0)
        return 1.0;
    else if (val < 0.0)
        return -1.0;
    else
        return 0.0;
}
