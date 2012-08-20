#ifndef __ENDBOSS_H__
#define __ENDBOSS_H__

//! Includes
#include "Enemy.h"


//! EndBoss class
class EndBoss : public Character
{
private:

                void            OnCreate();
                void            OnDelete();
                void            OnStartGame();
                void            OnStopGame();
                void            Update(const orxCLOCK_INFO &_rstInfo);
                void            OnAnimEvent(const orxSTRING _zAnim, const orxSTRING _zEvent, orxFLOAT _fTime, orxFLOAT _fValue);
                void            OnDeath();
                void            OnDamage();

                void            CreateSlotList();
                void            DeleteSlotList();
                void            SpawnWave();
                void            EnableProtection(orxBOOL _bEnable = orxTRUE);

                orxS32          ms32WaveCounter;
                orxS32          ms32WaveNumber;
                orxFLOAT        mfWaveDelay;
                orxBANK        *mpstSlotBank;
                ScrollObject   *mpoSpell;
};

#endif // __ENDBOSS_H__
