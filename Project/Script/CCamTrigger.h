#pragma once
#include <Engine\CScript.h>

enum class STAGE 
{
    NONE,
    S00,
    S01,
    S02,
    S03,
    S04,
    S05
};

class CCamTrigger :
    public CScript
{
private:
    CGameObject*    m_pMainCam;
    STAGE           m_eNext;

    bool            m_bCamMove;
    Vec2            m_vTargetPos;

public:
    STAGE GetNextStage() { return m_eNext; }

    // 기본 가상함수
public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

private:
    void CreateNextStage();

    // 이벤트
private:
    virtual void OnCollisionEnter(CGameObject* _OtherObject)override;
    virtual void OnCollision(CGameObject* _OtherObject) override;
    virtual void OnCollisionExit(CGameObject* _OtherObject) override;
    
    CLONE(CCamTrigger)

public:
    CCamTrigger();
    ~CCamTrigger();


};

