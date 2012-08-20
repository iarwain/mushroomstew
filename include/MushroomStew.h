#ifndef __MUSHROOMSTEW_H__
#define __MUSHROOMSTEW_H__

//! Includes
#include "Scroll.h"
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "EndBoss.h"
#include "Missile.h"
#include "SpecialWeapon.h"


//! MushroomStew class
class MushroomStew : public Scroll<MushroomStew>
{
public:

                void            RegisterPlayer(Player &_roPlayer);
                void            UnregisterPlayer(Player &_roPlayer);

          const Player *        GetPlayer() const;

                void            RegisterEnemy(Character &_roEnemy);
                void            UnregisterEnemy(Character &_roEnemy);
                void            UpdateEnemyIcon(const Character &_roEnemy);

                orxFLOAT        GetTime() const {return mfTime;}
                orxFLOAT        GetLevelTime() const {return mfTime - mfLevelStartTime;}
                orxOBJECT *     GetCameraObject() const {return mpstCameraObject;}
                orxS32          GetEnemyCounter() const {return ms32EnemyCounter;}

                const orxAABOX &GetWorldLimit() const {return mstWorldLimit;}

                void            ShakeCamera(orxBOOL _bMini = orxFALSE);

                void            RestartMap(orxFLOAT _fDelay = orx2F(-1.0f));
                orxSTATUS       LoadNextMap();

                void            SaveGame();
                void            LoadGame();
                orxBOOL         HasSaveGame() const;

                orxBOOL         HasChallengeMapList() const;

                orxBOOL         IsEndOfLevel() const;

                void            LoadTitleScreen();
                void            LaunchGame();

                void            AddScore(orxS32 _s32Value);


private:

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            Update(const orxCLOCK_INFO &_rstInfo);
                void            CameraUpdate(const orxCLOCK_INFO &_rstInfo);
                void            BindObjects();

                void            OnStartGame();
                void            OnStopGame();
                void            OnPauseGame(orxBOOL _bPause);
                void            OnMapLoad();

                orxAABOX        mstWorldLimit;
                orxAABOX        mstCameraLimit;
                orxAABOX        mstCameraFreeZone;
                Player *        mpoPlayer;
                orxOBJECT *     mpstCameraObject;
                orxOBJECT *     mpstTitleObject;
                orxOBJECT *     mpstEndLevelObject;
                orxOBJECT *     mpstPauseObject;
                orxOBJECT *     mpstScoreObject;
                orxSOUND *      mpstMusic;
                orxVECTOR       mvIconPosition;
                orxU32          mu32MapIndex;
                orxS32          ms32EnemyCounter;
                orxS32          ms32Score;
                orxHASHTABLE *  mpstEnemyTable;
                orxFLOAT        mfTime;
                orxFLOAT        mfLevelStartTime;
                orxFLOAT        mfRestartDelay;
                orxBOOL         mbRestartMap;
                orxBOOL         mbMapLoaded;
                orxBOOL         mbLoadSaveGame;
                orxBOOL         mbChallengeMode;
                orxBOOL         mbAllowSaveGame;
                orxBOOL         mbHasSaveGame;
                orxBOOL         mbHasChallengeMapList;
                orxBOOL         mbEndOfLevel;
                orxBOOL         mbEndOfGame;
};

#endif // __MUSHROOMSTEW_H__
