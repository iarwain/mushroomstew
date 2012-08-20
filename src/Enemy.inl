//! Constants
static  const orxSTRING szConfigAlwaysAttack        = "AlwaysAttack";
static  const orxSTRING szConfigAttackDelay         = "AttackDelay";
static  const orxSTRING szConfigReactionAttackDelay = "ReactionAttackDelay";
static  const orxSTRING szConfigPatrolDelay         = "PatrolDelay";
static  const orxSTRING szConfigReorient            = "Reorient";
static  const orxSTRING szConfigHide                = "Hide";
static  const orxSTRING szConfigPatrol              = "Patrol";
static  const orxSTRING szConfigAllowFall           = "AllowFall";
static  const orxSTRING szConfigSeeCrouched         = "SeeCrouched";


//! Code
void Enemy::OnCreate()
{
  // Sets status
  orxConfig_SetBool(szConfigIsEnemy, orxTRUE);
  orxConfig_SetBool(szConfigIsPlayer, orxFALSE);

  // Calls parent method
  Character::OnCreate();

  // Inits variables
  mfAttackDelay   = orxFLOAT_0;
  mfPatrolDelay   = orxFLOAT_0;
  meDirection     = GetOrientation();
  mbForceReorient = orxFALSE;
}

void Enemy::OnDelete()
{
  // Calls parent method
  Character::OnDelete();
}

void Enemy::OnStartGame()
{
  // Registers self
  MushroomStew::GetInstance().RegisterEnemy(*this);

  // Calls parent method
  Character::OnStartGame();

  // Gets direction
  meDirection = GetOrientation();
}

void Enemy::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Calls parent method
  Character::Update(_rstInfo);

  // Not dead?
  if(!IsDead())
  {
    const Player *poPlayer;
    orxBOOL       mbPatrol = orxTRUE;

    // Gets player
    poPlayer = MushroomStew::GetInstance().GetPlayer();

    // Deactivates crouch input
    ActivateInput(InputCrouch, orxFALSE);

    // Deactivates move inputs
    ActivateInput(InputRunLeft, orxFALSE);
    ActivateInput(InputRunRight, orxFALSE);

    // Updates attack delay
    mfAttackDelay -= _rstInfo.fDT;

    // Player is alive and always attack or in sight?
    if(poPlayer && !poPlayer->IsDead() && (orxConfig_GetBool(szConfigAlwaysAttack) || (IsSeen(poPlayer->GetPosition()) && (!poPlayer->IsCrouching() || orxConfig_GetBool(szConfigSeeCrouched)))))
    {
      // Updates patrol delay & status
      mfPatrolDelay = orxConfig_GetFloat(szConfigPatrolDelay);
      mbPatrol = orxFALSE;

      // Can attack?
      if(mfAttackDelay <= orxFLOAT_0)
      {
        // Attack!
        ActivateInput(InputAttack);

        // Updates attack delay
        mfAttackDelay = orxConfig_GetFloat(szConfigAttackDelay);
      }
      else
      {
        // Don't attack
        ActivateInput(InputAttack, orxFALSE);
      }
    }
    else
    {
      // Don't attack
      ActivateInput(InputAttack, orxFALSE);

      // Can Hide?
      if(orxConfig_GetBool(szConfigHide))
      {
        // Crouch
        ActivateInput(InputCrouch);
      }

      // Should reorient?
      if(mbForceReorient)
      {
        // Updates patrol delay & status
        mfPatrolDelay = orxConfig_GetFloat(szConfigPatrolDelay);
        mbPatrol = orxFALSE;

        // Updates attack delay
        mfAttackDelay = orxConfig_GetFloat(szConfigReactionAttackDelay);
      }
    }

    // Should patrol?
    if(mbPatrol && (mfPatrolDelay <= orxFLOAT_0) && orxConfig_GetBool(szConfigPatrol))
    {
      orxVECTOR vPrediction;
      orxBOOL   bBackward = orxFALSE;

      // Gets anticipation vector
      orxVector_Set(&vPrediction, (meDirection == OrientationLeft) ? -orxConfig_GetFloat(szConfigPredictionTime) * orxMath_Abs(GetSpeed().fX): orxConfig_GetFloat(szConfigPredictionTime) * orxMath_Abs(GetSpeed().fX), orxFLOAT_0, orxFLOAT_0);

      // No ground or wall forward?
      if((!orxConfig_GetBool(szConfigAllowFall)
       && !GetGround(orxNULL, orxNULL, &vPrediction))
      || GetWall(meDirection))
      {
        // Turns back
        bBackward = orxTRUE;
      }

      // Updates input & direction
      if((meDirection == OrientationLeft) ^ bBackward)
      {
        // Move left
        ActivateInput(InputRunLeft);
        meDirection = OrientationLeft;
      }
      else
      {
        // Move right
        ActivateInput(InputRunRight);
        meDirection = OrientationRight;
      }
    }
    else
    {
      // Updates patrol delay
      mfPatrolDelay -= _rstInfo.fDT;

      // Can reorient and is behind us?
      if(poPlayer && IsBehind(poPlayer->GetPosition()) && (mbForceReorient || (orxConfig_GetBool(szConfigReorient) && IsInVisionRange(poPlayer->GetPosition()) && IsTraceable(poPlayer->GetPosition()))))
      {
        // Facing left?
        if(GetOrientation() == OrientationLeft)
        {
          // Turns to right
          SetOrientation(OrientationRight);
        }
        else
        {
          // Turns to left
          SetOrientation(OrientationLeft);
        }
      }

      // Clears force reorient
      mbForceReorient = orxFALSE;

      // Updated moving direction
      meDirection = GetOrientation();
    }
  }
}

void Enemy::OnDeath()
{
  // Unregisters self
  MushroomStew::GetInstance().UnregisterEnemy(*this);

  // Calls parent method
  Character::OnDeath();
}

void Enemy::OnDamage()
{
  // Pushes its section
  PushConfigSection();

  // Force reorient
  mbForceReorient = orxTRUE;

  // Pops config section
  PopConfigSection();

  // Updates its icon
  MushroomStew::GetInstance().UpdateEnemyIcon(*this);
}
