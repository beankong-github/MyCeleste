#include "pch.h"

void SaveStringToFile(const string& _str, FILE* _pFile)
{
    BYTE len = (BYTE)_str.length();
    fwrite(&len, sizeof(BYTE), 1, _pFile);
    fwrite(_str.c_str(), sizeof(char), len, _pFile);
}

void LoadStringFromFile(string& _str, FILE* _pFile)
{
    char szBuffer[256] = {};
    BYTE len = 0;
    fread(&len, sizeof(BYTE), 1, _pFile);
    fread(szBuffer, sizeof(char), (size_t)len, _pFile);

    _str = szBuffer;
}

void SaveWStringToFile(const wstring& _str, FILE* _pFile)
{
    BYTE len = (BYTE)_str.length();
    fwrite(&len, sizeof(BYTE), 1, _pFile);
    fwrite(_str.c_str(), sizeof(wchar_t), len, _pFile);
}

void LoadWStringFromFile(wstring& _str, FILE* _pFile)
{
    wchar_t szBuffer[256] = {};
    BYTE len = 0;
    fread(&len, sizeof(BYTE), 1, _pFile);
    fread(szBuffer, sizeof(wchar_t), (size_t)len, _pFile);

    _str = szBuffer;
}

const wchar_t* ToWString(COMPONENT_TYPE _type)
{
    static const wchar_t* szWString[(UINT)COMPONENT_TYPE::END] = 
    {
        L"TRANSFORM",
        L"CAMERA",
        L"COLLIDER2D",
        L"COLLIDER3D",
        L"ANIMATOR2D",
        L"ANIMATOR3D",
        L"LIGHT2D",
        L"LIGHT3D",
        L"BOUNDINGBOX",
        L"MESHRENDER",
        L"TILEMAP",
        L"PARTICLESYSTEM",
        L"LANDSCAPE",
        L"DECAL",
    };

    return szWString[(UINT)_type];
}

const char* ToString(COMPONENT_TYPE _type)
{
    static const char* szString[(UINT)COMPONENT_TYPE::END] =
    {
        "TRANSFORM",
        "CAMERA",
        "COLLIDER2D",
        "COLLIDER3D",
        "ANIMATOR2D",
        "ANIMATOR3D",
        "LIGHT2D",
        "LIGHT3D",
        "BOUNDINGBOX",
        "MESHRENDER",
        "TILEMAP",
        "PARTICLESYSTEM",
        "LANDSCAPE",
        "DECAL",
    };

    return szString[(UINT)_type];
}

const wchar_t* ToWString(RES_TYPE _type)
{
    static const wchar_t* szWString[(UINT)RES_TYPE::END] =
    {
        L"PREFAB",
        L"MESHDATA",
        L"MATERIAL",
        L"GRAPHICS_SHADER",
        L"COMPUTE_SHADER",
        L"MESH",
        L"TEXTURE",
        L"SOUND",
        L"SCENEFILE",
        L"XMLDATA"
    };

    return szWString[(UINT)_type];
}

const char* ToString(RES_TYPE _type)
{
    static const char* szString[(UINT)RES_TYPE::END] =
    {
        "PREFAB",
        "MESHDATA",
        "MATERIAL",
        "GRAPHICS_SHADER",
        "COMPUTE_SHADER",
        "MESH",
        "TEXTURE",
        "SOUND",
        "SCENEFILE",
        "XMLDATA"
    };

    return szString[(UINT)_type];
}

std::wstring ToWString(const std::string& str)
{
    const int length = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr{};
    wstr.resize(length);
    MultiByteToWideChar(CP_ACP, 0, &str[0], -1, &wstr[0], length);
    wstr.pop_back();
    return wstr;
}

std::string ToString(const std::wstring& wstr)
{
    const int   length = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str{};
    str.resize(length);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, &str[0], length, nullptr, nullptr);
    str.pop_back();
    return str;
}
