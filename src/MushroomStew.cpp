//! Includes
//#define __NO_SCROLLED__ // Uncomment this define to prevent the embedded editor (ScrollEd) from being compiled
#define __SCROLL_IMPL__
#include "MushroomStew.h"
#undef __SCROLL_IMPL__


//! Inlined object classes
#include "Character.inl"
#include "Player.inl"
#include "Enemy.inl"
#include "EndBoss.inl"
#include "Missile.inl"
#include "SpecialWeapon.inl"


//! Constants
static  const orxSTRING szSaveGameFile              = "MushroomStew.sav";
static  const orxSTRING szInputQuit                 = "Quit";
static  const orxSTRING szInputScreenshot           = "Screenshot";
static  const orxSTRING szInputFullScreen           = "FullScreen";
static  const orxSTRING szInputPause                = "Pause";
static  const orxSTRING szInputNewGame              = "NewGame";
static  const orxSTRING szInputChallenge            = "Challenge";
static  const orxSTRING szInputContinue             = "Continue";
static  const orxSTRING szInputSetTitle             = "TitleInput";
static  const orxSTRING szInputSetEndLevel          = "EndLevelInput";
static  const orxSTRING szInputSetPause             = "PauseInput";
static  const orxSTRING szConfigSectionGame         = "Game";
static  const orxSTRING szConfigSectionSave         = "Save";
static  const orxSTRING szConfigSaveMapName         = "SaveMap";
static  const orxSTRING szConfigSaveScore           = "SaveScore";
static  const orxSTRING szConfigTitle               = "Title";
static  const orxSTRING szConfigPause               = "Pause";
static  const orxSTRING szConfigContinue            = "Continue";
static  const orxSTRING szConfigNewGame             = "NewGame";
static  const orxSTRING szConfigChallenge           = "Challenge";
static  const orxSTRING szConfigMapList             = "MapList";
static  const orxSTRING szConfigChallengeMapList    = "ChallengeMapList";
static  const orxSTRING szConfigPlayerList          = "PlayerList";
static  const orxSTRING szConfigEnemyList           = "EnemyList";
static  const orxSTRING szConfigEndBossList         = "EndBossList";
static  const orxSTRING szConfigMissileList         = "MissileList";
static  const orxSTRING szConfigSpecialWeaponList   = "SpecialWeaponList";
static  const orxSTRING szConfigShaderList          = "ShaderList";
static  const orxSTRING szConfigMusic               = "Music";
static  const orxSTRING szConfigEndLevel            = "EndLevel";
static  const orxSTRING szConfigSummary             = "Summary";
static  const orxSTRING szConfigEndLevelBonus       = "EndLevelBonus";
static  const orxSTRING szConfigLoadMap             = "LoadMap";
static  const orxSTRING szConfigCameraLimitTL       = "CameraLimitTL";
static  const orxSTRING szConfigCameraLimitBR       = "CameraLimitBR";
static  const orxSTRING szConfigCameraFreeZoneTL    = "CameraFreeZoneTL";
static  const orxSTRING szConfigCameraFreeZoneBR    = "CameraFreeZoneBR";
static  const orxSTRING szConfigCameraObject        = "CameraObject";
static  const orxSTRING szConfigCameraShakeFX       = "CameraShakeFX";
static  const orxSTRING szConfigCameraMiniShakeFX   = "MiniCameraShakeFX";
static  const orxSTRING szConfigIconDeathFX         = "IconDeathFX";
static  const orxSTRING szConfigIconDeathSound      = "IconDeathSound";
static  const orxSTRING szConfigRestartDelay        = "RestartDelay";
static  const orxSTRING szConfigStartSound          = "StartSound";
static  const orxSTRING szConfigIconPivot           = "IconPivot";
static  const orxSTRING szConfigIconOrigin          = "IconOrigin";
static  const orxSTRING szConfigIconSize            = "IconSize";
static  const orxSTRING szConfigLevelMaxBonusTime   = "LevelMaxBonusTime";
static  const orxSTRING szConfigTimeBonus           = "TimeBonus";
static  const orxSTRING szConfigHealthBonus         = "HealthBonus";
static  const orxSTRING szConfigPerfectHealthBonus  = "PerfectHealthBonus";
static  const orxSTRING szConfigPerfect             = "Perfect";
static  const orxSTRING szConfigSpecialWeaponBonus  = "SpecialWeaponBonus";


