//! Code
void Missile::OnCreate()
{
  // Inits damage
  mfDamage = orxConfig_GetFloat(szConfigDamage);

  // Inits type
  orxConfig_SetBool(szConfigIsMissile, orxTRUE);

  // Clears owner
  mpoOwner = orxNULL;
}

void Missile::OnDelete()
{
}

void Missile::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Is homing?
  if(orxConfig_GetBool(szConfigIsHoming))
  {
    const Player *poPlayer;

    // Gets player
    poPlayer = MushroomStew::GetInstance().GetPlayer();

    // Valid?
    if(poPlayer)
    {
      orxFLOAT    fRotation, fDiff, fMaxRotation;
      orxVECTOR   vTarget, vPosition, vPlayerPosition, vSpeed;
      orxOBJECT  *pstOrxObject;

      // Gets orx object
      pstOrxObject = GetOrxObject();

      // Gets its position
      orxObject_GetWorldPosition(poPlayer->GetOrxObject(), &vPlayerPosition);

      // Gets our position
      orxObject_GetWorldPosition(pstOrxObject, &vPosition);

      // Gets our target vector
      orxVector_Sub(&vTarget, &vPlayerPosition, &vPosition);

      // Cartesian -> Spherical
      orxVector_FromCartesianToSpherical(&vTarget, &vTarget);

      // Gets our current spherical speed
      orxObject_GetSpeed(pstOrxObject, &vSpeed);
      orxVector_FromCartesianToSpherical(&vSpeed, &vSpeed);

      // Gets our angle diff
      fDiff = vTarget.fTheta - vSpeed.fTheta;

      // Clamps it (-> shorter turn)
      fDiff   = fDiff * (1.0f / orxMATH_KF_2_PI) + orx2F(0.5f);
      fDiff  -= orxS2F(orxF2S(fDiff) - (orxS32)(fDiff < orxFLOAT_0));
      fDiff   = (fDiff - orx2F(0.5f)) * orxMATH_KF_2_PI;

      // Computes max rotation
      fMaxRotation = _rstInfo.fDT * orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(szConfigAimVelocity);

      // Adjust angle diff
      fDiff = orxCLAMP(fDiff, -fMaxRotation, fMaxRotation);

      // Updates our speed
      vSpeed.fTheta += fDiff;

      // Applies cartesian speed
      orxVector_FromSphericalToCartesian(&vSpeed, &vSpeed);
      orxObject_SetSpeed(pstOrxObject, &vSpeed);

      // Gets our rotation
      fRotation = orxObject_GetRotation(pstOrxObject);

      // Updates rotation
      fRotation += fDiff;

      // Applies it
      orxObject_SetRotation(pstOrxObject, fRotation);
    }
  }
}

void Missile::SetOwner(Character *_poOwner)
{
  orxVECTOR vSpeed;

  // Stores it
  mpoOwner = _poOwner;

  // Pushes config section
  PushConfigSection();

  // Has owner?
  if(mpoOwner)
  {
    orxBOOL bImmune;

    // Owner's oriented to the left?
    if(_poOwner->GetOrientation() == Character::OrientationLeft)
    {
      // Rotates 180°
      orxObject_SetRotation(GetOrxObject(), orxObject_GetRotation(GetOrxObject()) + orxMATH_KF_PI);
    }

    // Pushes its section
    mpoOwner->PushConfigSection();

    // Gets its special immune status
    bImmune = orxConfig_GetBool(szConfigSpecialImmune);

    // Pops its config section
    mpoOwner->PopConfigSection();

    // Sets our immune status
    orxConfig_SetBool(szConfigSpecialImmune, bImmune);
  }

  // Has speed?
  if(orxConfig_HasValue("Speed"))
  {
    // Gets it
    orxConfig_GetVector("Speed", &vSpeed);

    // Updates it depending on owner's orientation
    vSpeed.fX = (GetOwner()->GetOrientation() == Character::OrientationLeft) ? -orxMath_Abs(vSpeed.fX) : orxMath_Abs(vSpeed.fX);

    // Applies it
    orxObject_SetSpeed(GetOrxObject(), &vSpeed);
  }

  // Pops config section
  PopConfigSection();
}

