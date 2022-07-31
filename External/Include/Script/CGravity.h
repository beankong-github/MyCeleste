#pragma once
#include <Engine/CScript.h>

class CGravity :
    public CScript
{
private:
    Vec2    m_vCurForce;
    Vec2    m_vVelocity;

    Vec2    m_vGravityForce;

    float   m_fSpeed; // �ӷ�  
    float   m_fMass;  // ����    

     // �ִ� �ӷ�(����)
    float   m_fMaxXSpeed;
    float   m_fMaxYSpeed;


    float   m_fFriction; // ������


    bool    m_bGravityActive; // �߷� ��� ����
    bool    m_bGround;  // ������Ʈ�� ������ �ִ��� ���߿� ���ִ��� üũ



public:
    void SetMass(float _fMass) { m_fMass = _fMass; }
    float GetMass() { return m_fMass; }

    void SetSpeed(float _fSpeed)
    {
        m_vVelocity.Normalize();
        m_vVelocity *= _fSpeed;
    }
    float GetSpeed() { return m_fSpeed; }

    float GetMaxXSpeed() { return m_fMaxXSpeed; }
    float GetMaxYSpeed() { return m_fMaxYSpeed; }

    void SetMaxXSpeed(float _fMaxSpeed) { m_fMaxXSpeed = fabs(_fMaxSpeed); }
    void SetMaxYSpeed(float _fMaxSpeed) { m_fMaxYSpeed = fabs(_fMaxSpeed); }

    float GetFriction() { return m_fFriction; }
    void SetFriction(float _fFriction) { m_fFriction = _fFriction; }

    Vec2& GetVelocity() { return m_vVelocity; }

    void AddForce(Vec2 _vForce) { m_vCurForce += _vForce; }

    void OnGround();
    void OnAir();
    bool IsGround() { return m_bGround; }

    void EnableGravity() { m_bGravityActive = true; }
    void DisableGravity() { m_bGravityActive = false; }

public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();


    CLONE(CGravity);
public:
    CGravity();
    CGravity(const CGravity& _origin);
    ~CGravity();
};