//! Code
static orxSTATUS orxFASTCALL EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Depending on event type
  switch(_pstEvent->eType)
  {
    // Spawner event
    case orxEVENT_TYPE_SPAWNER:
    {
      // Create?
      if(_pstEvent->eID == orxSPAWNER_EVENT_SPAWN)
      {
        ScrollObject *poObject;

        // Gets scroll object
        poObject = (ScrollObject *)orxObject_GetUserData(orxOBJECT(_pstEvent->hRecipient));

        // Valid?
        if(poObject)
        {
          // Is a Missile?
          poObject->PushConfigSection();
          if(orxConfig_GetBool(szConfigIsMissile))
          {
            orxOBJECT *pstOwner;

            // Get its owner
            pstOwner = orxOBJECT(orxObject_GetOwner(orxOBJECT(orxSpawner_GetOwner(orxSPAWNER(_pstEvent->hSender)))));

            // Valid?
            if(pstOwner)
            {
              Missile *poMissile;

              // Gets Missile
              poMissile = ScrollCast<Missile *>(poObject);

              // Stores its owner
              poMissile->SetOwner((Character *)orxObject_GetUserData(pstOwner));
            }
          }
          poObject->PopConfigSection();
        }
      }

      break;
    }

    // Object event
    case orxEVENT_TYPE_OBJECT:
    {
      if(_pstEvent->eID == orxOBJECT_EVENT_CREATE)
      {
        // Is continue text?
        if(!orxString_Compare(szConfigContinue, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          // Has save game or end level?
          if(MushroomStew::GetInstance().HasSaveGame() || MushroomStew::GetInstance().IsEndOfLevel())
          {
            orxINPUT_TYPE eType;
            orxENUM       eID;
            orxCHAR       acBuffer[1024];

            // Gets input binding
            orxInput_GetBinding(szInputContinue, 0, &eType, &eID);

            // Updates its content
            orxString_NPrint(acBuffer, 1024, orxLocale_GetString(szConfigContinue), orxInput_GetBindingName(eType, eID));

            // Sets it
            orxObject_SetTextString(orxOBJECT(_pstEvent->hSender), acBuffer);
          }
          else
          {
            // Deletes it
            orxObject_SetLifeTime(orxOBJECT(_pstEvent->hSender), orxFLOAT_0);
          }
        }
        // Is new game text?
        else if(!orxString_Compare(szConfigNewGame, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          orxINPUT_TYPE eType;
          orxENUM       eID;
          orxCHAR       acBuffer[1024];

          // Gets input binding
          orxInput_GetBinding(szInputNewGame, 0, &eType, &eID);

          // Updates its content
          orxString_NPrint(acBuffer, 1024, orxLocale_GetString(szConfigNewGame), orxInput_GetBindingName(eType, eID));

          // Sets it
          orxObject_SetTextString(orxOBJECT(_pstEvent->hSender), acBuffer);
        }
        // Is challenge text?
        else if(!orxString_Compare(szConfigChallenge, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          // Has challenge list
          if(MushroomStew::GetInstance().HasChallengeMapList())
          {
            orxINPUT_TYPE eType;
            orxENUM       eID;
            orxCHAR       acBuffer[1024];

            // Gets input binding
            orxInput_GetBinding(szInputChallenge, 0, &eType, &eID);

            // Updates its content
            orxString_NPrint(acBuffer, 1024, orxLocale_GetString(szConfigChallenge), orxInput_GetBindingName(eType, eID));

            // Sets it
            orxObject_SetTextString(orxOBJECT(_pstEvent->hSender), acBuffer);
          }
          else
          {
            // Deletes it
            orxObject_SetLifeTime(orxOBJECT(_pstEvent->hSender), orxFLOAT_0);
          }
        }
        // Is pause text?
        else if(!orxString_Compare(szConfigPause, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          orxINPUT_TYPE eTypePause, eTypeQuit, eTypeLeft, eTypeRight, eTypeJump, eTypeCrouch, eTypeAttack, eTypeSpecial, eTypeSwitch;
          orxENUM       eIDPause, eIDQuit, eIDLeft, eIDRight, eIDJump, eIDCrouch, eIDAttack, eIDSpecial, eIDSwitch;
          orxCHAR       acBuffer[1024];

          // Gets input bindings
          orxInput_GetBinding(szInputRunLeft, 0, &eTypeLeft, &eIDLeft);
          orxInput_GetBinding(szInputRunRight, 0, &eTypeRight, &eIDRight);
          orxInput_GetBinding(szInputJump, 0, &eTypeJump, &eIDJump);
          orxInput_GetBinding(szInputCrouch, 0, &eTypeCrouch, &eIDCrouch);
          orxInput_GetBinding(szInputAttack, 0, &eTypeAttack, &eIDAttack);
          orxInput_GetBinding(szInputSpecial, 0, &eTypeSpecial, &eIDSpecial);
          orxInput_GetBinding(szInputSwitchSpecial, 0, &eTypeSwitch, &eIDSwitch);

          // Selects pause input set
          orxInput_SelectSet(szInputSetPause);

          // Gets input bindings
          orxInput_GetBinding(szInputPause, 0, &eTypePause, &eIDPause);
          orxInput_GetBinding(szInputQuit, 0, &eTypeQuit, &eIDQuit);

          // Updates its content
          orxString_NPrint(acBuffer,
                           1024,
                           orxLocale_GetString(szConfigPause),
                           orxInput_GetBindingName(eTypePause, eIDPause),
                           orxInput_GetBindingName(eTypeQuit, eIDQuit),
                           orxInput_GetBindingName(eTypeLeft, eIDLeft),
                           orxInput_GetBindingName(eTypeRight, eIDRight),
                           orxInput_GetBindingName(eTypeJump, eIDJump),
                           orxInput_GetBindingName(eTypeCrouch, eIDCrouch),
                           orxInput_GetBindingName(eTypeAttack, eIDAttack),
                           orxInput_GetBindingName(eTypeSpecial, eIDSpecial),
                           orxInput_GetBindingName(eTypeSwitch, eIDSwitch));

          // Sets it
          orxObject_SetTextString(orxOBJECT(_pstEvent->hSender), acBuffer);
        }
        // Is summary text?
        else if(!orxString_Compare(szConfigSummary, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          orxCHAR acBuffer[1024];

          // Pushes map section
          orxConfig_PushSection(MushroomStew::GetInstance().GetMapShortName());

          // Updates its content
          orxString_NPrint(acBuffer, 1024, orxLocale_GetString(szConfigSummary), orxConfig_GetString(szConfigSummary));

          // Sets it
          orxObject_SetTextString(orxOBJECT(_pstEvent->hSender), acBuffer);

          // Pops config section
          orxConfig_PopSection();
        }
        // Is end level bonus text?
        else if(!orxString_Compare(szConfigEndLevelBonus, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          orxFLOAT  fTimeBonus, fHealthBonus, fSpecialWeaponBonus, fHealthRatio;
          orxCHAR   acBuffer[1024];

          // Gets game instance
          MushroomStew &roGame = MushroomStew::GetInstance();

          // Pushes game section
          orxConfig_PushSection(szConfigSectionGame);

          // Gets health ratio
          fHealthRatio = roGame.GetPlayer() ? (roGame.GetPlayer()->GetHealth() / roGame.GetPlayer()->GetMaxHealth()) : orxFLOAT_0;

          // Gets bonuses
          fTimeBonus          = (orxConfig_GetFloat(szConfigLevelMaxBonusTime) - roGame.GetLevelTime()) * orxConfig_GetFloat(szConfigTimeBonus);
          fTimeBonus          = orxMAX(fTimeBonus, orxFLOAT_0);
          fHealthBonus        = (fHealthRatio == orxFLOAT_1) ? orxConfig_GetFloat(szConfigPerfectHealthBonus) : fHealthRatio * orxConfig_GetFloat(szConfigHealthBonus);
          fSpecialWeaponBonus = (roGame.GetPlayer() && roGame.GetPlayer()->GetSpecialWeapon()) ? roGame.GetPlayer()->GetSpecialWeapon()->GetChargeLevel() * orxConfig_GetFloat(szConfigSpecialWeaponBonus) : orxFLOAT_0;

          // Updates score
          roGame.AddScore(orxF2S(fTimeBonus) + orxF2S(fHealthBonus) + orxF2S(fSpecialWeaponBonus));

          // Updates its content
          orxString_NPrint(acBuffer, 1024, orxLocale_GetString(szConfigEndLevelBonus), orxF2S(fTimeBonus), orxF2S(fHealthBonus), (fHealthRatio == orxFLOAT_1) ? orxConfig_GetString(szConfigPerfect) : orxSTRING_EMPTY, orxF2S(fSpecialWeaponBonus));

          // Sets it
          orxObject_SetTextString(orxOBJECT(_pstEvent->hSender), acBuffer);

          // Pops config section
          orxConfig_PopSection();
        }
      }
      // Delete
      else if(_pstEvent->eID == orxOBJECT_EVENT_DELETE)
      {
        // Is title object?
        if(!orxString_Compare(szConfigTitle, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          // Launches game
          MushroomStew::GetInstance().LaunchGame();
        }
        // Is end level object?
        else if(!orxString_Compare(szConfigEndLevel, orxObject_GetName(orxOBJECT(_pstEvent->hSender))))
        {
          // Asks for immediate restart
          MushroomStew::GetInstance().RestartMap(orxFLOAT_0);
        }
      }

      break;
    }

    // Shader event
    case orxEVENT_TYPE_SHADER:
    {
      orxSHADER_EVENT_PAYLOAD *pstPayload;

      /* Gets its payload */
      pstPayload = (orxSHADER_EVENT_PAYLOAD *)_pstEvent->pstPayload;

      /* Time? */
      if(!orxString_Compare(pstPayload->zParamName, "Time"))
      {
        /* Updates its value */
        pstPayload->fValue = orxClock_GetInfo(orxClock_FindFirst(orx2F(-1.0f), orxCLOCK_TYPE_CORE))->fTime;
      }
      /* Camera position? */
      else if(!orxString_Compare(pstPayload->zParamName, "CameraPos"))
      {
        /* Updates its value */
        orxObject_GetWorldPosition(MushroomStew::GetInstance().GetCameraObject(), &pstPayload->vValue);
      }
      /* Object position? */
      else if(!orxString_Compare(pstPayload->zParamName, "ObjectPos"))
      {
        orxOBJECT *pstObject;

        /* Is sender an object? */
        if((pstObject = orxOBJECT(_pstEvent->hSender)))
        {
          /* Updates its value */
          orxObject_GetWorldPosition(pstObject, &pstPayload->vValue);
        }
      }
      /* Object scale? */
      else if(!orxString_Compare(pstPayload->zParamName, "ObjectScale"))
      {
        orxOBJECT *pstObject;

        /* Is sender an object? */
        if((pstObject = orxOBJECT(_pstEvent->hSender)))
        {
          /* Updates its value */
          orxObject_GetScale(pstObject, &pstPayload->vValue);
        }
      }
      /* Special weapon charge? */
      else if(!orxString_Compare(pstPayload->zParamName, "SpecialWeaponCharge"))
      {
        const Player *poPlayer;

        // Gets player
        poPlayer = MushroomStew::GetInstance().GetPlayer();

        // Valid?
        if(poPlayer)
        {
          SpecialWeapon *poSpecialWeapon;

          // Gets special weapon
          poSpecialWeapon = poPlayer->GetSpecialWeapon();

          // Valid?
          if(poSpecialWeapon)
          {
            // Gets its charge
            pstPayload->fValue = poSpecialWeapon->GetChargeLevel();
          }
        }
      }

      break;
    }

    default:
    {
      break;
    }
  }

  // Done!
  return eResult;
}

static orxBOOL orxFASTCALL SaveGameFilter(const orxSTRING _zSectionName, const orxSTRING _zKeyName, const orxSTRING _zFileName, orxBOOL _bUseEncryption)
{
  orxBOOL bResult = orxFALSE;

  // Is save section?
  if(!orxString_Compare(_zSectionName, szConfigSectionSave))
  {
    // Is current map or score?
    if(!_zKeyName || !orxString_Compare(_zKeyName, szConfigSaveMapName) || !orxString_Compare(_zKeyName, szConfigSaveScore))
    {
      // Updates result
      bResult = orxTRUE;
    }
  }

  // Done!
  return bResult;
}

orxSTATUS MushroomStew::Init()
{
  orxSTATUS eResult;

  // Creates camera object
  mpstCameraObject = orxObject_CreateFromConfig(szConfigCameraObject);

  // Valid?
  if(mpstCameraObject)
  {
    orxVECTOR   vCameraHalfSize;
    orxAABOX    stFrustum;

    // Gets camera frustum
    orxCamera_GetFrustum(GetMainCamera(), &stFrustum);

    // Gets its half size
    orxVector_Mulf(&vCameraHalfSize, orxVector_Sub(&vCameraHalfSize, &stFrustum.vBR, &stFrustum.vTL) , orx2F(0.5f));

    // Clears variables
    mpoPlayer             = orxNULL;
    mpstTitleObject       = orxNULL;
    mpstEndLevelObject    = orxNULL;
    mpstPauseObject       = orxNULL;
    mpstScoreObject       = orxNULL;
    mpstMusic             = orxNULL;
    mu32MapIndex          = 0;
    ms32EnemyCounter      = 0;
    ms32Score             = 0;
    mfTime                = orxFLOAT_0;
    mfLevelStartTime      = orxFLOAT_0;
    mfRestartDelay        = orxFLOAT_0;
    mbRestartMap          = orxFALSE;
    mbMapLoaded           = orxFALSE;
    mbLoadSaveGame        = orxFALSE;
    mbChallengeMode       = orxFALSE;
    mbAllowSaveGame       = orxFALSE;
    mbHasSaveGame         = orxFALSE;
    mbHasChallengeMapList = orxFALSE;
    mbEndOfLevel          = orxFALSE;
    mbEndOfGame           = orxFALSE;
    mpstEnemyTable        = orxHashTable_Create(64, orxHASHTABLE_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);
    orxVector_Set(&mvIconPosition, vCameraHalfSize.fX, -vCameraHalfSize.fY, orx2F(0.0001f));

    // Register handler
    eResult = ((orxEvent_AddHandler(orxEVENT_TYPE_SPAWNER, EventHandler) != orxSTATUS_FAILURE)
            && (orxEvent_AddHandler(orxEVENT_TYPE_OBJECT, EventHandler) != orxSTATUS_FAILURE)
            && (orxEvent_AddHandler(orxEVENT_TYPE_SHADER, EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

    // Not in editor?
    if(!ScrollBase::IsEditorMode())
    {
      // Loads save game
      if(orxConfig_Load(szSaveGameFile) != orxSTATUS_FAILURE)
      {
        // Updates status
        mbHasSaveGame = orxTRUE;
      }

      // Pushes game section
      orxConfig_PushSection(szConfigSectionGame);

      // Has challenge map lise?
      mbHasChallengeMapList = orxConfig_HasValue(szConfigChallengeMapList);

      // Pops config section
      orxConfig_PopSection();

      // Loads title screen
      LoadTitleScreen();

      // Allows save game
      mbAllowSaveGame = orxTRUE;
    }
  }
  else
  {
    // Updates result
    eResult = orxSTATUS_FAILURE;
  }

  // Done!
  return eResult;
}

orxSTATUS MushroomStew::Run()
{
  // Do your run code here
  if(orxInput_IsActive(szInputQuit))
  {
    return orxSTATUS_FAILURE;
  }
  else
  {
    return orxSTATUS_SUCCESS;
  }
}

void MushroomStew::Exit()
{
  // Do your exit code here
  orxEvent_RemoveHandler(orxEVENT_TYPE_SPAWNER, EventHandler);
  orxEvent_RemoveHandler(orxEVENT_TYPE_SHADER, EventHandler);

  // Deletes enemy table
  orxHashTable_Delete(mpstEnemyTable);
}

void MushroomStew::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Should pause?
  if(orxInput_IsActive(szInputPause) && orxInput_HasNewStatus(szInputPause))
  {
    // Toggles pause
    PauseGame(!IsGamePaused());
  }

  // Not paused?
  if(!IsGamePaused())
  {
    // Updates time
    mfTime = _rstInfo.fTime;
  }

  // Has music?
  if(mpstMusic)
  {
    // End of level?
    if(IsEndOfLevel())
    {
      // Deletes it
      orxSound_Delete(mpstMusic);
      mpstMusic = orxNULL;
    }
    else
    {
      // Stopped
      if(orxSound_GetStatus(mpstMusic) == orxSOUND_STATUS_STOP)
      {
        // Plays it
        orxSound_Play(mpstMusic);
      }
    }
  }

  // Should take a screenshot?
  if(orxInput_IsActive(szInputScreenshot) && orxInput_HasNewStatus(szInputScreenshot))
  {
    // Captures it
    orxScreenshot_Capture();
  }

  // Should toggle full screen?
  if(orxInput_IsActive(szInputFullScreen) && orxInput_HasNewStatus(szInputFullScreen))
  {
    // Toggles full screen mode
    orxDisplay_SetFullScreen(!orxDisplay_IsFullScreen());
  }

  // Map not loaded yet?
  if(!mbMapLoaded)
  {
    orxBOOL bStart = orxFALSE;

    // Is new game input active?
    if(orxInput_IsActive(szInputNewGame) && orxInput_HasNewStatus(szInputNewGame))
    {
      // Starts without loading
      mbLoadSaveGame      = orxFALSE;
      mbChallengeMode     = orxFALSE;
      bStart              = orxTRUE;
    }
    // Has challenge map list and is challenge input active?
    else if(HasChallengeMapList() && orxInput_IsActive(szInputChallenge) && orxInput_HasNewStatus(szInputChallenge))
    {
      // Starts without loading
      mbLoadSaveGame      = orxFALSE;
      mbChallengeMode     = orxTRUE;
      mbAllowSaveGame     = orxFALSE;
      bStart              = orxTRUE;
    }
    // Continue?
    else if(orxInput_IsActive(szInputContinue) && orxInput_HasNewStatus(szInputContinue))
    {
      // End of level?
      if(IsEndOfLevel())
      {
        // Starts
        bStart = orxTRUE;
      }
      // Has savegame and is continue input active?
      else if(HasSaveGame())
      {
        // Starts with loading
        mbLoadSaveGame      = orxTRUE;
        mbChallengeMode     = orxFALSE;
        bStart              = orxTRUE;
      }
    }

    // Should start?
    if(bStart)
    {
      // Has title?
      if(mpstTitleObject)
      {
        // Deletes title object
        orxObject_SetLifeTime(mpstTitleObject, orxFLOAT_0);
        mpstTitleObject = orxNULL;
      }
      // Has end level?
      else if(mpstEndLevelObject)
      {
        // Deletes end level object
        orxObject_SetLifeTime(mpstEndLevelObject, orxFLOAT_0);
        mpstEndLevelObject = orxNULL;

        // Updates status
        mbEndOfLevel = orxFALSE;

        // Selects game input set
        orxInput_SelectSet(MushroomStew::szInputSetGame);
      }
    }
  }
  else
  {
    // Has score display
    if(mpstScoreObject)
    {
      orxCHAR acBuffer[256];

      // Updates score
      orxString_NPrint(acBuffer, 256, orxLocale_GetString(szConfigScore), ms32Score);

      // Updates it
      orxObject_SetTextString(mpstScoreObject, acBuffer);
    }

    // All enemies are dead?
    if(!ms32EnemyCounter)
    {
      // Loads next map
      if(LoadNextMap() == orxSTATUS_FAILURE)
      {
        // End of game
        mbEndOfGame = orxTRUE;
      }
    }
  }

  // Should restart map?
  if(mbRestartMap)
  {
    // Pauses game
    PauseGame(orxTRUE);

    // Decreases delay
    mfRestartDelay -= _rstInfo.fDT;

    // Now?
    if(mfRestartDelay <= orxFLOAT_0)
    {
      // Stops game
      StopGame();

      // Loads map
      LoadMap();

      // Unpauses game
      PauseGame(orxFALSE);

      // Starts game
      StartGame();

      // Clears restart status
      mbRestartMap = orxFALSE;
    }
  }
}

void MushroomStew::CameraUpdate(const orxCLOCK_INFO &_rstInfo)
{
  // Has player?
  if(mpoPlayer)
  {
    orxVECTOR vPlayerPosition;

    // Gets player position
    orxVector_Copy(&vPlayerPosition, &mpoPlayer->GetPosition());

    // Valid?
    if(!orxVector_IsNull(&vPlayerPosition))
    {
      orxVECTOR vPosition, vTarget;

      // Gets camera position
      orxObject_GetWorldPosition(mpstCameraObject, &vPosition);

      // Gets target vector
      orxVector_Sub(&vTarget, &vPlayerPosition, &vPosition);
      vTarget.fZ = orxFLOAT_0;

      // Should move right?
      if(vTarget.fX > mstCameraFreeZone.vBR.fX)
      {
        vTarget.fX -= mstCameraFreeZone.vBR.fX;
      }
      // Should move left?
      else if(vTarget.fX < mstCameraFreeZone.vTL.fX)
      {
        vTarget.fX -= mstCameraFreeZone.vTL.fX;
      }
      // Don't move
      else
      {
        vTarget.fX = orxFLOAT_0;
      }

      // Should move down?
      if(vTarget.fY > mstCameraFreeZone.vBR.fY)
      {
        vTarget.fY -= mstCameraFreeZone.vBR.fY;
      }
      // Should move up?
      else if(vTarget.fY < mstCameraFreeZone.vTL.fY)
      {
        vTarget.fY -= mstCameraFreeZone.vTL.fY;
      }
      // Don't move
      else
      {
        vTarget.fY = orxFLOAT_0;
      }

      // Applies move
      orxVector_Add(&vPosition, &vPosition, &vTarget);

      // Clamps it with limits
      orxVector_Clamp(&vPosition, &vPosition, &mstCameraLimit.vTL, &mstCameraLimit.vBR);

      // Updates camera
      orxObject_SetPosition(mpstCameraObject, &vPosition);
    }
  }
}

void MushroomStew::BindObjects()
{
  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // For all players
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigPlayerList); i < u32Number; i++)
  {
    // Binds it
    ScrollBindObject<Player>(orxConfig_GetListString(szConfigPlayerList, i));
  }

  // For all enemies
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigEnemyList); i < u32Number; i++)
  {
    // Binds it
    ScrollBindObject<Enemy>(orxConfig_GetListString(szConfigEnemyList, i));
  }

  // For all end bosses (!)
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigEndBossList); i < u32Number; i++)
  {
    // Binds it
    ScrollBindObject<EndBoss>(orxConfig_GetListString(szConfigEndBossList, i));
  }

  // For all missiles
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigMissileList); i < u32Number; i++)
  {
    // Binds it
    ScrollBindObject<Missile>(orxConfig_GetListString(szConfigMissileList, i));
  }

  // For all special weapons
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigSpecialWeaponList); i < u32Number; i++)
  {
    // Binds it
    ScrollBindObject<SpecialWeapon>(orxConfig_GetListString(szConfigSpecialWeaponList, i));
  }

  // Pops section
  orxConfig_PopSection();
}

