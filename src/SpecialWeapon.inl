//! Code
void SpecialWeapon::OnCreate()
{
  // Inits variables
  mfDuration    = orxConfig_GetFloat(szConfigDuration);
  mfRefillRate  = orxConfig_GetFloat(szConfigRefillRate);
  mfCharge      = mfDuration;
  mbActivated   = orxFALSE;
  meType        = TypeTime;

  // Inits type
  orxConfig_SetBool(szConfigIsSpecialWeapon, orxTRUE);

  // Clears owner
  mpoOwner = orxNULL;

  // Updates associated shader
  orxObject_AddShader(GetOrxObject(), orxConfig_GetListString(szConfigChargeShaderList, (orxS32)meType));

  // Adds shrink FX
  orxObject_AddFX(GetOrxObject(), szConfigShrinkFX);
}

void SpecialWeapon::OnDelete()
{
}

void SpecialWeapon::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Is activated?
  if(mbActivated)
  {
    // Updates its charge
    mfCharge -= _rstInfo.fDT;

    // Empty?
    if(mfCharge <= orxFLOAT_0)
    {
      // Deactivates it
      Activate(orxFALSE);

      // Clears charge
      mfCharge = orxFLOAT_0;
    }
  }
  else
  {
    // Not fully charged?
    if(mfCharge < mfDuration)
    {
      // Updates it
      mfCharge += mfRefillRate * _rstInfo.fDT;

      // Clamps it
      mfCharge = orxMIN(mfCharge, mfDuration);
    }
  }
}

void SpecialWeapon::SetOwner(Character *_poOwner)
{
  // Stores it
  mpoOwner = _poOwner;

  // Sets it as parent
  orxObject_SetParent(GetOrxObject(), _poOwner ? _poOwner->GetOrxObject() : orxNULL);
}

void SpecialWeapon::Activate(orxBOOL _bActivate)
{
  // Changed status?
  if(_bActivate != mbActivated)
  {
    // Pushes config section
    PushConfigSection();

    // Stores status
    mbActivated = _bActivate;

    // Activated?
    if(_bActivate)
    {
      // Adds shader
      orxObject_AddShader(GetOrxObject(), orxConfig_GetListString(szConfigWeaponShaderList, (orxS32)meType));

      // Adds enlarge FX
      orxObject_AddFX(GetOrxObject(), szConfigEnlargeFX);

      // Adds on sound
      orxObject_AddSound(GetOrxObject(), orxConfig_GetString(szConfigOnSound));
    }
    else
    {
      // Removes shader
      orxObject_RemoveShader(GetOrxObject(), orxConfig_GetListString(szConfigWeaponShaderList, (orxS32)meType));

      // Adds shrink FX
      orxObject_AddFX(GetOrxObject(), szConfigShrinkFX);

      // Adds off sound
      orxObject_AddSound(GetOrxObject(), orxConfig_GetString(szConfigOffSound));
    }

    // Pops config section
    PopConfigSection();
  }
}

orxSTATUS SpecialWeapon::Switch()
{
  orxSTATUS eResult;

  // Not active?
  if(!IsActive())
  {
    orxU32 u32Type;

    // Disactivates it
    Activate(orxFALSE);

    // Pushes self config section
    PushConfigSection();

    // Removes associated shader
    orxObject_RemoveShader(GetOrxObject(), orxConfig_GetListString(szConfigWeaponShaderList, (orxS32)meType));
    orxObject_RemoveShader(GetOrxObject(), orxConfig_GetListString(szConfigChargeShaderList, (orxS32)meType));

    // Updates its type
    u32Type = (orxU32)meType;
    u32Type = (u32Type + 1) % (orxU32)TypeNumber;
    meType  = (Type)u32Type;

    // Updates associated shader
    orxObject_AddShader(GetOrxObject(), orxConfig_GetListString(szConfigChargeShaderList, (orxS32)meType));

    // Pops config section
    PopConfigSection();

    // Updates result
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    // Updates result
    eResult = orxSTATUS_FAILURE;
  }

  // Done!
  return eResult;
}

orxBOOL SpecialWeapon::OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
  orxBOOL bResult = orxTRUE;

  // Active?
  if(mbActivated)
  {
    // Has non-owner collider?
    if(_poCollider && _poCollider != mpoOwner)
    {
      // Pushes its section
      _poCollider->PushConfigSection();

      // Isn't immune or dead character?
      if(!orxConfig_GetBool(szConfigSpecialImmune)
      && (!orxConfig_GetBool(szConfigIsCharacter)
      || (!ScrollCast<Character *>(_poCollider)->IsDead())))
      {
        // Time weapon?
        if(meType == TypeTime)
        {
          orxCLOCK *pstClock;

          // Pushes config section
          PushConfigSection();

          // Creates clock
          pstClock = orxClock_CreateFromConfig(orxConfig_GetString(szConfigTimeClock));

          // Pops config section
          PopConfigSection();

          // Valid?
          if(pstClock)
          {
            // Sets it to collider
            orxObject_SetClock(_poCollider->GetOrxObject(), pstClock);
          }
        }
        else
        {
          orxVECTOR vGravity;

          // Pushed collider's instance section
          _poCollider->PushConfigSection(orxTRUE);

          // Backups its gravity
          if(orxObject_GetCustomGravity(_poCollider->GetOrxObject(), &vGravity))
          {
            orxConfig_SetVector(szConfigGravityBackup, &vGravity);
          }

          // Pops config section
          _poCollider->PopConfigSection();

          // Pushes config section
          PushConfigSection();

          // Use reverse gravity
          orxVector_Set(&vGravity, orxFLOAT_0, -orxConfig_GetFloat(szConfigGravityStrength), orxFLOAT_0);

          // Pops config section
          PopConfigSection();

          // Applies it
          orxObject_SetCustomGravity(_poCollider->GetOrxObject(), &vGravity);
        }
      }

      // Pops its config section
      _poCollider->PopConfigSection();
    }
  }

  // Done!
  return bResult;
}

orxBOOL SpecialWeapon::OnSeparate(ScrollObject *_poCollider)
{
  orxBOOL bResult = orxTRUE;

  // Has non-owner collider?
  if(_poCollider && _poCollider != mpoOwner)
  {
    // Time weapon?
    if(meType == TypeTime)
    {
      // Removes clock from the collider
      orxObject_SetClock(_poCollider->GetOrxObject(), orxNULL);
    }
    else
    {
      orxVECTOR vGravity;

      // Pushes its instance section
      _poCollider->PushConfigSection(orxTRUE);

      // Restores previous gravity
      orxObject_SetCustomGravity(_poCollider->GetOrxObject(), orxConfig_GetVector(szConfigGravityBackup, &vGravity));

      // Pops config section
      _poCollider->PopConfigSection();
    }
  }

  // Done!
  return bResult;
}
