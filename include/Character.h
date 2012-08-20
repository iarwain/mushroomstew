#ifndef __CHARACTER_H__
#define __CHARACTER_H__

//! Includes
#include "Scroll.h"


//! Constants
static  const orxSTRING szConfigIsCharacter         = "IsCharacter";
static  const orxSTRING szConfigIsPlayer            = "IsPlayer";
static  const orxSTRING szConfigIsEnemy             = "IsEnemy";
static  const orxSTRING szConfigIsEndBoss           = "IsEndBoss";
static  const orxSTRING szConfigDefaultOrientation  = "DefaultOrientation";
static  const orxSTRING szConfigGroundAcceleration  = "GroundAcceleration";
static  const orxSTRING szConfigIceAcceleration     = "IceAcceleration";
static  const orxSTRING szConfigGroundDeceleration  = "GroundDeceleration";
static  const orxSTRING szConfigIceDeceleration     = "IceDeceleration";
static  const orxSTRING szConfigCrouchDeceleration  = "CrouchDeceleration";
static  const orxSTRING szConfigAirAcceleration     = "AirAcceleration";
static  const orxSTRING szConfigAirDeceleration     = "AirDeceleration";
static  const orxSTRING szConfigMaxSpeed            = "MaxSpeed";
static  const orxSTRING szConfigMinSlideSpeed       = "MinSlideSpeed";
static  const orxSTRING szConfigJumpSpeed           = "JumpSpeed";
static  const orxSTRING szConfigAirJumpSpeed        = "AirJumpSpeed";
static  const orxSTRING szConfigJumpFX              = "JumpFX";
static  const orxSTRING szConfigAirJumpFX           = "AirJumpFX";
static  const orxSTRING szConfigLandFX              = "LandFX";
static  const orxSTRING szConfigIsStatic            = "IsStatic";
static  const orxSTRING szConfigIsSlot              = "IsSlot";
static  const orxSTRING szConfigIsInvisible         = "IsInvisible";
static  const orxSTRING szConfigIsIce               = "IsIce";
static  const orxSTRING szConfigHealth              = "Health";
static  const orxSTRING szConfigProtection          = "Protection";
static  const orxSTRING szConfigHealthBarLength     = "HealthBarLength";
static  const orxSTRING szConfigWeapon              = "Weapon";
static  const orxSTRING szConfigSpecialWeapon       = "SpecialWeapon";
static  const orxSTRING szConfigSpecialImmune       = "SpecialImmune";
static  const orxSTRING szConfigIcon                = "Icon";
static  const orxSTRING szConfigLeftAnchor          = "LeftAnchor";
static  const orxSTRING szConfigRightAnchor         = "RightAnchor";
static  const orxSTRING szConfigFeetOffset          = "FeetOffset";
static  const orxSTRING szConfigFlipFX              = "FlipFX";
static  const orxSTRING szConfigSlideFlipFX         = "SlideFlipFX";
static  const orxSTRING szConfigBigFallHeight       = "BigFallHeight";
static  const orxSTRING szConfigBigFallDamageCoef   = "BigFallDamageCoef";
static  const orxSTRING szConfigBigFallFX           = "BigFallFX";
static  const orxSTRING szConfigIsMini              = "IsMini";
static  const orxSTRING szConfigIsWhite             = "IsWhite";
static  const orxSTRING szConfigBloodColor          = "BloodColor";
static  const orxSTRING szConfigMaxAirJump          = "MaxAirJumpCounter";
static  const orxSTRING szConfigVisionDistance      = "VisionDistance";
static  const orxSTRING szConfigVisionAngle         = "VisionAngle";
static  const orxSTRING szConfigPredictionTime      = "PredictionTime";
static  const orxSTRING szConfigFootStepSound       = "FootStepSound";
static  const orxSTRING szConfigDyingSound          = "DyingSound";
static  const orxSTRING szConfigScore               = "Score";

static  const orxSTRING szAnimIdle                  = "Idle";
static  const orxSTRING szAnimRun                   = "Run";
static  const orxSTRING szAnimSlide                 = "Slide";
static  const orxSTRING szAnimCrouched              = "Crouched";
static  const orxSTRING szAnimJump                  = "Jump";
static  const orxSTRING szAnimFall                  = "Fall";
static  const orxSTRING szAnimLand                  = "Land";
static  const orxSTRING szAnimAttack                = "Attack";
static  const orxSTRING szAnimDeath                 = "Death";


//! Character class
class Character : public ScrollObject
{
public:

  enum Location
  {
    LocationOnGround = 0,
    LocationInAir,

    LocationNumber,

    LocationNone = orxENUM_NONE
  };

  enum Orientation
  {
    OrientationLeft = 0,
    OrientationRight,

    OrientationNumber,

    OrientationNone = orxENUM_NONE
  };

  enum Type
  {
    TypePlayer = 0,
    TypeEnemy,

    TypeNumber,

    TypeNone = orxENUM_NONE
  };

  enum Input
  {
    InputJump = 0,
    InputRunLeft,
    InputRunRight,
    InputAttack,
    InputSpecial,
    InputSwitchSpecial,
    InputCrouch,

    InputNumber,

    InputNone = orxENUM_NONE,

    InputShift = 16
  };

  enum Flag
  {
    FlagJumping       = 0x80000000,
    FlagFalling       = 0x40000000,
    FlagAttacking     = 0x20000000,
    FlagCrouching     = 0x10000000,
    FlagDead          = 0x08000000,
  };

