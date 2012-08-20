#ifndef __MISSILE_H__
#define __MISSILE_H__

//! Includes
#include "Character.h"


//! Constants
static  const orxSTRING szConfigIsMissile           = "IsMissile";
static  const orxSTRING szConfigIsHoming            = "IsHoming";
static  const orxSTRING szConfigDamage              = "Damage";
static  const orxSTRING szConfigAimVelocity         = "AimVelocity";
static  const orxSTRING szConfigImpact              = "Impact";
static  const orxSTRING szConfigAltImpact           = "AltImpact";
static  const orxSTRING szConfigDodgeable           = "Dodgeable";


//! Missile class
class Missile : public ScrollObject
{
public:

                void            SetOwner(Character *_poOwner);
                Character *     GetOwner() const {return mpoOwner;}

                orxFLOAT        GetDamage() const {return mfDamage;}
                void            CreateImpact(const ScrollObject &_roCollider, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal = orxVECTOR_0, const orxVECTOR &_rvScale = orxVECTOR_1, orxBOOL _bAltImpact = orxFALSE) const;


private:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxBOOL         OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);

                orxFLOAT        mfDamage;
                Character      *mpoOwner;
};

#endif // __MISSILE_H__