orxBOOL Missile::OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
  orxBOOL bResult = orxTRUE;

  // Has collider?
  if(_poCollider)
  {
    orxBOOL   bDelete = orxFALSE, bAltImpact = orxFALSE;
    orxVECTOR vScale;

    // Clears scale
    orxVector_Set(&vScale, orxFLOAT_1, orxFLOAT_1, orxFLOAT_1);

    // Pushes collider section
    _poCollider->PushConfigSection();

    // Is character?
    if(orxConfig_GetBool(szConfigIsCharacter))
    {
      Character *poCharacter;

      // Gets it
      poCharacter = ScrollCast<Character *>(_poCollider);

      // Is from opposite camp?
      if(poCharacter->GetType() != GetOwner()->GetType())
      {
        // Not dead?
        if(!poCharacter->IsDead())
        {
          // Pushes self config section
          PushConfigSection();

          // Not dodgeable or not on ground or not crouching?
          if(!orxConfig_GetBool(szConfigDodgeable)
          || (poCharacter->GetLocation() != Character::LocationOnGround)
          || (!poCharacter->IsCrouching()))
          {
            // Deals damage
            if(poCharacter->Damage(GetDamage()) == orxSTATUS_FAILURE)
            {
              // Alternate impact
              bAltImpact = orxTRUE;
            }

            // Gets scale
            orxObject_GetWorldScale(poCharacter->GetOrxObject(), &vScale);

            // Deletes missile
            bDelete = orxTRUE;
          }

          // Pops config section
          PopConfigSection();
        }
      }
    }
    // Is not special weapon?
    else if(!orxConfig_GetBool(szConfigIsSpecialWeapon))
    {
      // Deletes missile
      bDelete = orxTRUE;
    }

    // Should delete missile?
    if(bDelete)
    {
      // Creates impact
      CreateImpact(*_poCollider, _rvPosition, _rvNormal, vScale, bAltImpact);

      // Removes its visual and asks for deletion
      orxObject_UnlinkStructure(GetOrxObject(), orxSTRUCTURE_ID_GRAPHIC);
      orxObject_UnlinkStructure(GetOrxObject(), orxSTRUCTURE_ID_ANIMPOINTER);
      orxObject_SetLifeTime(GetOrxObject(), orxFLOAT_0);
    }

    // Pops config section
    _poCollider->PopConfigSection();
  }

  // Done!
  return bResult;
}

void Missile::CreateImpact(const ScrollObject &_roCollider, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal, const orxVECTOR &_rvScale, orxBOOL _bAltImpact) const
{
  orxOBJECT *pstObject;

  // Pushes missile section
  PushConfigSection();

  // Has impact object?
  if(orxConfig_HasValue(szConfigImpact))
  {
    const orxSTRING zImpactName;

    // Gets its name
    zImpactName = _bAltImpact ? szConfigAltImpact : orxConfig_GetString(szConfigImpact);

    // Pushes collider section
    _roCollider.PushConfigSection();

    // Has impact object?
    if(orxConfig_HasValue(zImpactName))
    {
      // Creates it
      pstObject = orxObject_CreateFromConfig(orxConfig_GetString(zImpactName));

      // Valid?
      if(pstObject)
      {
        orxVECTOR vSphericalNormal, vScale;
        orxCOLOR  stColor;
        orxBOOL   bHasColor = orxFALSE;

        // Has blood color?
        if(orxConfig_HasValue(szConfigBloodColor))
        {
          // Gets it
          orxVector_Mulf(&stColor.vRGB, orxConfig_GetVector(szConfigBloodColor, &stColor.vRGB), orxCOLOR_NORMALIZER);
          stColor.fAlpha = orxFLOAT_1;

          // Updates status
          bHasColor = orxTRUE;
        }

        // Pushes impact section
        orxConfig_PushSection(orxObject_GetName(pstObject));

        // Has collider a color?
        if(bHasColor)
        {
          // Is impact white?
          if(orxConfig_GetBool(szConfigIsWhite))
          {
            // Applies collider's color
            orxObject_SetColor(pstObject, &stColor);
          }
        }

        // Gets spherical normal
        orxVector_FromCartesianToSpherical(&vSphericalNormal, &_rvNormal);

        // Updates its position, rotation & scale
        orxObject_SetPosition(pstObject, &_rvPosition);
        orxObject_SetRotation(pstObject, vSphericalNormal.fTheta + orxMATH_KF_PI_BY_2 - orxConfig_GetFloat("Rotation"));
        orxObject_SetScale(pstObject, orxVector_Mul(&vScale, &_rvScale, orxObject_GetWorldScale(pstObject, &vScale)));

        // Pops config section
        orxConfig_PopSection();          
      }
    }

    // Pops config section
    _roCollider.PopConfigSection();
  }

  // Pops section
  PopConfigSection();
}