                void            StartFire();
                void            StopFire();

                orxOBJECT *     GetWeapon() const {return mpstWeapon;}
                class SpecialWeapon *GetSpecialWeapon() const {return mpoSpecialWeapon;}

                orxFLOAT        GetHealth() const {return mfHealth;}
                orxFLOAT        GetMaxHealth() const {return mfMaxHealth;}
                orxFLOAT        GetProtection() const {return mfProtection;}

                Location        GetLocation() const {return meLocation;}
                Orientation     GetOrientation() const {return meOrientation;}
                Orientation     GetInitialOrientation() const {return meInitialOrientation;}

                Type            GetType()const {return meType;}

                ScrollObject *  GetGround(orxVECTOR *_pvContact = orxNULL, orxVECTOR *_pvNormal = orxNULL, const orxVECTOR *_pvShift = orxNULL) const;
                ScrollObject *  GetWall(Orientation _eOrientation, orxVECTOR *_pvContact = orxNULL, orxVECTOR *_pvNormal = orxNULL) const;

                orxS32          GetAirJumpCounter() const {return ms32AirJumpCounter;}

                orxBOOL         IsSeen(const orxVECTOR &_rvTarget) const;
                orxBOOL         IsInVisionRange(const orxVECTOR &_rvTarget) const;
                orxBOOL         IsInVisionCone(const orxVECTOR &_rvTarget) const;
                orxBOOL         IsInFront(const orxVECTOR &_rvTarget) const;
                orxBOOL         IsBehind(const orxVECTOR &_rvTarget) const {return !IsInFront(_rvTarget);}
                orxBOOL         IsTraceable(const orxVECTOR &_rvTarget) const;

          const orxVECTOR &     GetFeetPosition() const {return mvFeetPosition;}
          const orxVECTOR &     GetPosition() const {return mvPosition;}
          const orxVECTOR &     GetSpeed() const {return *orxObject_GetSpeed(GetOrxObject(), const_cast<orxVECTOR *>(&mvSpeed));}
          const orxVECTOR &     GetFallStart() const {return mvFallStart;}
          const orxVECTOR &     GetSight() const {return mvSight;}

                orxSTATUS       Damage(orxFLOAT _fDamage);
                void            Kill();

                orxBOOL         IsJumping() const {return TestFlags((ScrollObject::Flag)FlagJumping);}
                orxBOOL         IsFalling() const {return TestFlags((ScrollObject::Flag)FlagFalling);}
                orxBOOL         IsAttacking() const {return TestFlags((ScrollObject::Flag)FlagAttacking);}
                orxBOOL         IsCrouching() const {return TestFlags((ScrollObject::Flag)FlagCrouching);}
                orxBOOL         IsDead() const {return TestFlags((ScrollObject::Flag)FlagDead);}


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);
                void            OnStartGame();
                void            OnStopGame();

  virtual       void            OnDeath();
  virtual       void            OnDamage();
  virtual       void            OnBigFall();
  virtual       void            OnNewLocation(Location _eLocation);
  virtual       void            OnNewOrientation(Orientation _eOrientation);
                void            OnNewAnim(const orxSTRING _zOldAnim, const orxSTRING _zNewAnim, orxBOOL _bCut);
                void            OnAnimEvent(const orxSTRING _zAnim, const orxSTRING _zEvent, orxFLOAT _fTime, orxFLOAT _fValue);

                orxBOOL         OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);
                orxBOOL         OnSeparate(ScrollObject *_poCollider);

                void            SetLocation(Location _eLocation);
                void            SetOrientation(Orientation _eOrientation);

                void            ActivateInput(Input _eInput, orxBOOL _bActivate = orxTRUE);
                orxBOOL         IsInputActive(Input _eInput);

                void            SetJumping(orxBOOL _bFalling = orxTRUE);
                void            SetFalling(orxBOOL _bFalling = orxTRUE);
                void            SetAttacking(orxBOOL _bAttacking = orxTRUE);
                void            SetCrouching(orxBOOL _bCrouching = orxTRUE);

                void            SetAnim(const orxSTRING _zAnimName, orxBOOL _bReplaceCurrentAnim = orxFALSE);

                void            CreateHealthBar();
                void            DeleteHealthBar();
                void            UpdateHealthBar();

                orxFLOAT        GetAcceleration() const;
                orxFLOAT        GetDeceleration() const;

                void            SetAirJumpCounter(orxS32 _s32Counter) {ms32AirJumpCounter = _s32Counter;}

                void            SetProtection(orxFLOAT _fProtection) {orxASSERT(_fProtection >= orxFLOAT_0); mfProtection = _fProtection;}

private:

                orxVECTOR       mvPosition;
                orxVECTOR       mvFeetPosition;
                orxVECTOR       mvSpeed;
                orxVECTOR       mvFallStart;
                orxVECTOR       mvSight;
                orxFLOAT        mfVisionCos;
                orxFLOAT        mfVisionSquareDist;
                orxFLOAT        mfHealth;
                orxFLOAT        mfMaxHealth;
                orxFLOAT        mfProtection;
                orxS32          ms32AirJumpCounter;
                orxOBJECT *     mpstWeapon;
                orxOBJECT *     mpstHealthBarFg;
                orxOBJECT *     mpstHealthBarBg;
                class SpecialWeapon * mpoSpecialWeapon;
                Location        meLocation;
                Orientation     meInitialOrientation;
                Orientation     meOrientation;
                Type            meType;
};

#endif // __CHARACTER_H__
