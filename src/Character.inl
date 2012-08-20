//! Code
static void SetOrxObjectAnim(orxOBJECT *_pstObject, const orxSTRING _zAnimName, orxBOOL _bReplaceCurrentAnim)
{
  orxBOOL bClearTarget = orxTRUE;

  // Has requested anim?
  if(_zAnimName)
  {
    // Pushes object's config section
    orxConfig_PushSection(orxObject_GetName(_pstObject));

    // Valid?
    if(orxConfig_HasValue(_zAnimName))
    {
      // Should replace current anim?
      if(_bReplaceCurrentAnim)
      {
        // Updates current anim
        orxObject_SetCurrentAnim(_pstObject, orxConfig_GetString(_zAnimName));
      }
      else
      {
        // Updates target anim
        orxObject_SetTargetAnim(_pstObject, orxConfig_GetString(_zAnimName));
      }

      // Don't clear target
      bClearTarget = orxFALSE;
    }

    // Pops config section
    orxConfig_PopSection();
  }

  // Should clear target?
  if(bClearTarget)
  {
    // Removes target anim
    orxObject_SetTargetAnim(_pstObject, orxNULL);
  }
}

void Character::OnCreate()
{
  // Sets status
  orxConfig_SetBool(szConfigIsCharacter, orxTRUE);

  // Inits variables
  mfHealth              = orxConfig_GetFloat(szConfigHealth);
  mfMaxHealth           = mfHealth;
  mfProtection          = orxConfig_GetFloat(szConfigProtection);
  ms32AirJumpCounter    = 0;
  meLocation            = LocationOnGround;
  meInitialOrientation  = !orxString_ICompare(orxConfig_GetString(szConfigDefaultOrientation), "right") ? OrientationRight : OrientationLeft;
  meOrientation         = meInitialOrientation;
  meType                = orxConfig_GetBool(szConfigIsPlayer) ? TypePlayer : TypeEnemy;
  mfVisionCos           = orxConfig_HasValue(szConfigVisionAngle) ? orxMath_Cos(orx2F(0.5f) * orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(szConfigVisionAngle)) : orxFLOAT_0;
  mfVisionSquareDist    = orxConfig_GetFloat(szConfigVisionDistance) * orxConfig_GetFloat(szConfigVisionDistance);
  mpstHealthBarBg       = orxNULL;
  mpstHealthBarFg       = orxNULL;
  mpoSpecialWeapon      = orxNULL;
  orxVector_Copy(&mvPosition, &orxVECTOR_0);
  orxVector_Copy(&mvFeetPosition, &orxVECTOR_0);
  orxVector_Copy(&mvFallStart, &orxVECTOR_0);
  orxVector_Copy(&mvSight, &orxVECTOR_0);

  // Has weapon?
  if(orxConfig_HasValue(szConfigWeapon))
  {
    // Creates it
    mpstWeapon = orxObject_CreateFromConfig(orxConfig_GetString(szConfigWeapon));

    // Valid?
    if(mpstWeapon)
    {
      // Binds it
      orxObject_SetParent(mpstWeapon, GetOrxObject());
      orxObject_SetOwner(mpstWeapon, GetOrxObject());
    }
  }
  else
  {
    // Clears it
    mpstWeapon = orxNULL;
  }

  // Stops firing
  StopFire();
}

void Character::OnDelete()
{
  // Has weapon?
  if(mpstWeapon)
  {
    // Unbinds it
    orxObject_SetParent(mpstWeapon, orxNULL);
    orxObject_SetOwner(mpstWeapon, orxNULL);

    // Deletes it
    orxObject_SetLifeTime(mpstWeapon, orxFLOAT_0);
    mpstWeapon = orxNULL;
  }
}