void MushroomStew::RegisterPlayer(Player &_roPlayer)
{
  // Has alreay a player?
  if(mpoPlayer)
  {
    // Deletes it
    DeleteObject(mpoPlayer);
  }

  // Stores new player
  mpoPlayer = &_roPlayer;
}

void MushroomStew::UnregisterPlayer(Player &_roPlayer)
{
  // as the registered one?
  if(mpoPlayer == &_roPlayer)
  {
    // Removes it
    mpoPlayer = orxNULL;
  }
}

const Player *MushroomStew::GetPlayer() const
{
  return mpoPlayer;
}

void MushroomStew::RegisterEnemy(Character &_roEnemy)
{
  orxOBJECT *pstIcon;

  // Pushes its section
  _roEnemy.PushConfigSection();

  // Creates its icon
  pstIcon = orxObject_CreateFromConfig(orxConfig_GetString(szConfigIcon));

  // Valid?
  if(pstIcon)
  {
    orxVECTOR vPivot, vOrigin, vSize, vPos;

    // Gets its pivot, origin & size
    orxObject_GetPivot(pstIcon, &vPivot);
    orxObject_GetOrigin(pstIcon, &vOrigin);
    orxObject_GetSize(pstIcon, &vSize);

    // Pushes its section
    orxConfig_PushSection(orxObject_GetName(pstIcon));

    // Stores pivot, origin & size
    orxConfig_SetVector(szConfigIconPivot, &vPivot);
    orxConfig_SetVector(szConfigIconOrigin, &vOrigin);
    orxConfig_SetVector(szConfigIconSize, &vSize);

    // Pops config section
    orxConfig_PopSection();

    // Updates icon position
    mvIconPosition.fX -= vSize.fX;

    // Computes its position
    orxVector_Add(&vPos, &mvIconPosition, &vPivot);

    // Sets it
    orxObject_SetPosition(pstIcon, &vPos);

    // Sets it as a child of camera object
    orxObject_SetParent(pstIcon, GetCameraObject());
  }

  // Pops config section
  _roEnemy.PopConfigSection();

  // Adds it to table
  orxHashTable_Add(mpstEnemyTable, orxString_ToCRC(_roEnemy.GetName()), (void *)pstIcon);

  // Updates enemy counter
  ms32EnemyCounter++;
}

