#pragma once
#include <Engine/CScript.h>

class CGravity :
    public CScript
{
private:
    Vec2    m_vCurForce;
    Vec2    m_vVelocity;

    Vec2    m_vGravityForce;

    float   m_fSpeed; // 속력  
    float   m_fMass;  // 질량    

     // 최대 속력(제한)
    float   m_fMaxXSpeed;
    float   m_fMaxYSpeed;


    float   m_fFriction; // 마찰력


    bool    m_bGravityActive; // 중력 사용 유무
    bool    m_bGround;  // 오브젝트가 땅위에 있는지 공중에 떠있는지 체크



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