void Character::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Not dead?
  if(!IsDead())
  {
    // Is outside world limits?
    if(!orxAABox_IsInside(&MushroomStew::GetInstance().GetWorldLimit(), &GetPosition()))
    {
      // Kills self
      Kill();
    }

    // No health left?
    if(mfHealth <= orxFLOAT_0)
    {
      // Sets death anim
      SetAnim(szAnimDeath, orxTRUE);

      // Clears speed
      orxObject_SetSpeed(GetOrxObject(), &orxVECTOR_0);

      // Updates death status
      SetFlags((ScrollObject::Flag)FlagDead);
    }
    else
    {
      orxVECTOR   vScale, vSpeed;
      orxFLOAT    fRotation;
      orxOBJECT  *pstOrxObject;

      // Gets character's orx object
      pstOrxObject = GetOrxObject();

      // Gets its position, scale, rotation and speed
      orxObject_GetWorldPosition(pstOrxObject, &mvPosition);
      orxObject_GetWorldScale(pstOrxObject, &vScale);
      fRotation = orxObject_GetWorldRotation(pstOrxObject);
      orxObject_GetSpeed(pstOrxObject, &vSpeed);

      // Gets its feet offset
      orxConfig_GetVector(szConfigFeetOffset, &mvFeetPosition);

      // Updates feet position
      orxVector_Add(&mvFeetPosition, orxVector_2DRotate(&mvFeetPosition, orxVector_Mul(&mvFeetPosition, &mvFeetPosition, &vScale), fRotation), &mvPosition);

      // Gets its sight
      orxVector_2DRotate(&mvSight, orxVector_Set(&mvSight, (GetOrientation() == OrientationRight) ? orxFLOAT_1 : -orxFLOAT_1, orxFLOAT_0, orxFLOAT_0), fRotation);

      // No ground?
      if(!GetGround())
      {
        // Updates location
        SetLocation(LocationInAir);
      }

      // Depending on its location
      switch(GetLocation())
      {
        case LocationInAir:
        {
          // Not falling and going down?
          if(!IsFalling() && (vSpeed.fY >= orxFLOAT_1))
          {
            // Falls!
            SetFalling();
          }

          // Fall through
        }

        case LocationOnGround:
        {
          orxVECTOR vAcceleration;
          orxBOOL   bBrake = orxFALSE;

          // Clears acceleration
          orxVector_Copy(&vAcceleration, &orxVECTOR_0);

          // On ground?
          if(GetLocation() == LocationOnGround)
          {
            // Crouching?
            if(IsInputActive(InputCrouch))
            {
              // Updates crouching status
              SetCrouching();

              // Asks for brake
              bBrake = orxTRUE;
            }
            else
            {
              // Updates crouching status
              SetCrouching(orxFALSE);
            }
          }

          // Has a special weapon? and use it?
          if(mpoSpecialWeapon)
          {
            // Use it?
            if(IsInputActive(InputSpecial))
            {
              // Updates it
              mpoSpecialWeapon->Activate(!mpoSpecialWeapon->IsActive());
            }
            // Switch it?
            else if(IsInputActive(InputSwitchSpecial))
            {
              // Updates it
              mpoSpecialWeapon->Switch();
            }
          }

          // Not crouching?
          if(!IsCrouching())
          {
            // Jumping?
            if(IsInputActive(InputJump))
            {
              // Can do additional air jump?
              if(GetAirJumpCounter() < orxConfig_GetS32(szConfigMaxAirJump))
              {
                orxOBJECT      *pstFX = orxNULL;
                const orxSTRING zFXName;

                // Updates jump status
                SetJumping();

                // In air?
                if(GetLocation() == LocationInAir)
                {
                  // Increases air jump counter
                  SetAirJumpCounter(GetAirJumpCounter() + 1);
                }

                // Updates its vertical speed
                vSpeed.fY = -orxConfig_GetFloat((GetLocation() == LocationOnGround) ? szConfigJumpSpeed : szConfigAirJumpSpeed);

                // Gets FX name
                zFXName = (GetLocation() == LocationOnGround) ? orxConfig_GetString(szConfigJumpFX) : orxConfig_GetString(szConfigAirJumpFX);

                // Valid?
                if(zFXName != orxSTRING_EMPTY)
                {
                  // Adds jump FX

                  pstFX = orxObject_CreateFromConfig(zFXName);

                  // Valid?
                  if(pstFX)
                  {
                    // Simple jump?
                    if(GetLocation() == LocationOnGround)
                    {
                      orxVECTOR vPos;

                      if(GetGround(&vPos))
                      {
                        // Updates its position
                        orxObject_SetPosition(pstFX, &vPos);
                      }
                      else
                      {
                        // Updates its position
                        orxObject_SetPosition(pstFX, &GetFeetPosition());
                      }
                    }
                    else
                    {
                      // Updates its position
                      orxObject_SetPosition(pstFX, &GetFeetPosition());
                    }
                  }
                }

                // Makes sure we're not on ground and not falling
                SetLocation(LocationInAir);
                SetFalling(orxFALSE);
              }
            }
            // Attacking?
            else if(IsInputActive(InputAttack))
            {
              // Updates status
              SetAttacking();
            }

            // Going right?
            if(IsInputActive(InputRunRight))
            {
              // Updates acceleration
              orxVector_Set(&vAcceleration, GetAcceleration() * _rstInfo.fDT, orxFLOAT_0, orxFLOAT_0);

              // Updates anim
              SetAnim((GetLocation() == LocationOnGround) ? szAnimRun : orxNULL);

              // Updates orientation
              SetOrientation(OrientationRight);
            }
            // Going left?
            else if(IsInputActive(InputRunLeft))
            {
              // Updates acceleration
              orxVector_Set(&vAcceleration, -GetAcceleration() * _rstInfo.fDT, orxFLOAT_0, orxFLOAT_0);

              // Updates anim
              SetAnim((GetLocation() == LocationOnGround) ? szAnimRun : orxNULL);

              // Updates orientation
              SetOrientation(OrientationLeft);
            }
            else
            {
              // Not attacking?
              if(!IsAttacking())
              {
                // Updates anim
                SetAnim(orxNULL);
              }

              // Should brake
              bBrake = orxTRUE;
            }
          }
          else
          {
            // Asks for brake
            bBrake = orxTRUE;
          }

          // Should brake?
          if(bBrake)
          {
            orxFLOAT fDeceleration;

            // Gets deceleration amount
            fDeceleration = GetDeceleration() * _rstInfo.fDT;

            // Almost stopped?
            if(orxMath_Abs(vSpeed.fX) <= fDeceleration)
            {
              // Stops player
              orxVector_Set(&vAcceleration, -vSpeed.fX, orxFLOAT_0, orxFLOAT_0);
            }
            else
            {
              // Updates acceleration
              orxVector_Set(&vAcceleration, (vSpeed.fX > orxFLOAT_0) ? -fDeceleration : fDeceleration, orxFLOAT_0, orxFLOAT_0);
            }
          }

          // Updates speed
          orxVector_Add(&vSpeed, &vSpeed, &vAcceleration);

          // Clamps it
          vSpeed.fX = orxCLAMP(vSpeed.fX, -orxConfig_GetFloat(szConfigMaxSpeed), orxConfig_GetFloat(szConfigMaxSpeed));

          // Applies it
          orxObject_SetSpeed(pstOrxObject, &vSpeed);

          break;
        }

        default:
        {
          // Error
          orxASSERT(orxFALSE && "Can't update character <%s>, unknown location [%ld]", GetName(), GetLocation());

          break;
        }
      }
    }
  }

  // Updates health bar
  UpdateHealthBar();
}