void MushroomStew::UnregisterEnemy(Character &_roEnemy)
{
  // Updates enemy counter
  ms32EnemyCounter--;

  // Updates its icon status
  UpdateEnemyIcon(_roEnemy);

  // Is enemy dead?
  if(_roEnemy.IsDead())
  {
    // Pushes its section
    _roEnemy.PushConfigSection();

    // Updates score
    AddScore(orxConfig_GetS32(szConfigScore));

    // Pops config section
    _roEnemy.PopConfigSection();
  }
}

void MushroomStew::UpdateEnemyIcon(const Character &_roEnemy)
 {
  orxOBJECT *pstIcon;

  // Gets its icon
  pstIcon = orxOBJECT(orxHashTable_Get(mpstEnemyTable, orxString_ToCRC(_roEnemy.GetName())));

  // Valid?
  if(pstIcon)
  {
    orxGRAPHIC *pstGraphic;

    // Gets its graphic object
    pstGraphic = orxOBJECT_GET_STRUCTURE(pstIcon, GRAPHIC);

    // Valid?
    if(pstGraphic)
    {
      orxFLOAT  fHealthRatio, fShift;
      orxVECTOR vPivot, vSize, vOrigin;

      // Gets enemy's health ratio
      fHealthRatio = _roEnemy.GetHealth() / _roEnemy.GetMaxHealth();

      // Pushes its section
      orxConfig_PushSection(orxObject_GetName(pstIcon));

      // Gets its original pivot, origin & size
      orxConfig_GetVector(szConfigIconPivot, &vPivot);
      orxConfig_GetVector(szConfigIconOrigin, &vOrigin);
      orxConfig_GetVector(szConfigIconSize, &vSize);

      // Pops config section
      orxConfig_PopSection();

      // Gets shift value
      fShift = orxMath_Ceil(vSize.fY * (orxFLOAT_1 - fHealthRatio));

      // Updates pivot, origin & size according to health
      vPivot.fY  -= fShift;
      vOrigin.fY += fShift;
      vSize.fY   -= fShift;

      // Applies them
      orxGraphic_SetPivot(pstGraphic, &vPivot);
      orxGraphic_SetOrigin(pstGraphic, &vOrigin);
      orxGraphic_SetSize(pstGraphic, &vSize);

      // Is enemy dead?
      if(_roEnemy.IsDead())
      {
        // Applies icon death FX & sound
        orxObject_AddFX(pstIcon, szConfigIconDeathFX);
        orxObject_AddSound(pstIcon, szConfigIconDeathSound);
      }
    }
  }
}

