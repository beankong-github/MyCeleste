#pragma once

#include <Engine\CGameObject.h>

// Tool ���� GameObject
class CGameObjectEx :
    public CGameObject
{
public:
    void finalupdate();

public:
    CGameObjectEx();
    ~CGameObjectEx();
};

