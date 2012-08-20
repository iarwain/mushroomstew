#ifndef __ENEMY_H__
#define __ENEMY_H__

//! Includes
#include "Character.h"


//! Enemy class
class Enemy : public Character
{
private:

                void            OnCreate();
                void            OnDelete();
                void            OnStartGame();
                void            Update(const orxCLOCK_INFO &_rstInfo);
                void            OnDeath();
                void            OnDamage();

                orxFLOAT        mfAttackDelay;
                orxFLOAT        mfPatrolDelay;
                Orientation     meDirection;
                orxBOOL         mbForceReorient;
};

#endif // __ENEMY_H__