void Character::OnStartGame()
{
  orxVECTOR vScale;

  // Has special weapon?
  if(orxConfig_HasValue(szConfigSpecialWeapon))
  {
    const orxSTRING zSpecialWeaponName;

    // Gets its name
    zSpecialWeaponName = orxConfig_GetString(szConfigSpecialWeapon);

    // Valid?
    if(orxConfig_HasSection(zSpecialWeaponName))
    {
      // Creates it
      mpoSpecialWeapon = ScrollCast<SpecialWeapon *>(MushroomStew::GetInstance().CreateObject(zSpecialWeaponName));

      // Valid?
      if(mpoSpecialWeapon)
      {
        // Binds it
        mpoSpecialWeapon->SetOwner(this);
      }
    }
    else
    {
      // Clears it
      mpoSpecialWeapon = orxNULL;
    }
  }
  else
  {
    // Clears it
    mpoSpecialWeapon = orxNULL;
  }

  // Is flipped?
  if(orxObject_GetScale(GetOrxObject(), &vScale)->fX < orxFLOAT_0)
  {
    meOrientation = (meOrientation == OrientationLeft) ? OrientationRight : OrientationLeft;
  }

  // Creates health bar
  CreateHealthBar();

  // Updates it
  UpdateHealthBar();
}

void Character::OnStopGame()
{
  // Has special weapon
  if(mpoSpecialWeapon)
  {
    // Deletes it
    MushroomStew::GetInstance().DeleteObject(mpoSpecialWeapon);
    mpoSpecialWeapon = orxNULL;
  }

  // Deletes health bar
  DeleteHealthBar();
}

void Character::OnDeath()
{
  // Deletes health bar
  DeleteHealthBar();
}

void Character::OnDamage()
{
}

void Character::OnBigFall()
{
  // Pushes section
  PushConfigSection();

  // Deals damage
  Damage(orxConfig_GetFloat(szConfigBigFallDamageCoef) * GetMaxHealth());

  // Pops config section
  PopConfigSection();
}

