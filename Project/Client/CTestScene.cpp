#include "pch.h"
#include "CTestScene.h"

#include <Engine/CResMgr.h>
#include <Engine/CCollisionMgr.h>

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>

#include <Engine/CTransform.h>
#include <Engine/CMeshRender.h>
#include <Engine/CCollider2D.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CCamera.h>
#include <Engine/CTileMap.h>
#include <Engine/CLight2D.h>
#include <Engine/CAnimator2D.h>


#include <Script/CPlayerScript.h>
#include <Script/CCameraMoveScript.h>
#include <Script/CMissileScript.h>

void CTestScene::CreateTestScene()
{

	CScene* pCurScene = new CScene;
	CSceneMgr::GetInst()->ChangeScene(pCurScene);

	//return;

	pCurScene->SetLayerName(0, L"Tile");
	pCurScene->SetLayerName(1, L"Default");
	pCurScene->SetLayerName(2, L"Player");
	pCurScene->SetLayerName(3, L"Monster");

	// Prefab 제작
	//CGameObject* pMissileObj = new CGameObject;
	//pMissileObj->AddComponent(new CTransform);
	//pMissileObj->AddComponent(new CMeshRender);
	//pMissileObj->AddComponent(new CMissileScript);

	//pMissileObj->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	//pMissileObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	//pMissileObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));

	//CResMgr::GetInst()->AddRes<CPrefab>(L"MissilePrefab", new CPrefab(pMissileObj));


	// Camera Object 추가
	CGameObject* pCamObj = new CGameObject;
	pCamObj->SetName(L"MainCamera");
	pCamObj->AddComponent(new CTransform);
	pCamObj->AddComponent(new CCamera);
	pCamObj->AddComponent(new CCameraMoveScript);

	pCamObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pCamObj->Camera()->SetCameraAsMain();
	pCamObj->Camera()->CheckLayerMaskAll();

	pCurScene->AddObject(pCamObj, L"Default");

	//pCamObj = new CGameObject;
	//pCamObj->SetName(L"SubCam");
	//pCamObj->AddComponent(new CTransform);
	//pCamObj->AddComponent(new CCamera);
	//pCamObj->AddComponent(new CCameraMoveScript);
	//
	//pCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	//pCamObj->Camera()->CheckLayerMaskAll();
	//
	//pCurScene->AddObject(pCamObj, L"Default");

	// TileMap 추가
	Ptr<CTexture> tileAtlas = CResMgr::GetInst()->Load<CTexture>(L"texture\\TILE_32.bmp", L"texture\\TILE_32.bmp");

	//CGameObject* pTileMap = new CGameObject;
	//pTileMap->SetName(L"TileMap");
	//pTileMap->AddComponent(new CTransform);
	//pTileMap->Transform()->SetRelativePos(Vec3{ 0, 0, 10 });
	//pTileMap->Transform()->SetRelativeScale(Vec3{ 1500, 500, 1 });
	//
	//pTileMap->AddComponent(new CTileMap);
	//pTileMap->TileMap()->SetAtlasTex(tileAtlas);
	//pTileMap->TileMap()->SetTileSize(Vec2{ 64, 64 });
	//pTileMap->TileMap()->SetTileMapSize(30, 10);
	//
	//int iTileMapSize = pTileMap->TileMap()->GetTileMapSize().x * pTileMap->TileMap()->GetTileMapSize().y;
	//for (int i = 0; i < iTileMapSize; i++)
	//{
	//	pTileMap->TileMap()->SetTileData(i, 2);
	//}
	//
	//pCurScene->AddObject(pTileMap, L"Tile");
	//
	// Plane Object
	//CGameObject* pObject = new CGameObject;
	//pObject->SetName(L"Background");
	//
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	////pObject->AddComponent(new CPlayerScript);
	//
	//pObject->Transform()->SetRelativePos(0.f, 0.f, 500.f);
	//pObject->Transform()->SetRelativeScale(1600.f, 900.f, 1.f);
	//
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std2DMtrl.mtrl"));
	//pObject->MeshRender()->GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->Load<CTexture>(L"texture\\Background.png", L"texture\\Background.png"));
	//
	//pCurScene->AddObject(pObject, L"Default");


	// Particle Object 
	//CGameObject* pParticleObj = new CGameObject;
	//pParticleObj->SetName(L"ParticleObject_01");

	//pParticleObj->AddComponent(new CTransform);
	//pParticleObj->AddComponent(new CParticleSystem);
	//pParticleObj->AddComponent(new CCollider2D);

	//pParticleObj->Transform()->SetRelativePos(0.f, 0.f, 500.f);

	//Ptr<CTexture> pParticleTex = CResMgr::GetInst()->Load<CTexture>(L"Particle_01", L"texture\\particle\\AlphaCircle.png");
	//pParticleObj->ParticleSystem()->GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, pParticleTex);

	//pCurScene->AddObject(pParticleObj, L"Default");


	//pParticleObj = pParticleObj->Clone();
	//pParticleObj->SetName(L"ParticleObject_02");
	//pParticleObj->Transform()->SetRelativePos(-500.f, 0.f, 500.f);
	//
	//pCurScene->AddObject(pParticleObj, L"Default");

	// 플레이어 추가
	CGameObject* pPlayer = new CGameObject;
	pPlayer->SetName(L"Player");

	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CAnimator2D);
	pPlayer->AddComponent(new CMeshRender);
	pPlayer->AddComponent(new CPlayerScript);

	pPlayer->Transform()->SetRelativePos(Vec3(0.f, 0.f, 10.f));
	pPlayer->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 10.f));
	pPlayer->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Std2DMtrl.mtrl"));

	
	pPlayer->Animator2D()->CreateAnim(L"Anim", CResMgr::GetInst()->Load<CTexture>(L"texture\\link_0.png", L"texture\\link_0.png"), Vec2(60.f, 65.f), Vec2(0.f, 260.f), 
		Vec2(60.f, 65.f), Vec2(60.f, 0.f), 0.1f, 10, true);

	pPlayer->Animator2D()->Play(L"Anim");

	pCurScene->AddObject(pPlayer, L"Default");

	// 광원 오브젝트 추가
	CGameObject* pLight2D = new CGameObject;
	pLight2D->SetName(L"Light Object");
	
	pLight2D->AddComponent(new CTransform);
	pLight2D->AddComponent(new CMeshRender);
	pLight2D->AddComponent(new CLight2D);
	pLight2D->AddComponent(new CPlayerScript);
	
	pLight2D->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	
	pLight2D->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	pLight2D->Light2D()->SetRange(500.f);
	pLight2D->Light2D()->SetAngle(30.f);
	pLight2D->Light2D()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
	
	pCurScene->AddObject(pLight2D, L"Default");

	// PostProcess Object
	//CGameObject* pPostProcess = new CGameObject;
	//pPostProcess->SetName(L"PostProcessObject");

	//pPostProcess->AddComponent(new CTransform);
	//pPostProcess->AddComponent(new CMeshRender);

	//pPostProcess->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	//pPostProcess->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	//pPostProcess->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pPostProcess->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostProcessMtrl"));

	//pCurScene->AddObject(pPostProcess, L"Default");
	//pPostProcess->Deactivate();


	//((CCameraMoveScript*)pCamObj->GetScript(0))->SetFilter(pPostProcess);
	//CCameraMoveScript* pCamMoveScript = pCamObj->GetScript<CCameraMoveScript>();


	// 충돌 레이어 설정
	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"Monster");	

	pCurScene->start();	
	pCurScene->SetSceneState(SCENE_STATE::PLAY);
}
