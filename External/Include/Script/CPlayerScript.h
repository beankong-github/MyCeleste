#pragma once

#include <Engine/CScript.h>

enum class PLAYER_STATE
{
    IDLE,
    LOOKUP,
    CROUCH,
    FLIP,
    WALK,
    RUN,
    JUMP,
    DASH,
    CLIME,
    DANGLING,
    CUTSCENE,
    DEAD,
    END
};

class CPlayerScript :
    public CScript
{
private:
    PLAYER_STATE    m_eState;

    bool    m_bGround;
    bool    m_bUseDash;
    bool    m_bClimb;

    float   m_fDanglingSec;     // 벽에 붙어있을 수 있는 시간

    // F = M * A
    // V += A * DT
    float         m_fSpeed;         // 속력
    Vec2          m_vVelocity;      // 속도 ( 속력과 방향 )
    Vec2         m_fMaxSpeed;      // 최대속력
    Vec2          m_vForce;         // 힘 ( 질량 X 가속도 )
    Vec2          m_vAccel;         // 가속도
    Vec2          m_vAccelA;        // 추가 가속도
    float         m_fMass;          // 질량

    float         m_fFricCoeff;     // 마찰계수
    float         m_fGravity;       // 중력

    // collision
    CGameObject* m_pColBot;
    CGameObject* m_pColFront;
    CGameObject* m_pColBody;
    CGameObject* m_pCollisionObj;

// 기본 가상함수
public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    // 캡슐
private:
    float GetMess() { return m_fMass; }

    void SetMess(float _f) { m_fMass = _f; }
    void SetVelocity(Vec2 _v2) { m_vVelocity = _v2; }
    void SetMaxVelocity(Vec2 _f) { m_fMaxSpeed = _f; }
    void SetAccelAlpha(Vec2 _vAccel) { m_vAccelA = _vAccel; }

 // 1프레임마다 할 일
private:
    // update
    void KeyCheck();
    void PlayAnim();

    // lateupdate
    void CalPhysics();
    void CalGravity();
    
    // lateupdate->CalPhysics->Move
    void Move();

// 물리
private:
    void AddForce(Vec2 _f) { m_vForce += _f; }

// 이벤트
private:    
    virtual void OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName) override;
    virtual void OnChildCollision(CGameObject* _OtherObject, wstring _childName) override;
    virtual void OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName)override;

    CLONE(CPlayerScript)

public:
    CPlayerScript();
    ~CPlayerScript();

};