void Character::OnNewLocation(Location _eLocation)
{
  // Was in air?
  if(GetLocation() == LocationInAir)
  {
    // Now on ground?
    if(_eLocation == LocationOnGround)
    {
      orxOBJECT      *pstFX = orxNULL;
      orxBOOL         bBigFall = orxFALSE;
      const orxSTRING zFXName;

      // Lands!
      SetFalling(orxFALSE);

      // Clears jump status
      SetJumping(orxFALSE);

      // Clears air jump counter
      SetAirJumpCounter(0);

      // Pushes character's section
      PushConfigSection();

      // Big fall?
      if(orxConfig_HasValue(szConfigBigFallHeight) && (GetFallStart().fY - GetPosition().fY < -orxConfig_GetFloat(szConfigBigFallHeight)))
      {
        // Shakes camera
        MushroomStew::GetInstance().ShakeCamera(orxConfig_GetBool(szConfigIsMini));

        // Updates status
        bBigFall = orxTRUE;
      }

      // Gets FX name
      zFXName = orxConfig_GetString(bBigFall ? szConfigBigFallFX : szConfigLandFX);

      // Valid?
      if(zFXName != orxSTRING_EMPTY)
      {
        // Adds land FX
        pstFX = orxObject_CreateFromConfig(zFXName);

        // Valid?
        if(pstFX)
        {
          orxVECTOR vPos;

          if(GetGround(&vPos))
          {
            // Updates its position
            orxObject_SetPosition(pstFX, &vPos);
          }
          else
          {
            // Updates its position
            orxObject_SetPosition(pstFX, &GetFeetPosition());
          }
        }
      }

      // Bigfall?
      if(bBigFall)
      {
        // Calls bigfall callback
        OnBigFall();
      }

      // Pops section
      PopConfigSection();

      // Clears fall start
      orxVector_Copy(&mvFallStart, &orxVECTOR_0);
    }
  }

  // Stores new location
  meLocation = _eLocation;
}

void Character::OnNewOrientation(Orientation _eOrientation)
{
  // Changed orientation?
  if(GetOrientation() != _eOrientation)
  {
    orxBOOL bUseRegularFX = orxTRUE;

    // Pushes character section
    PushConfigSection();

    // On ground?
    if(GetLocation() == LocationOnGround)
    {
      orxVECTOR vSpeed;

      // Gets speed
      orxObject_GetSpeed(GetOrxObject(), &vSpeed);

      // Fast enough?
      if(orxConfig_HasValue(szConfigMinSlideSpeed) && (orxMath_Abs(vSpeed.fX) >= orxConfig_GetFloat(szConfigMinSlideSpeed)))
      {
        // Sets sliding animation
        SetAnim(szAnimSlide);

        // Adds slide flip FX
        AddFX(orxConfig_GetString(szConfigSlideFlipFX));

        // No regular flip FX
        bUseRegularFX = orxFALSE;
      }
    }

    // Should use regular FX?
    if(bUseRegularFX)
    {
      // Adds flip FX
      AddFX(orxConfig_GetString(szConfigFlipFX));
    }

    // Pops config section
    PopConfigSection();
  }

  // Stores new location
  meOrientation = _eOrientation;
}

void Character::OnNewAnim(const orxSTRING _zOldAnim, const orxSTRING _zNewAnim, orxBOOL _bCut)
{
  //orxDEBUG_PRINT(orxDEBUG_LEVEL_ANIM, "[%s] -> [%s]%s", _zOldAnim, _zNewAnim, _bCut ? " <CUT>" : orxSTRING_EMPTY);

  // Pushes character section
  PushConfigSection();

  // Was attacking?
  if(!orxString_Compare(orxConfig_GetString(szAnimAttack), _zOldAnim))
  {
    // Updates status
    SetAttacking(orxFALSE);

    // Stops firing
    StopFire();

    // In air?
    if(GetLocation() == LocationInAir)
    {
      // Going to idle?
      if(!orxString_Compare(orxConfig_GetString(szAnimIdle), _zNewAnim))
      {
        // Updates animation
        SetAnim(IsFalling() ? szAnimFall : szAnimJump);
      }
    }
  }
  // Was dying
  else if(!orxString_Compare(orxConfig_GetString(szAnimDeath), _zOldAnim))
  {
    // Calls death callback
    OnDeath();
  }

  // Pops config section
  PopConfigSection();
}

void Character::OnAnimEvent(const orxSTRING _zAnim, const orxSTRING _zEvent, orxFLOAT _fTime, orxFLOAT _fValue)
{
  // Shoot?
  if(!orxString_Compare(_zEvent, "Shoot"))
  {
    // Starts firing
    StartFire();
  }
  // Foot step?
  else if(!orxString_Compare(_zEvent, "FootStep"))
  {
    // Adds foot step sound
    orxObject_AddSound((GetType() == TypePlayer) ? MushroomStew::GetInstance().GetCameraObject() : GetOrxObject(), szConfigFootStepSound);
  }
  // Dying?
  else if(!orxString_Compare(_zEvent, "Dying"))
  {
    // Adds foot step sound
    orxObject_AddSound((GetType() == TypePlayer) ? MushroomStew::GetInstance().GetCameraObject() : GetOrxObject(), szConfigDyingSound);
  }
}

