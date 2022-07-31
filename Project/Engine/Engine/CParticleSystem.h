#pragma once
#include "CRenderComponent.h"

#include "CStructuredBuffer.h"
#include "CParticleUpdateShader.h"

class CParticleSystem :
    public CRenderComponent
{
private:
    Ptr<CParticleUpdateShader>      m_CS;           // ��ƼŬ ������Ʈ ���̴�

    CStructuredBuffer*              m_ParticleBuffer;   // ��ƼŬ ����ȭ ����
    CStructuredBuffer*              m_DataBuffer;       // �߰� ������ ����(��ƼŬ AliveCount)
    UINT                            m_iMaxCount;        // ��ƼŬ �ִ� ����

    int                             m_bPosInherit;      // ������Ʈ ��ǥ�� ����
    int                             m_iAliveCount;      // �̹� �����ӿ� ����(Ȱ��ȭ) �� ��ƼŬ ��

    float                           m_fMinLifeTime;     // ��ƼŬ�� �ּ� ���� �ð�
    float                           m_fMaxLifeTime;     // ��ƼŬ�� �ִ� ���� �ð�

    float                           m_fStartSpeed;      // ��ƼŬ�� �ּ� �ӷ�
    float                           m_fEndSpeed;        // ��ƼŬ�� �ִ� �ӷ�

    Vec4                            m_vStartColor;      // ��ƼŬ �ʱ� ����
    Vec4                            m_vEndColor;        // ��ƼŬ ���� ����

    Vec3                            m_vStartScale;      // ��ƼŬ �ʱ� ũ��
    Vec3                            m_vEndScale;        // ��ƼŬ ���� ũ��


    float                           m_fParticleCreateDistance;  // ��ƼŬ ���� ����
    float                           m_fParticleCreateTerm;      // ��ƼŬ ���� ����
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
    void SetParticleCreateDiastance(float _fDist) { m_fParticleCreateDistance = _fDist; }   // ��ƼŬ ���� ���� ����
    void SetParticleCreateTerm(float _fTime) { m_fParticleCreateTerm = _fTime; }    // ��ƼŬ ���� ����
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
    float GetParticleCreateDiastance() { return m_fParticleCreateDistance; }// ��ƼŬ ���� ����
    float GetParticleCreateTerm() { return m_fParticleCreateTerm; }// ��ƼŬ ���� ����
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

