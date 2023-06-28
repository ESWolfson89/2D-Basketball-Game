#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "object.h"

void applyBallPhysics(object *);

void offsetObjPosition(object *);

void rotateBall(object *);

void applyGravity(object *);

void applyPlayerPhysics(object *);

void applyXResistance(object *, double, double);

void checkObjectFloorCollision(object *);

void setObjectToXWindowBoundary(object *);

void checkObjectWallCollision(object *);

void checkObjectWindowBoundary(object *);

void bounceBallOnBoundary(object *, bool);

bool checkBackboardBallCollision(object *);

void checkBallNetCollision(object *, object *);

void checkBallNetBottomCollision(object *, object *);

void checkBallHoopCollision(object *, object *);

void bounceBallOnNetBottom(object *);

bool ballCollidingWithNetBottom(object *, object *);

void bounceBallOnHoop(object *);

bool ballCollidingWithHoop(object *, object *);

void initBallPhysics(object *);

void initNetPhysics(object *, int);

void checkObjectXMotionFlags(object *);

bool objAtRightBoundary(object *);

bool objAtLeftBoundary(object *);

bool objAtLowerBoundary(object *);

bool checkBallAtScorePosition(object *,point,point);

bool objectAreasColliding(object *, object *);

void checkAndSetObjectLowerBoundary(object *);

void checkPlayerPlayerCollision(object *, object *);

void updatePlayerFloorBoundaryFlags(object *);

void checkObjectPossibleFloorBounce(object *);

double getEndVelocityFromType(obj_type);

double getSign(double);

#endif