orxBOOL Character::OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
  orxBOOL bStatic = orxTRUE, bResult = orxTRUE;

  // Has collider
  if(_poCollider)
  {
    // Pushes its section
    _poCollider->PushConfigSection();

    // Updates static status
    bStatic = orxConfig_GetBool(szConfigIsStatic);

    // Pops section
    _poCollider->PopConfigSection();
  }

  // Is static?
  if(bStatic)
  {
    // Is on ground?
    if(GetGround())
    {
      // Updates location
      SetLocation(LocationOnGround);
    }
  }

  // Done!
  return bResult;
}

orxBOOL Character::OnSeparate(ScrollObject *_poCollider)
{
  return orxTRUE;
}

void Character::SetLocation(Location _eLocation)
{
  orxASSERT(_eLocation < LocationNumber);
  
  // Calls children callback
  OnNewLocation(_eLocation);
}

void Character::SetOrientation(Orientation _eOrientation)
{
  orxASSERT(_eOrientation < OrientationNumber);
  
  // Calls children callback
  OnNewOrientation(_eOrientation);
}

void Character::ActivateInput(Input _eInput, orxBOOL _bActivate)
{
  // Checks
  orxASSERT(_eInput < InputNumber);

  // Activate?
  if(_bActivate)
  {
    // Updates flags
    SetFlags((ScrollObject::Flag)(1 << (_eInput + InputShift)));
  }
  else
  {
    // Updates flags
    SetFlags(ScrollObject::FlagNone, (ScrollObject::Flag)(1 << (_eInput + InputShift)));
  }
}

orxBOOL Character::IsInputActive(Input _eInput)
{
  // Checks
  orxASSERT(_eInput < InputNumber);

  // Done!
  return TestFlags((ScrollObject::Flag)(1 << (_eInput + InputShift)));
}

void Character::SetJumping(orxBOOL _bJumping)
{
  // Jumping?
  if(_bJumping)
  {
    // Updates animation
    SetAnim(szAnimJump, orxTRUE);

    // Stores it
    SetFlags((ScrollObject::Flag)FlagJumping);
  }
  else
  {
    // Updates animation
    SetAnim(szAnimLand, orxTRUE);

    // Stores it
    SetFlags(ScrollObject::FlagNone, (ScrollObject::Flag)FlagJumping);
  }
}

void Character::SetFalling(orxBOOL _bFalling)
{
  // Falling?
  if(_bFalling)
  {
    // Updates anim
    SetAnim(szAnimFall);

    // Not falling before?
    if(!IsFalling())
    {
      // Stores fall start
      orxVector_Copy(&mvFallStart, &mvPosition);
    }

    // Stores it
    SetFlags((ScrollObject::Flag)FlagFalling);
  }
  else
  {
    // Stores it
    SetFlags(ScrollObject::FlagNone, (ScrollObject::Flag)FlagFalling);
  }
}

void Character::SetAttacking(orxBOOL _bAttacking)
{
  // Attacking?
  if(_bAttacking)
  {
    // Sets attack anim
    SetAnim(szAnimAttack);

    // Stores it
    SetFlags((ScrollObject::Flag)FlagAttacking);
  }
  else
  {
    // Stores it
    SetFlags(ScrollObject::FlagNone, (ScrollObject::Flag)FlagAttacking);
  }
}

void Character::SetCrouching(orxBOOL _bCrouching)
{
  // Crouch?
  if(_bCrouching)
  {
    // Updates animation
    SetAnim(szAnimCrouched);

    // Stores it
    SetFlags((ScrollObject::Flag)FlagCrouching);
  }
  else
  {
    // Stores it
    SetFlags(ScrollObject::FlagNone, (ScrollObject::Flag)FlagCrouching);
  }
}

void Character::SetAnim(const orxSTRING _zAnimName, orxBOOL _bReplaceCurrentAnim)
{
  // Not dead?
  if(!IsDead())
  {
    // Sets character's anim
    SetOrxObjectAnim(GetOrxObject(), _zAnimName, _bReplaceCurrentAnim);

    // For all children
    for(orxOBJECT *pstChild = orxObject_GetOwnedChild(GetOrxObject());
        pstChild;
        pstChild = orxObject_GetOwnedSibling(pstChild))
    {
      // Sets its anim
      SetOrxObjectAnim(pstChild, _zAnimName, _bReplaceCurrentAnim);
    }
  }
}

