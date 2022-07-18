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

    float   m_fDanglingSec;     // ���� �پ����� �� �ִ� �ð�

    // F = M * A
    // V += A * DT
    float         m_fSpeed;         // �ӷ�
    Vec2          m_vVelocity;      // �ӵ� ( �ӷ°� ���� )
    Vec2         m_fMaxSpeed;      // �ִ�ӷ�
    Vec2          m_vForce;         // �� ( ���� X ���ӵ� )
    Vec2          m_vAccel;         // ���ӵ�
    Vec2          m_vAccelA;        // �߰� ���ӵ�
    float         m_fMass;          // ����

    float         m_fFricCoeff;     // �������
    float         m_fGravity;       // �߷�

    // collision
    CGameObject* m_pColBot;
    CGameObject* m_pColFront;
    CGameObject* m_pColBody;
    CGameObject* m_pCollisionObj;

// �⺻ �����Լ�
public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    // ĸ��
private:
    float GetMess() { return m_fMass; }

    void SetMess(float _f) { m_fMass = _f; }
    void SetVelocity(Vec2 _v2) { m_vVelocity = _v2; }
    void SetMaxVelocity(Vec2 _f) { m_fMaxSpeed = _f; }
    void SetAccelAlpha(Vec2 _vAccel) { m_vAccelA = _vAccel; }

 // 1�����Ӹ��� �� ��
private:
    // update
    void KeyCheck();
    void PlayAnim();

    // lateupdate
    void CalPhysics();
    void CalGravity();
    
    // lateupdate->CalPhysics->Move
    void Move();

// ����
private:
    void AddForce(Vec2 _f) { m_vForce += _f; }

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

