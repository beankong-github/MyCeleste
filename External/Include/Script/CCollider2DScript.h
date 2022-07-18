#pragma once
#include <Engine\CScript.h>
#include <Engine\CCollider2D.h>

enum class COLLIDER2D_PROPERTY
{
    NONE,
    PLAYER,
    GROUND,
    WALL,
    SPIKE,
    DREAM_BLOCK,
    CAM_AREA,
    TELEPORT,
    OBJ
};


class CCollider2DScript :
    public CScript
{
private:
    CCollider2D*            m_pCollider;
    COLLIDER2D_PROPERTY     m_eProperty;

public:
    CCollider2D* GetCollider2D() { return m_pCollider; }
    COLLIDER2D_PROPERTY GetColliderProterty() { return m_eProperty; }

    void SetColliderProperty(COLLIDER2D_PROPERTY _e) { m_eProperty = _e; }

public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();
    
    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
    virtual void OnCollision(CGameObject* _OtherObject) override;
    virtual void OnCollisionExit(CGameObject* _OtherObject) override;
   

    CLONE(CCollider2DScript);

public:
    CCollider2DScript();
    ~CCollider2DScript();
};