void Character::CreateHealthBar()
{
  orxVECTOR vScale;

  // Creates bars
  mpstHealthBarBg = orxObject_CreateFromConfig("BlackPixel");
  mpstHealthBarFg = orxObject_CreateFromConfig("WhitePixel");

  // Checks
  orxSTRUCTURE_ASSERT(mpstHealthBarBg);
  orxSTRUCTURE_ASSERT(mpstHealthBarFg);

  // Pushes config section
  PushConfigSection();

  // Sets background bar scale
  orxObject_SetScale(mpstHealthBarBg, orxVector_Set(&vScale, orx2F(2.0f) + orxConfig_GetFloat(szConfigHealthBarLength), orx2F(5.0f), orxFLOAT_1));

  // Has blood color?
  if(orxConfig_HasValue(szConfigBloodColor))
  {
    orxCOLOR stColor;

    // Gets it
    orxVector_Mulf(&stColor.vRGB, orxConfig_GetVector(szConfigBloodColor, &stColor.vRGB), orxCOLOR_NORMALIZER);
    stColor.fAlpha = orxFLOAT_1;

    // Applies it to foreground bar
    orxObject_SetColor(mpstHealthBarFg, &stColor);
  }

  // Pops config section
  PopConfigSection();
}

void Character::DeleteHealthBar()
{
  // Deletes both bars
  if(mpstHealthBarBg)
  {
    orxObject_SetLifeTime(mpstHealthBarBg, orxFLOAT_0);
    mpstHealthBarBg = orxNULL;
  }
  if(mpstHealthBarFg)
  {
    orxObject_SetLifeTime(mpstHealthBarFg, orxFLOAT_0);
    mpstHealthBarFg = orxNULL;
  }
}

void Character::UpdateHealthBar()
{
  // Valid?
  if(mpstHealthBarBg && mpstHealthBarFg)
  {
    orxVECTOR vScale, vPosition, vPivot;
    orxFLOAT  fHealthBarLength;

    // Gets object position & pivot
    orxObject_GetWorldPosition(GetOrxObject(), &vPosition);
    orxObject_GetPivot(GetOrxObject(), &vPivot);

    // Pushes config section
    PushConfigSection();

    // Gets health bar length
    fHealthBarLength = orxConfig_GetFloat(szConfigHealthBarLength);

    // Pops config section
    PopConfigSection();

    // Sets background position
    vPosition.fX -= orx2F(0.5f) * fHealthBarLength + orxFLOAT_1;
    vPosition.fY -= vPivot.fY + orx2F(5.0f);
    vPosition.fX = orxMath_Floor(vPosition.fX);
    vPosition.fY = orxMath_Floor(vPosition.fY);
    orxObject_SetPosition(mpstHealthBarBg, &vPosition);

    // Sets foreground position
    vPosition.fX += orxFLOAT_1;
    vPosition.fY += orxFLOAT_1;
    vPosition.fZ -= orx2F(0.0001f);
    orxObject_SetPosition(mpstHealthBarFg, &vPosition);

    // Sets foreground bar scale
    orxVector_Set(&vScale, GetHealth() / GetMaxHealth() * fHealthBarLength, orx2F(3.0f), orxFLOAT_1);
    orxObject_SetScale(mpstHealthBarFg, &vScale);
  }
}

void Character::StartFire()
{
  // Has weapon?
  if(GetWeapon())
  {
    // Enables it
    orxSpawner_Enable(orxOBJECT_GET_STRUCTURE(GetWeapon(), SPAWNER), orxTRUE);
  }
}

void Character::StopFire()
{
  // Has weapon?
  if(GetWeapon())
  {
    // Disables it
    orxSpawner_Enable(orxOBJECT_GET_STRUCTURE(GetWeapon(), SPAWNER), orxFALSE);
  }
}

orxFLOAT Character::GetAcceleration() const
{
  orxFLOAT fResult = orxFLOAT_0;

  // Pushes config section
  PushConfigSection();

  // Depending on location
  switch(GetLocation())
  {
    case LocationOnGround:
    {
      ScrollObject *poGround;

      // Gets ground
      poGround = GetGround();

      // Valid?
      if(poGround)
      {
        orxBOOL bIsIce;

        // Updates ice status
        poGround->PushConfigSection();
        bIsIce = orxConfig_GetBool(szConfigIsIce);
        poGround->PopConfigSection();

        // Is ice?
        if(bIsIce)
        {
          // Gets acceleration
          fResult = orxConfig_GetFloat(szConfigIceAcceleration);
        }
        else
        {
          // Gets acceleration
          fResult = orxConfig_GetFloat(szConfigGroundAcceleration);
        }
      }
      else
      {
        // Gets acceleration
        fResult = orxConfig_GetFloat(szConfigGroundAcceleration);
      }

      break;
    }

    case LocationInAir:
    {
      // Gets acceleration
      fResult = orxConfig_GetFloat(szConfigAirAcceleration);

      break;
    }

    default:
    {
      // Error
      orxASSERT(orxFALSE && "Can't get acceleration for character <%s>, unknown location [%ld]", GetName(), meLocation);

      break;
    }
  }

  // Pops config section
  PopConfigSection();

  // Done!
  return fResult;
}

