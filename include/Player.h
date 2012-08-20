#ifndef __PLAYER_H__
#define __PLAYER_H__

//! Includes
#include "Character.h"


//! Player class
class Player : public Character
{
private:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);
                void            OnDeath();

                orxOBJECT *     mpstDeathObject;
};

#endif // __PLAYER_H__
