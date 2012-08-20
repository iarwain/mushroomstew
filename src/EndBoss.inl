//! Constants
static  const orxSTRING szConfigWaveList            = "WaveList";
static  const orxSTRING szConfigWaveDelay           = "WaveDelay";
static  const orxSTRING szConfigSpell               = "WZSpell";
static  const orxSTRING szConfigWizardEnlargeFX     = "WZEnlarge";
static  const orxSTRING szConfigWizardShrinkFX      = "WZShrink";
static  const orxSTRING szConfigWizardDeathFX       = "WZDeath";
static  const orxSTRING szConfigWizardShrinkSound   = "CollapseSound";


//! Code
void EndBoss::OnCreate()
{
  // Sets status
  orxConfig_SetBool(szConfigIsEndBoss, orxTRUE);
  orxConfig_SetBool(szConfigIsPlayer, orxFALSE);

  // Inits variables
  ms32WaveCounter = 0;
  ms32WaveNumber  = orxConfig_GetListCounter(szConfigWaveList);
  mfWaveDelay     = orxFLOAT_0;
  mpstSlotBank    = orxBank_Create(16, sizeof(orxVECTOR), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);
  mpoSpell        = orxNULL;

  // Calls parent method
  Character::OnCreate();
}

void EndBoss::OnDelete()
{
  // Deletes slot bank
  orxBank_Delete(mpstSlotBank);
  mpstSlotBank = orxNULL;

  // Calls parent method
  Character::OnDelete();
}

void EndBoss::OnStartGame()
{
  // Registers self
  MushroomStew::GetInstance().RegisterEnemy(*this);

  // Calls parent method
  Character::OnStartGame();

  // Creates slot list
  CreateSlotList();

  // For all wave numbers
  for(orxS32 i = 0; i < ms32WaveNumber; i++)
  {
    // Add enlarge fx
    orxObject_AddFX(GetOrxObject(), szConfigWizardEnlargeFX);
  }
}

void EndBoss::OnStopGame()
{
  // Has spell?
  if(mpoSpell)
  {
    // Removes it
    orxObject_SetLifeTime(mpoSpell->GetOrxObject(), orxFLOAT_0);
    mpoSpell = orxNULL;
  }

  // Deletes slot list
  DeleteSlotList();
}

void EndBoss::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Calls parent method
  Character::Update(_rstInfo);

  // Not dead?
  if(!IsDead())
  {
    // No minions left?
    if(MushroomStew::GetInstance().GetEnemyCounter() == 1)
    {
      // Has spell?
      if(mpoSpell)
      {
        // Updates its animation
        orxObject_SetTargetAnim(mpoSpell->GetOrxObject(), "WZSDisappear");

        // Prepares it for deletion
        orxObject_SetLifeTime(mpoSpell->GetOrxObject(), orx2F(0.16f));
        mpoSpell = orxNULL;

        // Disables protection
        EnableProtection(orxFALSE);
      }

      // Updates wave delay
      mfWaveDelay -= _rstInfo.fDT;

      // Should spawn?
      if(mfWaveDelay <= orxFLOAT_0)
      {
        // Play spell animation
        SetAnim(szAnimAttack);

        // Enables protection
        EnableProtection();

        // Reinits wave delay
        mfWaveDelay = orxConfig_GetFloat(szConfigWaveDelay);
      }
    }
    else
    {
      // Enables protection
      EnableProtection();
    }
  }
}

void EndBoss::OnAnimEvent(const orxSTRING _zAnim, const orxSTRING _zEvent, orxFLOAT _fTime, orxFLOAT _fValue)
{
  // Spawn?
  if(!orxString_Compare(_zEvent, "Spawn"))
  {
    // Spawns wave
    SpawnWave();
  }
  // Dying?
  else if(!orxString_Compare(_zEvent, "Die"))
  {
    // Adds death FX
    orxObject_AddFX(GetOrxObject(), szConfigWizardDeathFX);

    // Removes body
    orxObject_UnlinkStructure(GetOrxObject(), orxSTRUCTURE_ID_BODY);
  }
}

