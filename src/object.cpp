#include "object.h"

object::object()
{
}

object::object(obj_type t, point l, point a, int mf)
{
    // visual
    animating = false;
    angle = 0;
    rotation_type = ROTATION_FREE;
    current_frame = 0;
    max_frame = mf;
    lifetime = 0;
    orientation = SDL_FLIP_NONE;
    player_colliding = false;
    // physical
    t_val = t;
    prev_loc = loc = l;
    area = a;
    velocity = point(0.0,0.0);
    activateMotion();
}

void object::checkVelocityRange()
{
    if (velocity.x() < -MAX_VELOCITY)
        velocity.setx(-MAX_VELOCITY);
    if (velocity.x() > MAX_VELOCITY)
        velocity.setx(MAX_VELOCITY);
    if (velocity.y() < -MAX_VELOCITY)
        velocity.sety(-MAX_VELOCITY);
    if (velocity.y() > MAX_VELOCITY)
        velocity.sety(MAX_VELOCITY);
}

void object::setPlayerCollisionFlag(bool b)
{
    player_colliding = b;
}

void object::addHitLine(line l)
{
    hit_lines.push_back(l);
}

void object::addHitCircle(circle c)
{
    hit_circles.push_back(c);
}

void object::setLoc(point l)
{
    loc = l;
}

void object::setPrevLoc(point l)
{
    prev_loc = l;
}

void object::setVelocity(point v)
{
    velocity = v;
    checkVelocityRange();
}

void object::offLoc(point l)
{
    loc = addPoints(loc,l);
}

void object::offPrevLoc(point l)
{
    prev_loc = addPoints(prev_loc,l);
}

void object::offVelocity(point v)
{
    velocity = addPoints(velocity,v);
    checkVelocityRange();
}

void object::activateMotion()
{
    setXMotion(true);
    setYMotion(true);
}

void object::setYMotion(bool b)
{
    in_motion_y = b;
}

void object::setXMotion(bool b)
{
    in_motion_x = b;
}

void object::freezeYMotion()
{
    setVelocity(point(getVelocity().x(),0.0));
    setYMotion(false);
}

void object::freezeXMotion()
{
    setVelocity(point(0.0,getVelocity().y()));
    setXMotion(false);
}

void object::freezeMotion()
{
    freezeXMotion();
    freezeYMotion();
}

void object::setCurrentFrame(int i)
{
    current_frame = i;
    if (current_frame > max_frame)
    {
        current_frame = 0;
        zeroLifetime();
        animating = false;
    }
}

void object::setRotationType(RotationType rt) {
    rotation_type = rt;
}

void object::setOrientation(SDL_RendererFlip o)
{
    orientation = o;
}

void object::flipOrientation()
{
    if (orientation == SDL_FLIP_NONE)
        orientation = SDL_FLIP_HORIZONTAL;
    else
        orientation = SDL_FLIP_NONE;
}

bool object::getYMotionFlag()
{
    return in_motion_y;
}

bool object::getXMotionFlag()
{
    return in_motion_x;
}

point object::getLoc()
{
    return loc;
}

point object::getPrevLoc()
{
    return prev_loc;
}

point object::getVelocity()
{
    return velocity;
}

point object::getArea()
{
    return area;
}

obj_type object::getType()
{
    return t_val;
}

line object::getHitLine(int i)
{
    return hit_lines[i];
}

circle object::getHitCircle(int i)
{
    return hit_circles[i];
}

int object::getNumHitLines()
{
    return (int)hit_lines.size();
}

int object::getNumHitCircles()
{
    return (int)hit_circles.size();
}

int object::getCurrentFrame()
{
    return current_frame;
}

int object::getNumFrames()
{
    return max_frame + 1;
}

bool object::isAnimating()
{
    return animating;
}

void object::setAnimationState(bool b)
{
    animating = b;
}

double object::getAngle()
{
    return angle;
}

void object::setAngle(double a)
{
    angle = a;
}

void object::incLifetime()
{
    lifetime++;
}

void object::zeroLifetime()
{
    lifetime = 0;
}

int object::getLifetime()
{
    return lifetime;
}

bool object::collidingWithPlayer()
{
    return player_colliding;
}

RotationType object::getRotationType()
{
    return rotation_type;
}

SDL_RendererFlip object::getOrientation()
{
    return orientation;
}
