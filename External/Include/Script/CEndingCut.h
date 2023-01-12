#pragma once
#include <Engine\CScript.h>
class CEndingCut :
    public CScript
{
private:
    CGameObject* m_pTransition;
    bool         m_bEventPlay;

public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;
    
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
    virtual void OnCollision(CGameObject* _OtherObject) override;
    virtual void OnCollisionExit(CGameObject* _OtherObject) override;
    
    CLONE(CEndingCut)

public:
    CEndingCut();
    ~CEndingCut();
};

