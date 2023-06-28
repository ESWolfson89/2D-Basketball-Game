#ifndef PLAYER_H_
#define PLAYER_H_

#include "object.h"

enum ShotZoneType {
     SHOTZONE_CLOSE,
     SHOTZONE_MIDDLE,
     SHOTZONE_FAR
};

enum DefenseType {
     DEFENSETYPE_INFRONT,
     DEFENSETYPE_WILD
};

struct bpstats_struct
{
    // range from 0 to 1000 chance out of 1000 each
    // game tick iteration
    int turn_chance_offense;
    int turn_chance_defense;
    int jump_chance_offense;
    int jump_chance_defense;
    int jump_chance_loose;
    int inside_chance;
    int regtwo_chance;
    int longtwo_chance;
    int three_chance;
    int loseball_chance;
    DefenseType defense_type;
};

static const bpstats_struct allplayer_stats[1] =
{
    {65,25,25,150,25,10,10,10,10,100,DEFENSETYPE_INFRONT}
};

class bplayer
{
    public:
        bplayer();
        bplayer(obj_type, point, point, int);
        void setMoveState(move_state);
        void setShootFlag(bool);
        void setScore(int);
        void incScore(int);
        void setFatigue(int);
        void incFatigue(int);
        void setShotZoneType(ShotZoneType);
        object *getObject();
        bpstats_struct *getPlayerStats();
        move_state getMoveState();
        DefenseType getDefenseType();
        bool getShootFlag();
        ShotZoneType getShotZoneType();
        point getLoc();
        point getArea();
        int getScore();
        int getFatigue();
    private:
        object obj;
        move_state move_flag;
        bpstats_struct bpstats;
        ShotZoneType shot_zone_type;
        bool shoot_flag;
        bool drop_flag;
        int player_score;
        int fatigue;
};

#endif
