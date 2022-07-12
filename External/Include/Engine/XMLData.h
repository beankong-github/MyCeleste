#pragma once
#include "CRes.h"
class XMLData :
    public CRes
{
public:
    virtual int Load(const wstring& _strFilePath) override;

public:
    XMLData();
    ~XMLData();
};

