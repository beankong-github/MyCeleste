#include "pch.h"
#include "CXMLData.h"

CXMLData::CXMLData()
	: CRes(RES_TYPE::XMLData)
{
}

CXMLData::~CXMLData()
{
}

tSprite* CXMLData::FindSpriteInfo(wstring _key)
{
	map<wstring, tSprite>::iterator iter = m_SpriteData.find(_key);

	if (iter != m_SpriteData.end())
		return &iter->second;
	
	return nullptr;
}

int CXMLData::Load(const wstring& _strFilePath)
{
	CMarkup xml;
	if (xml.Load(_strFilePath))
	{
		// ��Ʋ�� Ű �˻�
		wstring key = CPathMgr::GetInst()->GetRelativePath(_strFilePath);
		wstring atlasKey = key;
		size_t pos = atlasKey.find_last_of(L".");
		assert(pos!=atlasKey.npos);
		atlasKey = atlasKey.substr(0, pos);
		atlasKey += L".png";
		
		// �������� �ʴ� �̹��� ���Ͽ� ���� XML DATA�� ��� Return;
		if (nullptr == CResMgr::GetInst()->FindRes<CTexture>(atlasKey) || nullptr != CResMgr::GetInst()->FindRes<CXMLData>(key))
		{
			return E_FAIL;
		}
		m_wstrAtlasKey = atlasKey;
		pos = m_wstrAtlasKey.find_last_of(L"\\");
		wstring imgKey = m_wstrAtlasKey.substr(pos+1);
	
		// ��Ʋ���� ��������Ʈ �����͵��� �����Ѵ�
		if (xml.FindElem(L"TextureAtlas"))
		{
			while (xml.FindChildElem(L"sprite"))
			{
				xml.IntoElem();
				wstring spriteKey = xml.GetAttrib(L"n");
				wstring x = xml.GetAttrib(L"x");
 				wstring y = xml.GetAttrib(L"y");
				wstring width = xml.GetAttrib(L"w");
 				wstring height = xml.GetAttrib(L"h");
			
				Vec2 vPos = Vec2(std::stoi(x), std::stoi(y));
				Vec2 vSize = Vec2(std::stoi(width), std::stoi(height));
				
				tSprite tS;
				tS.vPos = vPos;
				tS.vSize = vSize;

				m_SpriteData.insert(make_pair(spriteKey, tS));

				xml.OutOfElem();
			
			}
			return S_OK;
		}
	}
	return E_FAIL;
}

