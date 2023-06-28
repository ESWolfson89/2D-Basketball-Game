#include "player.h"

bplayer::bplayer()
{

}

bplayer::bplayer(obj_type t, point loc, point area, int max_frame)
{
    obj = object(t,loc,area,max_frame);
    move_flag = MOVEST_NONE;
    shot_zone_type = SHOTZONE_CLOSE;
    shoot_flag = false;
    player_score = 0;
    fatigue = 1000;
    bpstats = allplayer_stats[0];
}

void bplayer::setFatigue(int f)
{
    fatigue = f;
}

void bplayer::incFatigue(int i)
{
    fatigue += i;
    if (fatigue > 1000)
        fatigue = 1000;
    if (fatigue < 250)
        fatigue = 250;
}

void bplayer::setMoveState(move_state m)
{
    move_flag = m;
}

void bplayer::setShootFlag(bool b)
{
    shoot_flag = b;
}

void bplayer::setShotZoneType(ShotZoneType szt)
{
    shot_zone_type = szt;
}

void bplayer::setScore(int s)
{
    player_score = s;
}

void bplayer::incScore(int i)
{
    player_score += i;
}

object *bplayer::getObject()
{
    return &obj;
}

move_state bplayer::getMoveState()
{
    return move_flag;
}

bpstats_struct *bplayer::getPlayerStats()
{
    return &bpstats;
}

bool bplayer::getShootFlag()
{
    return shoot_flag;
}

ShotZoneType bplayer::getShotZoneType()
{
    return shot_zone_type;
}

point bplayer::getLoc()
{
    return obj.getLoc();
}

point bplayer::getArea()
{
    return obj.getArea();
}

DefenseType bplayer::getDefenseType()
{
    return bpstats.defense_type;
}

int bplayer::getScore()
{
    return player_score;
}

int bplayer::getFatigue()
{
    return fatigue;
}