void MushroomStew::ShakeCamera(orxBOOL _bMini)
{
  // Applies shake FX on camera object
  orxObject_AddFX(mpstCameraObject, _bMini ? szConfigCameraMiniShakeFX : szConfigCameraShakeFX);
}

void MushroomStew::RestartMap(orxFLOAT _fDelay)
{
  // End of game?
  if(mbEndOfGame)
  {
    // Quit
    orxInput_SetValue(szInputQuit, orxFLOAT_1);
  }
  else
  {
    // Asks for map restart
    mbRestartMap = orxTRUE;

    // No given delay?
    if(_fDelay < orxFLOAT_0)
    {
      // Pushes game section
      orxConfig_PushSection(szConfigSectionGame);

      // Gets restart delay
      mfRestartDelay = orxConfig_GetFloat(szConfigRestartDelay);

      // Pops config section
      orxConfig_PopSection();
    }
    else
    {
      // Stores its delay
      mfRestartDelay = _fDelay;
    }
  }
}

orxSTATUS MushroomStew::LoadNextMap()
{
  orxU32    u32MapIndex;
  orxSTATUS eResult;

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Gets next map
  u32MapIndex = (mu32MapIndex + 1) % (mbChallengeMode ? orxConfig_GetListCounter(szConfigChallengeMapList) : orxConfig_GetListCounter(szConfigMapList));

  // Updates status
  mbMapLoaded = orxFALSE;

  // Selects end level input set
  orxInput_SelectSet(szInputSetEndLevel);

  // Updates status
  mbEndOfLevel = orxTRUE;

  // Creates end level object
  mpstEndLevelObject = orxObject_CreateFromConfig(szConfigEndLevel);

  // Valid?
  if(u32MapIndex)
  {
    // Sets it
    SetMapName(orxConfig_GetListString(mbChallengeMode ? szConfigChallengeMapList : szConfigMapList, u32MapIndex));

    // Stores its index
    mu32MapIndex = u32MapIndex;

    // Saves game
    SaveGame();
  }

  // Has end level object?
  if(mpstEndLevelObject)
  {
    // Pauses game
    PauseGame(orxTRUE);
  }
  // Has valid map?
  else if(u32MapIndex)
  {
    // Updates status
    mbEndOfLevel = orxFALSE;

    // Asks for immediate restart
    RestartMap(orxFLOAT_0);
  }

  // Pops config section
  orxConfig_PopSection();

  // Updates result
  eResult = u32MapIndex ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

  // Done!
  return eResult;
}

