#ifndef __SPECIALWEAPON_H__
#define __SPECIALWEAPON_H__

//! Includes
#include "Character.h"


//! Constants
static  const orxSTRING szConfigIsSpecialWeapon     = "IsSpecialWeapon";
static  const orxSTRING szConfigWeaponShaderList    = "WeaponShaderList";
static  const orxSTRING szConfigChargeShaderList    = "ChargeShaderList";
static  const orxSTRING szConfigDuration            = "Duration";
static  const orxSTRING szConfigRefillRate          = "RefillRate";
static  const orxSTRING szConfigTimeClock           = "TimeClock";
static  const orxSTRING szConfigGravityStrength     = "GravityStrength";
static  const orxSTRING szConfigGravityBackup       = "GravityBackup";
static  const orxSTRING szConfigEnlargeFX           = "EnlargeFX";
static  const orxSTRING szConfigShrinkFX            = "ShrinkFX";
static  const orxSTRING szConfigOnSound             = "OnSound";
static  const orxSTRING szConfigOffSound            = "OffSound";


//! Special weapon class
class SpecialWeapon : public ScrollObject
{
public:

  enum Type
  {
    TypeTime = 0,
    TypeGravity,

    TypeNumber,

    TypeNone = orxENUM_NONE
  };

                void            SetOwner(Character *_poOwner);
                Character *     GetOwner() const {return mpoOwner;}

                void            Activate(orxBOOL _bActivate = orxTRUE);
                orxSTATUS       Switch();
                orxBOOL         IsActive() const {return mbActivated;}
                Type            GetType() const {return meType;}
                orxFLOAT        GetChargeLevel() const {return mfCharge / mfDuration;}


private:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxBOOL         OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);
                orxBOOL         OnSeparate(ScrollObject *_poCollider);

                orxFLOAT        mfCharge;
                orxFLOAT        mfDuration;
                orxFLOAT        mfRefillRate;
                orxBOOL         mbActivated;
                Type            meType;
                Character      *mpoOwner;
};

#endif // __SPECIALWEAPON_H__
