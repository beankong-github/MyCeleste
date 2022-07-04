#pragma once
#include "CRes.h"

class CScene;

// Scene Save Data Resource
class CSceneFile :
    public CRes
{    
public:
    virtual int Load(const wstring& _strFilePath) override;
        
public:
    CSceneFile();
    ~CSceneFile();
};

