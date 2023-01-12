#pragma once

#include <Engine/CScript.h>
#include "CCamTrigger.h"

enum class PLAYER_STATE
{
    IDLE,
    LOOKUP,
    CROUCH,
    FLIP,
    WALK,
    RUN,
    JUMP,
    FALL,
    DASH,
    DREAM_DASH,
    CLIME,
    DANGLING,
    CUTSCENE,
    DEAD,
    END
};

class CPhysics;

class CPlayerScript :
    public CScript
{
private:
    PLAYER_STATE    m_eState;
    PLAYER_STATE    m_ePrevState;

    STAGE           m_eStage;

    bool    m_bGround;
    bool    m_bWall;
    bool    m_bCollision;

    bool    m_bWalk;
    bool    m_bUseJump;
    bool    m_bUseDash;
    bool    m_bDash;
    bool    m_bClimb;
    bool    m_bClimbMove;
    bool    m_bFlip;
    float   m_fDanglingSec;     // ���� �پ����� �� �ִ� �ð�

    float         m_fSpeed;         // �ӷ�
    Vec2         m_vMaxSpeed;      // �ִ�ӷ�

    float         m_fDashTime;       // �뽬 �ð�
    Vec2         m_fDashDir;       // �뽬 �ð�
    Vec2          m_fDreamDashDir;   // DreamDash ����

    // collision
    CGameObject* m_pColBot;
    CGameObject* m_pColFront;
    CGameObject* m_pColBody;
    CGameObject* m_pCollisionObj;
     
    list<CGameObject*> m_lCurWall;
    list<CGameObject*> m_lCurGround;

    // Transition 
    CGameObject* m_pTransition;

    //Script
    CPhysics* m_pPhysics;

    // Bang Color
    Vec4    m_vNormalHairColor;
    Vec4    m_vDashHairColor;


// �⺻ �����Լ�
public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    // ĸ��
public:
    Vec4 GetNormalHairColor() { return m_vNormalHairColor; }
    Vec4 GetDashHairColor() { return m_vDashHairColor; }
    PLAYER_STATE GetPlayerState() { return m_eState; }
    PLAYER_STATE GetPrevPlayerState() { return m_ePrevState; }
    
    STAGE   GetCurStage() { return m_eStage; }
    void    SetCurStage(STAGE _eStage) { m_eStage = _eStage; }

    bool    UseDash() { return m_bUseDash; }

 // 1�����Ӹ��� �� ��
private:
    // update
    void KeyCheck();
    void PlayAnim();

public:
    void Dead();
    void Respwan();

// �̺�Ʈ
private:    
    virtual void OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName) override;
    virtual void OnChildCollision(CGameObject* _OtherObject, wstring _childName) override;
    virtual void OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName)override;

    CLONE(CPlayerScript)

public:
    CPlayerScript();
    ~CPlayerScript();

};

