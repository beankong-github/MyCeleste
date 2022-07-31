#pragma once
#include <Engine\CScript.h>

class CDreamBlock :
    public CScript
{

public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;
    
    CLONE(CDreamBlock)

public:
    CDreamBlock();
    ~CDreamBlock();
};