void EndBoss::OnDeath()
{
  // Unregisters self
  MushroomStew::GetInstance().UnregisterEnemy(*this);

  // Calls parent method
  Character::OnDeath();
}

void EndBoss::OnDamage()
{
  // Updates its icon
  MushroomStew::GetInstance().UpdateEnemyIcon(*this);

  // Should change wave?
  if(orxFLOAT_1 - (GetHealth() / GetMaxHealth()) >= orxS2F(ms32WaveCounter + 1) / orxS2F(ms32WaveNumber))
  {
    // Updates wave counter
    ms32WaveCounter++;

    // Adds shrink FX
    orxObject_AddFX(GetOrxObject(), szConfigWizardShrinkFX);

    // Plays shrink sound
    orxObject_AddSound(GetOrxObject(), szConfigWizardShrinkSound);

    // Spawns now
    mfWaveDelay = orxFLOAT_0;
  }
}

void EndBoss::CreateSlotList()
{
  // Gets game instance
  MushroomStew &roGame = MushroomStew::GetInstance();

  // For all objects
  for(ScrollObject *poObject = roGame.GetNextObject();
      poObject;
      poObject = roGame.GetNextObject(poObject))
  {
    // Pushes its section
    poObject->PushConfigSection();

    // Is a slot?
    if(orxConfig_GetBool(szConfigIsSlot))
    {
      // Adds it to our list
      orxObject_GetWorldPosition(poObject->GetOrxObject(), (orxVECTOR *)orxBank_Allocate(mpstSlotBank));
    }

    // Pops config section
    poObject->PopConfigSection();
  }
}

void EndBoss::DeleteSlotList()
{
  // Clears slot bank
  orxBank_Clear(mpstSlotBank);
}

void EndBoss::SpawnWave()
{
  const orxSTRING zWaveName;
  orxVECTOR      *pvSlotPosition;

  // Gets game instance
  MushroomStew &roGame = MushroomStew::GetInstance();

  // Checks
  orxASSERT(ms32WaveCounter < ms32WaveNumber);

  // Pushes self config section
  PushConfigSection();

  // Gets wave name
  zWaveName = orxConfig_GetListString(szConfigWaveList, ms32WaveCounter);

  // Gets first slot
  pvSlotPosition = (orxVECTOR *)orxBank_GetNext(mpstSlotBank, orxNULL);

  // For all minions to spawn
  for(orxS32 i = 0, s32Number = orxConfig_GetListCounter(zWaveName); i < s32Number; i++)
  {
    ScrollObject *poMinion;

    // Spawns it
    poMinion = roGame.CreateObject(orxConfig_GetListString(zWaveName, i));

    // Valid?
    if(poMinion)
    {
      // Valid slot?
      if(pvSlotPosition)
      {
        // Updates minion's position
        orxObject_SetPosition(poMinion->GetOrxObject(), pvSlotPosition);

        // Gets next slot position
        pvSlotPosition = (orxVECTOR *)orxBank_GetNext(mpstSlotBank, (void *)pvSlotPosition);
      }
      else
      {
        // Sets it to our position
        orxObject_SetPosition(poMinion->GetOrxObject(), &GetPosition());
      }
    }
  }

  // Creates spell
  mpoSpell = roGame.CreateObject(szConfigSpell);

  // Valid?
  if(mpoSpell)
  {
    // Sets us as parent
    orxObject_SetParent(mpoSpell->GetOrxObject(), GetOrxObject());
  }

  // Pops config section
  PopConfigSection();
}

void EndBoss::EnableProtection(orxBOOL _bEnable)
{
  // Should enable?
  if(_bEnable)
  {
    // Updates protection
    SetProtection(orx2F(1000.0f));
  }
  else
  {
    // Updates protection
    SetProtection(orxFLOAT_0);
  }
}