orxFLOAT Character::GetDeceleration() const
{
  orxFLOAT fResult = orxFLOAT_0;

  // Pushes config section
  PushConfigSection();

  // Depending on location
  switch(GetLocation())
  {
    case LocationOnGround:
    {
      ScrollObject *poGround;

      // Gets ground
      poGround = GetGround();

      // Valid?
      if(poGround)
      {
        orxBOOL bIsIce;

        // Updates ice status
        poGround->PushConfigSection();
        bIsIce = orxConfig_GetBool(szConfigIsIce);
        poGround->PopConfigSection();

        // Is ice?
        if(bIsIce)
        {
          // Gets acceleration
          fResult = orxConfig_GetFloat(szConfigIceDeceleration);
        }
        else
        {
          // Gets acceleration
          fResult = IsCrouching() ? orxConfig_GetFloat(szConfigCrouchDeceleration) : orxConfig_GetFloat(szConfigGroundDeceleration);
        }
      }
      else
      {
        // Gets acceleration
        fResult = IsCrouching() ? orxConfig_GetFloat(szConfigCrouchDeceleration) : orxConfig_GetFloat(szConfigGroundDeceleration);
      }

      break;
    }

    case LocationInAir:
    {
      // Gets acceleration
      fResult = orxConfig_GetFloat(szConfigAirDeceleration);

      break;
    }

    default:
    {
      // Error
      orxASSERT(orxFALSE && "Can't get deceleration for character <%s>, unknown location [%ld]", GetName(), meLocation);

      break;
    }
  }

  // Pops config section
  PopConfigSection();

  // Done!
  return fResult;
}

ScrollObject *Character::GetGround(orxVECTOR *_pvContact, orxVECTOR *_pvNormal, const orxVECTOR *_pvShift) const
{
  const orxSTRING zFrontAnchor;
  const orxSTRING zBackAnchor;
  orxVECTOR       vPos, vScale, vSize, vStart, vEnd;
  orxOBJECT      *pstObject;
  ScrollObject   *poResult = orxNULL;

  // Gets character's position, size & scale
  orxObject_GetWorldPosition(GetOrxObject(), &vPos);
  orxObject_GetWorldScale(GetOrxObject(), &vScale);
  orxVector_Mul(&vSize, orxObject_GetSize(GetOrxObject(), &vSize), &vScale);

  // Has a shift vector?
  if(_pvShift)
  {
    // Applies it
    orxVector_Add(&vPos, &vPos, _pvShift);
  }

  // Pushes character's section
  PushConfigSection();

  // Checks
  orxASSERT(orxConfig_HasValue(szConfigLeftAnchor));
  orxASSERT(orxConfig_HasValue(szConfigRightAnchor));

  // Gets anchor names
  if(GetInitialOrientation() == OrientationLeft)
  {
    zFrontAnchor  = szConfigLeftAnchor;
    zBackAnchor   = szConfigRightAnchor;
  }
  else
  {
    zFrontAnchor  = szConfigRightAnchor;
    zBackAnchor   = szConfigLeftAnchor;
  }

  // Gets front ray extremities
  orxVector_Add(&vStart, orxVector_Mul(&vStart, orxConfig_GetVector(zFrontAnchor, &vStart), &vScale), &vPos);
  orxVector_Set(&vEnd, vStart.fX, vStart.fY + vSize.fY, orxFLOAT_0);

  // Issues front raycast
  pstObject = orxObject_Raycast(&vStart, &vEnd, 0x0001, 0xFF00, orxFALSE, _pvContact, _pvNormal);

  // Found?
  if(pstObject)
  {
    // Updates result
    poResult = (ScrollObject *)orxObject_GetUserData(pstObject);
  }
  else
  {
    // Gets back ray extremities
    orxVector_Add(&vStart, orxVector_Mul(&vStart, orxConfig_GetVector(zBackAnchor, &vStart), &vScale), &vPos);
    orxVector_Set(&vEnd, vStart.fX, vStart.fY + vSize.fY, orxFLOAT_0);

    // Issues back raycast
    pstObject = orxObject_Raycast(&vStart, &vEnd, 0x0001, 0xFF00, orxFALSE, _pvContact, _pvNormal);

    // Found?
    if(pstObject)
    {
      // Updates result
      poResult = (ScrollObject *)orxObject_GetUserData(pstObject);
    }
  }

  // Pops config section
  PopConfigSection();

  // Found result?
  if(poResult)
  {
    // Asked for contact?
    if(_pvContact)
    {
      // Updates its X coordinate
      _pvContact->fX = GetFeetPosition().fX;
    }
  }

  // Done!
  return poResult;
}

