#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Cube;
class CTexture;
class CShader;
END

BEGIN(Client)

class CEnvCube final : public CGameObject
{
public:
private:
	CEnvCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvCube(const CEnvCube& rhs);
	virtual ~CEnvCube() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				Start() override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void											Set_CubeRegion(_int iRegion) { if (m_iEntireTexture <= iRegion || iRegion<0)return; m_iCurRegion = iRegion; }
	void											Set_CubeTextureNum(_int iEnum) { if (m_TextureNumMap[m_iCurRegion] <= iEnum || iEnum < 0)return; m_iCubTextureNum = iEnum; }
	void											Set_WorldMatrix(_float4x4 WorldMatrix) { m_CubeList[m_iCurRegion][m_iCubTextureNum] = WorldMatrix; }
	
	_int											Get_CubeRegion() { return m_iCurRegion; }
	_int											Get_CubeTextureNum() { return m_iCubTextureNum; }
	_float4x4									Get_WorldFloat4x4() {return m_CubeList[m_iCurRegion][m_iCubTextureNum];	}
	_int											Get_EntireRegion() { return m_iEntireTexture; }
	_int											Get_MaxNum() { return m_TextureNumMap[m_iCurRegion]; }
	CTexture*									Get_Texture() { return m_TextureComMap[m_iCurRegion]; }
private:
	_int											m_iEntireTexture = {0};


	_int											m_iCurRegion = {0}; // 현재 맵 enum값(LOCATION_MAP_VISIT)
	_int											m_iCubTextureNum = {0}; //  현재 멀티 텍스쳐 num 
	map<_int, _int>							m_TextureNumMap = {};// 맵마다 멀티 cube텍스쳐 몇번까지 사용 가능? => 최대 enum값

	map<_int, vector<_float4x4>>	m_CubeList = {}; //맵enum값과 거기에 필요한 월드좌표
	map<_int, CTexture*>				m_TextureComMap = {}; // 맵enum값과 거기에 필요한 텍스쳐 => 얜 정해져있음 파싱 x
	CVIBuffer_Cube*						m_pVIBufferCom = { nullptr };
	CShader*									m_pShaderCom = { nullptr };	
	class CPlayer*							m_pPlayer = { nullptr };
private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CEnvCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END