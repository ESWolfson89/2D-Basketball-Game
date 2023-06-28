#ifndef OBJECT_H_
#define OBJECT_H_

#include "globals.h"
#include "point.h"

enum RotationType {
     ROTATION_NONE,
     ROTATION_FREE,
     ROTATION_FIXEDLEFT,
     ROTATION_FIXEDRIGHT
};

struct line
{
    point p1;
    point p2;
};


struct circle
{
    point loc;
    double rad;
};

class object
{
    public:
        object();
        object(obj_type,point,point,int);
        void addHitLine(line);
        void addHitCircle(circle);
        void setLoc(point);
        void setPrevLoc(point);
        void setVelocity(point);
        void offLoc(point);
        void offPrevLoc(point);
        void offVelocity(point);
        void freezeYMotion();
        void freezeXMotion();
        void freezeMotion();
        void setYMotion(bool);
        void setXMotion(bool);
        void activateMotion();
        void setAngle(double);
        void setCurrentFrame(int);
        void setAnimationState(bool);
        void incLifetime();
        void zeroLifetime();
        void setOrientation(SDL_RendererFlip);
        void flipOrientation();
        void setPlayerCollisionFlag(bool);
        void checkVelocityRange();
        void setRotationType(RotationType);
        bool collidingWithPlayer();
        bool getYMotionFlag();
        bool getXMotionFlag();
        bool isAnimating();
        point getLoc();
        point getPrevLoc();
        point getVelocity();
        point getArea();
        obj_type getType();
        circle getHitCircle(int);
        line getHitLine(int);
        SDL_RendererFlip getOrientation();
        RotationType getRotationType();
        double getAngle();
        int getNumHitLines();
        int getNumHitCircles();
        int getNumFrames();
        int getCurrentFrame();
        int getLifetime();
    private:
        std::vector<circle> hit_circles;
        std::vector<line> hit_lines;
        obj_type t_val;
        point loc;
        point prev_loc;
        point velocity;
        point area;
        double angle;
        bool in_motion_x;
        bool in_motion_y;
        bool animating;
        bool player_colliding;
        int lifetime;
        int current_frame;
        int max_frame;
        RotationType rotation_type;
        SDL_RendererFlip orientation;
};

#endif
