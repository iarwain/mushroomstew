//! Constants
static  const orxSTRING szInputRunLeft              = "RunLeft";
static  const orxSTRING szInputRunRight             = "RunRight";
static  const orxSTRING szInputRun                  = "Run";
static  const orxSTRING szInputCrouch               = "Crouch";
static  const orxSTRING szInputJump                 = "Jump";
static  const orxSTRING szInputCrouchAxis           = "CrouchAxis";
static  const orxSTRING szInputAttack               = "Attack";
static  const orxSTRING szInputSpecial              = "Special";
static  const orxSTRING szInputSwitchSpecial        = "SwitchSpecial";

static  const orxSTRING szConfigDeathObject         = "DeathObject";


//! Code
void Player::OnCreate()
{
  // Clears variables
  mpstDeathObject = orxNULL;

  // Sets status
  orxConfig_SetBool(szConfigIsPlayer, orxTRUE);
  orxConfig_SetBool(szConfigIsEnemy, orxFALSE);

  // Calls parent funtion
  Character::OnCreate();

  // Registers self
  MushroomStew::GetInstance().RegisterPlayer(*this);
}

void Player::OnDelete()
{
  // Unregisters self
  MushroomStew::GetInstance().UnregisterPlayer(*this);

  // Has death object?
  if(mpstDeathObject)
  {
    // Releases it
    orxObject_SetLifeTime(mpstDeathObject, orxFLOAT_0);
    mpstDeathObject = orxNULL;
  }

  // Calls parent method
  Character::OnDelete();
}

void Player::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Not dead?
  if(!IsDead())
  {
    // Updates listener position
    orxSoundSystem_SetListenerPosition(&GetFeetPosition());

    // Maps lasting inputs to character
    if(orxInput_IsActive(szInputRun))
    {
      ActivateInput(InputRunLeft, orxInput_GetValue(szInputRun) < orxFLOAT_0);
      ActivateInput(InputRunRight,orxInput_GetValue(szInputRun) > orxFLOAT_0);
    }
    else
    {
      ActivateInput(InputRunLeft, orxInput_IsActive(szInputRunLeft));
      ActivateInput(InputRunRight, orxInput_IsActive(szInputRunRight));
    }
    if(orxInput_IsActive(szInputCrouchAxis))
    {
      ActivateInput(InputRunLeft, orxInput_GetValue(szInputCrouchAxis) < orxFLOAT_0);
    }
    else
    {
      ActivateInput(InputCrouch, orxInput_IsActive(szInputCrouch));
    }

    // Maps instant inputs to character
    ActivateInput(InputJump, orxInput_IsActive(szInputJump) && orxInput_HasNewStatus(szInputJump));
    ActivateInput(InputAttack, orxInput_IsActive(szInputAttack) && orxInput_HasNewStatus(szInputAttack));
    ActivateInput(InputSpecial, orxInput_IsActive(szInputSpecial) && orxInput_HasNewStatus(szInputSpecial));
    ActivateInput(InputSwitchSpecial, orxInput_IsActive(szInputSwitchSpecial) && orxInput_HasNewStatus(szInputSwitchSpecial));

    // Calls parent method
    Character::Update(_rstInfo);
  }
}

void Player::OnDeath()
{
  // Calls parent method
  Character::OnDeath();

  // Gets game instance
  MushroomStew &roGame = MushroomStew::GetInstance();

  // Reloads game
  roGame.LoadGame();

  // Restarts map
  roGame.RestartMap();

  // Has death object?
  if(orxConfig_HasValue(szConfigDeathObject))
  {
    orxVECTOR vPos;

    // Creates it
    mpstDeathObject = orxObject_CreateFromConfig(orxConfig_GetString(szConfigDeathObject));

    // Valid?
    if(mpstDeathObject)
    {
      // No ground?
      if(!GetGround(&vPos))
      {
        // Gets feet position
        orxVector_Copy(&vPos, &GetFeetPosition());
      }

      // Makes sure it's slightly in front
      vPos.fZ -= 0.000001f;

      // Updates death object's position
      orxObject_SetPosition(mpstDeathObject, &vPos);
    }
  }
}
