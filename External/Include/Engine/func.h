#pragma once


template<typename T>
void Safe_Del_Vec(vector<T*>& _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		SAFE_DELETE(_vec[i]);
	}
	_vec.clear();
}

template<typename T>
void Safe_Del_list(list<T*>& _list)
{
	typename list<T*>::iterator iter = _list.begin();

	for (; iter != _list.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	_list.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	typename map<T1, T2>::iterator iter = _map.begin();
	for (; iter != _map.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	_map.clear();
}

template<typename T, UINT _iSize>
void Safe_Del_Arr(T(&_Arr)[_iSize])
{
	for (UINT i = 0; i < _iSize; ++i)
	{
		SAFE_DELETE(_Arr[i]);
	}
}


void SaveStringToFile(const string& _str, FILE* _pFile);
void LoadStringFromFile(string& _str, FILE* _pFile);

void SaveWStringToFile(const wstring& _str, FILE* _pFile);
void LoadWStringFromFile(wstring& _str, FILE* _pFile);



const wchar_t* ToWString(COMPONENT_TYPE _type);
const char* ToString(COMPONENT_TYPE _type);
const wchar_t* ToWString(RES_TYPE _type);
const char* ToString(RES_TYPE _type);

std::wstring ToWString(const std::string& str);
std::string  ToString(const std::wstring& wstr);

#include "Ptr.h"
#include "CResMgr.h"

template<typename Res>
void SaveResPtr(Ptr<Res> _ptr, FILE* _pFile)
{
	// Resource가 null인지 조사하고 파일에 저장한다
	bool bNullPtr = nullptr == _ptr ? true : false;
	fwrite(&bNullPtr, sizeof(bool), 1, _pFile);

	// null이 아니라면 키와 경로 저장
	if (!bNullPtr)
	{
		SaveWStringToFile(_ptr->GetKey(), _pFile);
		SaveWStringToFile(_ptr->GetRelativePath(), _pFile);
	}
}

template<typename Res>
void LoadResPtr(Ptr<Res>& _ptr, FILE* _pFile)
{
	// Resource가 null이었는지 조사
	bool bNull = false;
	fread(&bNull, sizeof(bool), 1, _pFile);

	// null이 아니었다면 키와 경로 로드, 메모리에 로드
	if (!bNull)
	{
		wstring strKey, strRelativePath;

		LoadWStringFromFile(strKey, _pFile);
		LoadWStringFromFile(strRelativePath, _pFile);

		_ptr = CResMgr::GetInst()->Load<Res>(strKey, strRelativePath);
	}
}