ScrollObject *Character::GetWall(Orientation _eOrientation, orxVECTOR *_pvContact, orxVECTOR *_pvNormal) const
{
  orxVECTOR     vPos, vScale, vSize, vEnd;
  orxOBJECT    *pstObject;
  ScrollObject *poResult = orxNULL;

  // Gets character's position, size & scale
  orxObject_GetWorldPosition(GetOrxObject(), &vPos);
  orxObject_GetWorldScale(GetOrxObject(), &vScale);
  orxVector_Mul(&vSize, orxObject_GetSize(GetOrxObject(), &vSize), &vScale);

  // Gets ray end extremity
  orxVector_Set(&vEnd, (_eOrientation == OrientationLeft) ? vPos.fX - orx2F(0.5f) * orxMath_Abs(vSize.fX) : vPos.fX + orx2F(0.5f) * orxMath_Abs(vSize.fX), vPos.fY, orxFLOAT_0);

  // Issues raycast
  pstObject = orxObject_Raycast(&vPos, &vEnd, 0x0001, 0xFF00, orxFALSE, _pvContact, _pvNormal);

  // Found?
  if(pstObject)
  {
    // Updates result
    poResult = (ScrollObject *)orxObject_GetUserData(pstObject);

    // Asked for contact?
    if(_pvContact)
    {
      // Updates its X coordinate
      _pvContact->fY = vPos.fY;
    }
  }

  // Done!
  return poResult;
}

orxBOOL Character::IsSeen(const orxVECTOR &_rvTarget) const
{
  orxBOOL bResult = orxFALSE;

  // Is in vision range and cone?
  if(IsInVisionRange(_rvTarget) && IsInVisionCone(_rvTarget))
  {
    // Issues raycast against walls
    bResult = IsTraceable(_rvTarget);
  }

  // Done!
  return bResult;
}

orxBOOL Character::IsTraceable(const orxVECTOR &_rvTarget) const
{
  orxBOOL bResult = orxFALSE;

  // Issues raycast against walls
  bResult = orxObject_Raycast(&GetPosition(), &_rvTarget, 0x0001, 0xF800, orxTRUE, orxNULL, orxNULL) ? orxFALSE : orxTRUE;

  // Done!
  return bResult;
}

orxBOOL Character::IsInVisionRange(const orxVECTOR &_rvTarget) const
{
  orxBOOL bResult;

  // Updates result
  bResult = (orxVector_GetSquareDistance(&GetPosition(), &_rvTarget) <= mfVisionSquareDist) ? orxTRUE : orxFALSE;

  // Done!
  return bResult;
}

orxBOOL Character::IsInVisionCone(const orxVECTOR &_rvTarget) const
{
  orxBOOL   bResult;
  orxVECTOR vToTarget;

  // Gets normalized to target vector
  orxVector_Normalize(&vToTarget, orxVector_Sub(&vToTarget, &_rvTarget, &GetPosition()));

  // Updates result
  bResult = (orxVector_Dot(&GetSight(), &vToTarget) >= mfVisionCos) ? orxTRUE : orxFALSE;

  // Done!
  return bResult;
}

orxBOOL Character::IsInFront(const orxVECTOR &_rvTarget) const
{
  orxBOOL   bResult;
  orxVECTOR vToTarget;

  // Gets to target vector
  orxVector_Sub(&vToTarget, &_rvTarget, &GetPosition());

  // Updates result
  bResult = (orxVector_Dot(&GetSight(), &vToTarget) >= orxFLOAT_0) ? orxTRUE : orxFALSE;

  // Done!
  return bResult;
}

orxSTATUS Character::Damage(orxFLOAT _fDamage)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  // Valid?
  if(_fDamage >= orxFLOAT_0)
  {
    orxFLOAT fDamage;

    // Gets real damage after protection
    fDamage = orxCLAMP(_fDamage - mfProtection, orxFLOAT_0, mfHealth);

    // Should damage?
    if(fDamage > orxFLOAT_0)
    {
      // Applies it
      mfHealth -= fDamage;

      // Updates result
      eResult = orxSTATUS_SUCCESS;
    }
  }

  // Calls callback
  OnDamage();

  // Done!
  return eResult;
}

void Character::Kill()
{
  // Kills it
  mfHealth = orxFLOAT_0;
}