void MushroomStew::SaveGame()
{
  // Is allowed?
  if(mbAllowSaveGame)
  {
    // Pushes config section
    orxConfig_PushSection(szConfigSectionSave);

    // Stores current map
    orxConfig_SetString(szConfigSaveMapName, GetMapName());

    // Stores score
    orxConfig_SetS32(szConfigSaveScore, ms32Score);

    // Saves it
    orxConfig_Save(szSaveGameFile, orxTRUE, SaveGameFilter);

    // Pops config section
    orxConfig_PopSection();
  }
}

void MushroomStew::LoadGame()
{
  // Pushes save section
  orxConfig_PushSection(szConfigSectionSave);

  // Has saved map?
  if(orxConfig_HasValue(szConfigSaveMapName))
  {
    const orxSTRING zMapName;

    // Gets map name
    zMapName = orxConfig_GetString(szConfigSaveMapName);

    // Pushes game section
    orxConfig_PushSection(szConfigSectionGame);

    // For all maps
    for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigMapList); i < u32Number; i++)
    {
      // Found?
      if(orxString_SearchString(orxConfig_GetListString(szConfigMapList, i), zMapName))
      {
        // Updates map index
        mu32MapIndex = i;
        break;
      }
    }

    // Pops config section
    orxConfig_PopSection();

    // Loads score
    ms32Score = orxConfig_GetS32(szConfigSaveScore);
  }

  // Pops config section
  orxConfig_PopSection();
}

