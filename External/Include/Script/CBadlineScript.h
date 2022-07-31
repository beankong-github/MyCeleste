#pragma once
#include <Engine\CScript.h>

enum class BADLINE_STATE
{
    NONE,
    SPAWN,
    FOLLOW,
    PLAYER_DEAD
};

class CBadlineScript :
    public CScript
{
private:
    BADLINE_STATE m_ePrevState;
    BADLINE_STATE m_eState;

    CGameObject* m_pPlayer;
    list<Vec2> m_lTargetPos;
    float m_fStartDelay;
    float m_fRecordDelay;
    float m_fSpeed;

private:
    void PlayAnim();    
    void RecordPlayer();

    // 기본 가상함수
public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    
    // 이벤트
private:
    virtual void OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName) override;
    virtual void OnChildCollision(CGameObject* _OtherObject, wstring _childName) override;
    virtual void OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName)override;

    CLONE(CBadlineScript)

public:
    CBadlineScript();
    ~CBadlineScript();
};

