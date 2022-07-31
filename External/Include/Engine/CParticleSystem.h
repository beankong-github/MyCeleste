#pragma once
#include "CRenderComponent.h"

#include "CStructuredBuffer.h"
#include "CParticleUpdateShader.h"

class CParticleSystem :
    public CRenderComponent
{
private:
    Ptr<CParticleUpdateShader>      m_CS;           // 파티클 업데이트 쉐이더

    CStructuredBuffer*              m_ParticleBuffer;   // 파티클 구조화 버퍼
    CStructuredBuffer*              m_DataBuffer;       // 추가 데이터 버퍼(파티클 AliveCount)
    UINT                            m_iMaxCount;        // 파티클 최대 개수

    int                             m_bPosInherit;      // 오브젝트 좌표에 영향
    int                             m_iAliveCount;      // 이번 프레임에 생성(활성화) 될 파티클 수

    float                           m_fMinLifeTime;     // 파티클의 최소 생명 시간
    float                           m_fMaxLifeTime;     // 파티클의 최대 생명 시간

    float                           m_fStartSpeed;      // 파티클의 최소 속력
    float                           m_fEndSpeed;        // 파티클의 최대 속력

    Vec4                            m_vStartColor;      // 파티클 초기 색상
    Vec4                            m_vEndColor;        // 파티클 최종 색상

    Vec3                            m_vStartScale;      // 파티클 초기 크기
    Vec3                            m_vEndScale;        // 파티클 최종 크기


    float                           m_fParticleCreateDistance;  // 파티클 생성 범위
    float                           m_fParticleCreateTerm;      // 파티클 생성 간격
    float                           m_fAccTime;

    Ptr<CTexture>                   m_pTex[4];


public:
    void SetParticleUpdateShader(Ptr<CParticleUpdateShader> _pShader) { m_CS = _pShader; }
    void SetMaxParticleCount(UINT _iMax);
    void SetPosInherit(bool _b) { m_bPosInherit = _b; }
    void SetAliveParticleCountPerFrame(int _iCount) { m_iAliveCount = _iCount; }
    void SetMinLifeTime(float _fTime) { m_fMinLifeTime = _fTime; }
    void SetMaxLifeTime(float _fTime) { m_fMaxLifeTime = _fTime; }
    void SetStartSpeed(float _fSpeed) { m_fStartSpeed = _fSpeed; }
    void SetEndSpeed(float _fSpeed) { m_fEndSpeed = _fSpeed; }
    void SetStartColor(Vec4 _vColor) { m_vStartColor = _vColor; }
    void SetEndColor(Vec4 _vColor) { m_vEndColor = _vColor; }
    void SetStartScale(Vec3 _vScale) { m_vStartScale = _vScale; }
    void SetEndScale(Vec3 _vScale) { m_vEndScale = _vScale; }
    void SetParticleCreateDiastance(float _fDist) { m_fParticleCreateDistance = _fDist; }   // 파티클 생성 범위 설정
    void SetParticleCreateTerm(float _fTime) { m_fParticleCreateTerm = _fTime; }    // 파티클 생성 간격
    void SetParticleTexture(Ptr<CTexture> _pTex, int _idx) { if (0 <= _idx && 4 > _idx) m_pTex[_idx] = _pTex; }

    const Ptr<CParticleUpdateShader> GetParticelUpdateShader() { return m_CS; }
    UINT GetMaxParticleCount() { return m_iMaxCount; }
    bool GetPosInherit() {  return m_bPosInherit; }
    int GetAliveParticleCountPerFrame() { return m_iAliveCount; }
    float GetMinLifeTime() { return m_fMinLifeTime; }
    float GetMaxLifeTime() { return m_fMaxLifeTime; }
    float GetStartSpeed() { return m_fStartSpeed; }
    float GetEndSpeed() { return m_fEndSpeed; }
    Vec4 GetStartColor() { return m_vStartColor; }
    Vec4 GetEndColor() { return m_vEndColor; }
    Vec3 GetStartScale() { return m_vStartScale; }
    Vec3 GetEndScale() { return m_vEndScale; }
    float GetParticleCreateDiastance() { return m_fParticleCreateDistance; }// 파티클 생성 범위
    float GetParticleCreateTerm() { return m_fParticleCreateTerm; }// 파티클 생성 간격
    Ptr<CTexture> GetParticleTex(int _idx) { return m_pTex[_idx]; }


public:
    virtual void finalupdate() override;
    virtual void render()   override;

public:
    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;
    CLONE(CParticleSystem);

public:
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _origin);
    ~CParticleSystem();
};