orxBOOL MushroomStew::HasSaveGame() const
{
  // Done!
  return mbHasSaveGame;
}

orxBOOL MushroomStew::HasChallengeMapList() const
{
  // Done!
  return mbHasChallengeMapList;
}

orxBOOL MushroomStew::IsEndOfLevel() const
{
  return mbEndOfLevel;
}

void MushroomStew::LoadTitleScreen()
{
  // Selects title input set
  orxInput_SelectSet(szInputSetTitle);

  // Creates title object
  mpstTitleObject = orxObject_CreateFromConfig(szConfigTitle);

  // Failed?
  if(!mpstTitleObject)
  {
    // Launches game
    LaunchGame();
  }
}

void MushroomStew::LaunchGame()
{
  // Clears title object reference
  mpstTitleObject = orxNULL;

  // Selects game input set
  orxInput_SelectSet(szInputSetGame);

  // Should load save game?
  if(mbLoadSaveGame)
  {
    // Loads it
    LoadGame();
  }

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Sets map name
  SetMapName(orxConfig_GetListString(mbChallengeMode ? szConfigChallengeMapList : szConfigMapList, mu32MapIndex));

  // Pops config section
  orxConfig_PopSection();

  // Restarts map
  RestartMap(orxFLOAT_0);
}

void MushroomStew::AddScore(orxS32 _s32Value)
{
  // Updates score
  ms32Score = orxMAX(ms32Score + _s32Value, 0);

  // Has score display
  if(mpstScoreObject)
  {
    orxCHAR acBuffer[256];

    // Updates score
    orxString_NPrint(acBuffer, 256, orxLocale_GetString(szConfigScore), ms32Score);

    // Updates it
    orxObject_SetTextString(mpstScoreObject, acBuffer);
  }
}

void MushroomStew::OnStartGame()
{
  // Has loaded map?
  if(mbMapLoaded)
  {
    orxVECTOR   vPos, vCameraHalfSize;
    orxOBJECT  *pstObject;
    orxAABOX    stFrustum;

    // Gets camera frustum
    orxCamera_GetFrustum(GetMainCamera(), &stFrustum);

    // Gets its half size
    orxVector_Mulf(&vCameraHalfSize, orxVector_Sub(&vCameraHalfSize, &stFrustum.vBR, &stFrustum.vTL) , orx2F(0.5f));

    // Pushes game section
    orxConfig_PushSection(szConfigSectionGame);

    // Gets free zone
    orxConfig_GetVector(szConfigCameraFreeZoneTL, &mstCameraFreeZone.vTL);
    orxConfig_GetVector(szConfigCameraFreeZoneBR, &mstCameraFreeZone.vBR);

    // Pops config section
    orxConfig_PopSection();

    // Pushes map's section
    orxConfig_PushSection(GetMapShortName());

    // Gets camera limit
    orxConfig_GetVector(szConfigCameraLimitTL, &mstCameraLimit.vTL);
    orxConfig_GetVector(szConfigCameraLimitBR, &mstCameraLimit.vBR);

    // Updates world limit
    orxVector_Sub(&mstWorldLimit.vTL, &mstCameraLimit.vTL, &vCameraHalfSize);
    orxVector_Add(&mstWorldLimit.vBR, &mstCameraLimit.vBR, &vCameraHalfSize);

    // Gets camera's position
    orxCamera_GetPosition(GetMainCamera(), &vPos);

    // Clears it
    vPos.fX = vPos.fY = orxFLOAT_0;

    // Adds camera as child
    orxCamera_SetParent(GetMainCamera(), mpstCameraObject);

    // Clears camera's position
    orxCamera_SetPosition(GetMainCamera(), &orxVECTOR_0);

    // Updates camera object's position
    orxObject_SetPosition(mpstCameraObject, &vPos);

    // For all defined shaders
    for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigShaderList);
        i < u32Number;
        i++)
    {
      // Adds it
      orxViewport_AddShader(GetMainViewport(), orxConfig_GetListString(szConfigShaderList, i));
    }

    // Creates music
    mpstMusic = orxSound_CreateFromConfig(orxConfig_GetString(szConfigMusic));

    // Valid?
    if(mpstMusic)
    {
      // Plays it
      orxSound_Play(mpstMusic);
    }

    // Creates score object
    mpstScoreObject = orxObject_CreateFromConfig(szConfigScore);

    // For all objects
    for(pstObject = orxOBJECT(orxStructure_GetFirst(orxSTRUCTURE_ID_OBJECT));
        pstObject;
        pstObject = orxOBJECT(orxStructure_GetNext(pstObject)))
    {
      // Pushes its section
      orxConfig_PushSection(orxObject_GetName(pstObject));

      // Is static?
      if(orxConfig_GetBool(szConfigIsStatic))
      {
        orxVECTOR vPos;

        // Removes its Y axis differential scrolling
        orxStructure_SetFlags(orxOBJECT_GET_STRUCTURE(pstObject, FRAME), orxFRAME_KU32_FLAG_NONE, orxFRAME_KU32_FLAG_SCROLL_Y);

        // Is in background?
        if(orxObject_GetWorldPosition(pstObject, &vPos)->fZ > orxFLOAT_0)
        {
          // Actives smoothing
          orxObject_SetSmoothing(pstObject, orxDISPLAY_SMOOTHING_ON);
        }
      }

      // Is invisible?
      if(orxConfig_GetBool(szConfigIsInvisible))
      {
        // Removes its graphic & animation pointer
        orxObject_UnlinkStructure(pstObject, orxSTRUCTURE_ID_GRAPHIC);
        orxObject_UnlinkStructure(pstObject, orxSTRUCTURE_ID_ANIMPOINTER);
      }

      // Pops config section
      orxConfig_PopSection();
    }

    // Challenge mode?
    if(mbChallengeMode)
    {
      // Adds text
      pstObject = orxObject_CreateFromConfig(szConfigLoadMap);

      // Valid?
      if(pstObject)
      {
        // Updates its content
        orxObject_SetTextString(pstObject, GetMapShortName());
      }
    }

    // Adds start sound
    orxObject_AddSound(GetCameraObject(), szConfigStartSound);

    // Gets level start time
    mfLevelStartTime = mfTime;

    // Pops config section
    orxConfig_PopSection();
  }
}

void MushroomStew::OnStopGame()
{
  orxOBJECT  *pstIcon;
  orxU32      u32EnemyKey;
  orxHANDLE   hID;
  orxVECTOR   vCameraHalfSize;
  orxAABOX    stFrustum;

  // Gets camera frustum
  orxCamera_GetFrustum(GetMainCamera(), &stFrustum);

  // Gets its half size
  orxVector_Mulf(&vCameraHalfSize, orxVector_Sub(&vCameraHalfSize, &stFrustum.vBR, &stFrustum.vTL) , orx2F(0.5f));

  // Unlinks camera
  orxCamera_SetParent(GetMainCamera(), orxNULL);

  // Pushes current map's section
  orxConfig_PushSection(GetMapShortName());

  // For all defined shaders
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter(szConfigShaderList);
      i < u32Number;
      i++)
  {
    // Removes it
    orxViewport_RemoveShader(GetMainViewport(), orxConfig_GetListString(szConfigShaderList, i));
  }

  // Has music?
  if(mpstMusic)
  {
    // Removes it
    orxSound_Delete(mpstMusic);
    mpstMusic = orxNULL;
  }

  // Has score object?
  if(mpstScoreObject)
  {
    // Deletes it
    orxObject_Delete(mpstScoreObject);
    mpstScoreObject = orxNULL;
  }

  // For all registered icons
  for(hID = orxHashTable_GetNext(mpstEnemyTable, orxNULL, &u32EnemyKey, (void **)&pstIcon);
      hID != orxHANDLE_UNDEFINED;
      hID = orxHashTable_GetNext(mpstEnemyTable, hID, &u32EnemyKey, (void **)&pstIcon))
  {
    // Valid?
    if(pstIcon)
    {
      // Deletes it
      orxObject_Delete(pstIcon);

      // Removes it from table
      orxHashTable_Remove(mpstEnemyTable, u32EnemyKey);
    }
  }

  // Resets icon position
  orxVector_Set(&mvIconPosition, vCameraHalfSize.fX, -vCameraHalfSize.fY, orx2F(0.0001f));

  // Clears enemy counter
  ms32EnemyCounter = 0;

  // Pops config section
  orxConfig_PopSection();
}

void MushroomStew::OnPauseGame(orxBOOL _bPause)
{
  // Not restarting or end of level?
  if(!mbRestartMap && !IsEndOfLevel())
  {
    // Should pause?
    if(_bPause)
    {
      // Has music?
      if(mpstMusic)
      {
        // Updates it
        orxSound_Pause(mpstMusic);
      }

      // Creates pause object
      mpstPauseObject = orxObject_CreateFromConfig(szConfigPause);
    }
    else
    {
      // Selects game input set
      orxInput_SelectSet(szInputSetGame);

      // Has music?
      if(mpstMusic)
      {
        // Updates it
        orxSound_Play(mpstMusic);
      }

      // Has pause object?
      if(mpstPauseObject)
      {
        // Deletes it
        orxObject_SetLifeTime(mpstPauseObject, orxFLOAT_0);
        mpstPauseObject = orxNULL;
      }
    }
  }
}

void MushroomStew::OnMapLoad()
{
  // Updates status
  mbMapLoaded = orxTRUE;
}


//! Main section
int main(int argc, char **argv)
{
  // Executes our game
  MushroomStew::GetInstance().Execute(argc, argv);

  return EXIT_SUCCESS;
}

#ifdef __orxMSVC__

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  // Executes our game
  MushroomStew::GetInstance().Execute();

  return EXIT_SUCCESS;
}

#endif // __orxMSVC__
