#include "stdafx.h"
#include "Debuger.h"
#include "GameObject.h"
#include "Player.h"
#include "Body_Player.h"

CDebuger::CDebuger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGUIObject{ pDevice, pContext }
{
}

HRESULT CDebuger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDebuger::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Debugger");

	//	창 크기 다시 만듦
	ImVec2 WinSize = ImGui::GetWindowSize();
	ImGui::Text("WinSizeX : %f \n WinSizeY : %f", WinSize.x, WinSize.y);

	POINT ptMouse = m_pGameInstance->Get_MouseCurPos();
	ImGui::Text("MousePosX : %d \n MousePosY : %d", ptMouse.x, ptMouse.y);

	Update_Tools(fTimeDelta);

#ifdef  _DEBUG
	if (ImGui::Button("DebugRender"))
	{
		//	m_pGameInstance->On_Off_DebugRender();
	}
#endif //  _DEBUG	

	/*if (ImGui::CollapsingHeader("WorldLight"))
	{
		const LIGHT_DESC* pLightDesc = { const_cast<LIGHT_DESC*>(m_pGameInstance->Get_LightDesc(0)) };

		ImGui::SliderFloat4("LightDiffuse", (_float*)&pLightDesc->vDiffuse, 0.f, 1.f);
		ImGui::SliderFloat4("LightAmbient", (_float*)&pLightDesc->vAmbient, 0.f, 1.f);
		ImGui::SliderFloat4("LightDirrection", (_float*)&pLightDesc->vDirection, -1.f, 1.f);
		ImGui::SliderFloat4("LightSpecular", (_float*)&pLightDesc->vSpecular, 0.f, 1.f);
	}*/

//	for (auto Pair : Layers)
//	{
//		_char szLayerTag[MAX_PATH] = { "" };
//		const _tchar* szLargeLayerTag = Pair.first.c_str();
//
//		WideCharToMultiByte(CP_UTF8, 0, szLargeLayerTag, -1, szLayerTag, sizeof(szLayerTag), NULL, NULL);		
//
//		if (ImGui::CollapsingHeader(szLayerTag))
//		{
//			const list<CGameObject*>* pObjects = Pair.second;
//
//			_int iIdx = { 0 };
//			for (auto pObject : *pObjects)
//			{
//				++iIdx;
//				string strText = string(szLayerTag) + string("Object %d");
//				ImGui::Text(strText.c_str(), iIdx);
//
//				if (wstring(szLargeLayerTag) == TEXT("Layer_BackGround"))
//				{
//					CMap*		pMap = { dynamic_cast<CMap*>(pObject) };
//					if (nullptr != pMap)
//					{
//						if (ImGui::SliderFloat4("Fog_Color", (_float*)&pMap->m_vFogColor, 0.f, 1.f))
//						{
//
//						}
//						ImGui::InputFloat("Fog_Density", &pMap->m_fFogDensity);
//
//						////////////////////////////////////////////////////////////////
//						////////////////////////////////////////////////////////////////
//						////////////////////////////////MESH////////////////////////////
//						////////////////////////////////////////////////////////////////
//						////////////////////////////////////////////////////////////////
//						if (ImGui::CollapsingHeader(string(string("Mesh##") + to_string(iIdx)).c_str()))
//						{
//							CModel* pModel = { dynamic_cast<CModel*>(pMap->Get_Component(g_strComModelTag)) };
//							if (nullptr != pModel)
//							{
//								set<string>			MeshTags = pModel->Get_MeshTags();
//								map<_uint, string>	MeshSortingTags;
//								for (auto& strMeshTag : MeshTags)
//								{
//									list<_uint>		MeshIndices;
//									MeshIndices = { pModel->Get_MeshIndices(strMeshTag) };
//
//									for (auto& iMeshIndex : MeshIndices)
//									{
//										MeshSortingTags.emplace(iMeshIndex, strMeshTag);
//									}
//								}
//
//								for (auto& Pair : MeshSortingTags)
//								{
//
//									ImGui::Text("Mesh Index : %d", Pair.first);
//									ImGui::Text(string(string("Mesh Tag : ") + Pair.second).c_str());
//
//									MATERIAL_DESC			MatDesc = { pModel->Get_Mesh_MaterialDesc(Pair.first) };
//									string			strSliderTag = { string(string("Matrial ##") + to_string(iIdx) + to_string(Pair.first) + Pair.second) };
//
//									if (ImGui::SliderFloat4(strSliderTag.c_str(), (_float*)&MatDesc, 0.f, 1.f))
//									{
//										pModel->Set_Mesh_MaterialDesc(Pair.first, MatDesc);
//									}
//									ImGui::NewLine();
//								}
//							}
//						}
//					}					
//				}
//
//				if (wstring(szLargeLayerTag) == TEXT("Layer_Player"))
//				{
//					CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
//
//					if (nullptr == pPlayer)
//						continue;
//
//					const _float3* pMoveDir = { pPlayer->Get_MoveDir_Ptr() };
//					_uint				iCurStateIndex = { pPlayer->Get_CurStateIndex() };
//
//					CBody_Player* pBodyPlayer = { dynamic_cast<CBody_Player*>(pPlayer->Get_PartObject(CPlayer::PART_BODY)) };
//					if (nullptr != pBodyPlayer)
//					{
//						_uint		iAnimIndex = { pBodyPlayer->Get_CurrentAnimIndex() };
//
//						ImGui::Text("AnimIndex : %d", iAnimIndex);
//					}
//
//					CTransform* pTransform = { dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strComTransformTag)) };
//					_vector			vPosition = { pTransform->Get_State_Vector(CTransform::STATE_POSITION) };
//
//					ImGui::Text("Position X : %f", XMVectorGetX(vPosition));
//					ImGui::Text("Position Y : %f", XMVectorGetY(vPosition));
//					ImGui::Text("Position Z : %f", XMVectorGetZ(vPosition));
//					ImGui::NewLine();
//
//					_bool				isFlip = { pPlayer->Is_Filp() };
//
//					ImGui::Text("MoveDir X : %f", pMoveDir->x);
//					ImGui::Text("MoveDir Y : %f", pMoveDir->y);
//					ImGui::Text("MoveDir Z : %f", pMoveDir->z);
//					ImGui::NewLine();
//					ImGui::Text("CurStateIndex : %d", iCurStateIndex);
//					ImGui::NewLine();
//					if (true == isFlip)
//						ImGui::Text("Fliped");
//					else
//						ImGui::Text("Non Fliped");
//
//
//					CArm_Player* pArm = { dynamic_cast<CArm_Player*>(pPlayer->Get_PartObject(CPlayer::PART_ARM)) };
//					if (nullptr == pArm)
//						continue;
//
//					_float4* pDrawTranslation = { pArm->Get_Draw_Translation_Ptr() };
//					_float4* pJointTranslation = { pArm->Get_Joint_Translation_Ptr() };
//
//					ImGui::SliderFloat3("DrawTranslation", (_float*)pDrawTranslation, -0.2f, 0.2f);
//					ImGui::SliderFloat3("JointTranslation", (_float*)pJointTranslation, -0.2f, 0.2f);
//				}
//
//				if (wstring(szLargeLayerTag) == TEXT("Layer_Monster"))
//				{
//					CMonster* pMonster = dynamic_cast<CMonster*>(pObject);
//					if (nullptr == pMonster)
//						continue;
//
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////ANIMATION///////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					if (ImGui::CollapsingHeader(string(string("Animation##") + to_string(iIdx)).c_str()))
//					{
//						_int		iAnimIndex = { (_int)pMonster->Get_CurrentAnimIndex() };
//						_int		iKeyFrameIndex = { (_int)pMonster->Get_CurrentKeyFrame() };
//						CModel*			pModel = { dynamic_cast<CModel*>(pMonster->Get_Component(g_strComModelTag)) };
//						static _float	fTickPerSec = { 20.f };
//						ImGui::SliderFloat("TickPerSec", &fTickPerSec, 1.f, 60 * 25.f);
//
//						if (nullptr != pModel)
//						{
//							_uint			iNumAnims = { pModel->Get_NumAnims() };
//							for (_uint i = 0; i < iNumAnims; ++i)
//							{
//								pModel->Set_TickPerSec(i, fTickPerSec);
//
//							}
//						}
//
//						ImGui::Text("CurAnimIndex : %d", iAnimIndex);
//						ImGui::NewLine();
//						ImGui::Text("CurKeyFrameIndex : %d", iKeyFrameIndex);
//
//						if(ImGui::Button("PauseAnim"))
//						{
//							pMonster->Set_PauseAnimation();
//						}
//
//						if (ImGui::Button("NonPauseAnim"))
//						{
//							pMonster->Reset_PauseAnimation();
//						}
//
//						/////////////////////////////////////////////////////////////
//						/////////////////////////////////////////////////////////////
//						/////////////////////////SOUNDTEST///////////////////////////
//						/////////////////////////////////////////////////////////////
//						/////////////////////////////////////////////////////////////
//						static _uint		iTestSound = { 0 };
//						static _uint		iNumSound = { 26 };
//						ImGui::Text("CurrentSoundIndex : %d", iTestSound);
//						if (ImGui::Button("SoundNext"))
//						{
//							++iTestSound;
//						}
//						if (ImGui::Button("SoundPrevious"))
//						{
//							--iTestSound;
//						}
//
//						if (0 > iTestSound)
//						{
//							iTestSound = 0;
//						}
//
//						else if (iNumSound <= iTestSound)
//						{
//							iTestSound = iNumSound - 1;
//						}
//
//#pragma region HERMIT_CRAB_SOUND
//
//						vector<wstring>		SoundTag;
//						SoundTag.resize(iNumSound);
//
//						SoundTag[0] = { TEXT("Boss_HermitCrab_Angry_01.wav") };
//						SoundTag[1] = { TEXT("Boss_HermitCrab_Attack_01_01.wav") };
//						SoundTag[2] = { TEXT("Boss_HermitCrab_Attack_01_02.wav") };
//						SoundTag[3] = { TEXT("Boss_HermitCrab_Attack_02_01.wav") };
//						SoundTag[4] = { TEXT("Boss_HermitCrab_Attack_02_02.wav") };
//						SoundTag[5] = { TEXT("Boss_HermitCrab_Damage_01.wav") };
//						SoundTag[6] = { TEXT("Boss_HermitCrab_Damage_02.wav") };
//						SoundTag[7] = { TEXT("Boss_HermitCrab_Damage_A_01.wav") };
//						SoundTag[8] = { TEXT("Boss_HermitCrab_Damage_A_02.wav") };
//						SoundTag[9] = { TEXT("Boss_HermitCrab_Damage_A_03.wav") };
//						SoundTag[10] = { TEXT("Boss_HermitCrab_Die_01.wav") };
//						SoundTag[11] = { TEXT("Boss_HermitCrab_Foot_01.wav") };
//						SoundTag[12] = { TEXT("Boss_HermitCrab_Foot_02.wav") };
//						SoundTag[13] = { TEXT("Boss_HermitCrab_Move_01.wav") };
//						SoundTag[14] = { TEXT("Boss_HermitCrab_Normal_Attack_A_01.wav") };
//						SoundTag[15] = { TEXT("Boss_HermitCrab_Normal_Attack_A_02.wav") };
//						SoundTag[16] = { TEXT("Boss_HermitCrab_Normal_Attack_A_03.wav") };
//						SoundTag[17] = { TEXT("Boss_HermitCrab_Normal_Attack_B_01.wav") };
//						SoundTag[18] = { TEXT("Boss_HermitCrab_Normal_Attack_B_02.wav") };
//						SoundTag[19] = { TEXT("Boss_HermitCrab_Normal_Attack_B_03.wav") };
//						SoundTag[20] = { TEXT("Boss_HermitCrab_Roar_01.wav") };
//						SoundTag[21] = { TEXT("Boss_HermitCrab_Roar_02.wav") };
//						SoundTag[22] = { TEXT("Boss_HermitCrab_Turn_01.wav") };
//						SoundTag[23] = { TEXT("Timeline_Boss_HermitCrabAppear_01.wav") };
//						SoundTag[24] = { TEXT("Timeline_Boss_HermitCrabDie_01.wav") };
//						SoundTag[25] = { TEXT("Timeline_Boss_HermitCrabDie_02.wav") };
//						ImGui::Text("CurrentSoundTag");
//						ImGui::SameLine();
//						_char		szSoundTag[MAX_PATH];
//						WideCharToMultiByte(CP_UTF8, 0, SoundTag[iTestSound].c_str(), -1, szSoundTag, sizeof(szSoundTag), NULL, NULL);
//						ImGui::Text(szSoundTag);
//
//#pragma endregion
//
//						static _float		fDelaySoundTime = { 0.f };
//						static _float		fDelaySoundCool = { 0.f };
//						fDelaySoundCool -= fTimeDelta;
//						if (fDelaySoundCool < 0.f)
//							fDelaySoundCool = 0.f;
//
//						ImGui::InputFloat("SoundDelayTime", &fDelaySoundTime);
//
//						static _float		fDelayAnimTime = { 0.f };
//						static _float		fDelayAnimCool = { 0.f };
//						fDelayAnimCool -= fTimeDelta;
//						if (fDelayAnimCool < 0.f)
//							fDelayAnimCool = 0.f;
//
//						ImGui::InputFloat("AnimationDelayTime", &fDelayAnimTime);
//
//
//						static _bool		isNeedSound = { false };
//						static _bool		isNeedPlayAnimation = { false };
//						static _uint		iTargetAnimIndex = { 0 };
//						_int		iMaxAnimIndex = { (_int)pMonster->Get_KeyFrameIndices().size() };
//						if (ImGui::Button(string(string("NextAnim##") + to_string(iIdx)).c_str()))
//						{
//							fDelayAnimCool = fDelayAnimTime;
//							fDelaySoundCool = fDelaySoundTime;
//							isNeedSound = true;
//							isNeedPlayAnimation = true;
//
//							if (iAnimIndex + 1 <= iMaxAnimIndex)
//								iTargetAnimIndex = iAnimIndex + 1;
//
//						}
//
//						if (ImGui::Button(string(string("PreAnim##") + to_string(iIdx)).c_str()))
//						{
//							_uint		iAnimIndex = { pMonster->Get_CurrentAnimIndex() };
//
//							fDelayAnimCool = fDelayAnimTime;
//							fDelaySoundCool = fDelaySoundTime;
//							isNeedSound = true;
//							isNeedPlayAnimation = true;
//
//							if (iAnimIndex - 1 >= 0)
//								iTargetAnimIndex = iAnimIndex - 1;
//						}
//
//						if (true == isNeedPlayAnimation)
//						{
//							if (0.f >= fDelayAnimCool)
//							{
//								isNeedPlayAnimation = false;
//								pMonster->Set_Animation(iTargetAnimIndex);
//							}
//						}
//
//						/* SoundTest */
//						if (true == isNeedSound)
//						{
//							if (0.f >= fDelaySoundCool)
//							{
//								if (iTestSound < SoundTag.size())
//								{
//									m_pGameInstance->Stop_Sound(static_cast<_uint>(SOUND_CH::CH_BOSS_DAMAGE));
//									m_pGameInstance->Play_Sound(SoundTag[iTestSound], static_cast<_uint>(SOUND_CH::CH_BOSS_DAMAGE), false);
//								}
//
//								isNeedSound = false;
//							}
//						}
//
//						_float		fCurrentTrackPosition, fDuration;
//						fCurrentTrackPosition = { pMonster->Get_TrackPosition() };
//						fDuration = { pMonster->Get_Duration() };
//
//						if (ImGui::SliderFloat(string(string("TrackPosition##") + to_string(iIdx)).c_str(), &fCurrentTrackPosition, 0, fDuration))
//						{
//							pMonster->Set_TrackPosition(fCurrentTrackPosition);
//							pMonster->Play_Animation(fTimeDelta);
//						}
//
//						/////////////////////////////////////////////
//						/////////////////////////////////////////////
//						/////////////////////////////////////////////
//						////////////PLAY_ANIM////////////////////////
//						/////////////////////////////////////////////
//						/////////////////////////////////////////////
//						/////////////////////////////////////////////
//						CPredetor_Monster*		pPredetorMonster = { dynamic_cast<CPredetor_Monster*>(pMonster) };
//												
//
//						if (nullptr != pPredetorMonster)
//						{
//							if (static_cast<_int>(MONSTER_ID::ID_HAMMER_SHARK) == pPredetorMonster->Get_Status_Ptr()->iID)
//							{
//								static _int		iCurrentAnim = { -1 };
//
//								if (ImGui::Button("Hammer_Play_Attack_A"))
//									iCurrentAnim = 0;
//								if (0 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Attack_A();
//
//								if (ImGui::Button("Hammer_Play_Attack_B"))
//									iCurrentAnim = 1;
//								if (1 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Attack_B();
//
//								if (ImGui::Button("Hammer_Play_Attack_C"))
//									iCurrentAnim = 2;
//								if (2 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Attack_C();
//
//								if (ImGui::Button("Hammer_Play_Die"))
//									iCurrentAnim = 3;
//								if (3 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Die();
//
//								if (ImGui::Button("Hammer_Play_Sprint"))
//									iCurrentAnim = 5;
//								if (5 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Sprint();
//
//								if (ImGui::Button("Hammer_Play_Stun_A"))
//									iCurrentAnim = 6;
//								if (6 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Stun_A();
//
//								if (ImGui::Button("Hammer_Play_Swim_A"))
//									iCurrentAnim = 7;
//								if (7 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Swim_A();
//
//								if (ImGui::Button("Hammer_Play_Swim_B"))
//									iCurrentAnim = 8;
//								if (8 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Swim_B();
//
//								if (ImGui::Button("Hammer_Play_Swim_C"))
//									iCurrentAnim = 9;
//								if (9 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Swim_C();
//
//								if (ImGui::Button("Hammer_Play_Swim_Turn"))
//									iCurrentAnim = 10;
//								if (10 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Swim_Turn();
//
//								if (ImGui::Button("Hammer_Play_Swim_Weak"))
//									iCurrentAnim = 11;
//								if (11 == iCurrentAnim)
//									pPredetorMonster->Hammer_Play_Swim_Weak();
//							}
//							
//							else if (static_cast<_int>(MONSTER_ID::ID_SAW_SHARK) == pPredetorMonster->Get_Status_Ptr()->iID)
//							{
//								static _int		iCurrentAnim = { -1 };
//
//								if (ImGui::Button("Saw_Play_Attack_B"))
//									iCurrentAnim = 0;
//								if (0 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Attack_B();
//
//								if (ImGui::Button("Saw_Play_Die"))
//									iCurrentAnim = 1;
//								if (1 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Die();
//
//								if (ImGui::Button("Saw_Play_Sprint"))
//									iCurrentAnim = 2;
//								if (2 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Sprint();
//
//								if (ImGui::Button("Saw_Play_Swim_A"))
//									iCurrentAnim = 3;
//								if (3 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Swim_A();
//
//								if (ImGui::Button("Saw_Play_Swim_B"))
//									iCurrentAnim = 5;
//								if (5 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Swim_B();
//
//								if (ImGui::Button("Saw_Play_Swim_C"))
//									iCurrentAnim = 6;
//								if (6 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Swim_C();
//
//								if (ImGui::Button("Saw_Play_Swim_Turn"))
//									iCurrentAnim = 7;
//								if (7 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_Swim_Turn();
//
//								if (ImGui::Button("Saw_Play_QTE_Enter_A"))
//									iCurrentAnim = 8;
//								if (8 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_QTE_Enter_A();
//
//								if (ImGui::Button("Saw_Play_QTE_Fail_A"))
//									iCurrentAnim = 9;
//								if (9 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_QTE_Fail_A();
//
//								if (ImGui::Button("Saw_Play_QTE_Ready_A"))
//									iCurrentAnim = 10;
//								if (10 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_QTE_Ready_A();
//
//								if (ImGui::Button("Saw_Play_QTE_Success_A"))
//									iCurrentAnim = 11;
//								if (11 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_QTE_Success_A();
//
//								if (ImGui::Button("Saw_Play_QTE_Enter_Loop"))
//									iCurrentAnim = 12;
//								if (12 == iCurrentAnim)
//									pPredetorMonster->Saw_Play_QTE_Enter_Loop();
//							}
//						}
//						CBoss_Mantis_Shrimp* pMantisShrimp = { dynamic_cast<CBoss_Mantis_Shrimp*>(pMonster) };
//						if (nullptr != pMantisShrimp)
//						{
//							
//							_matrix			EyeMatrix = { XMLoadFloat4x4(&pMantisShrimp->m_EyeMatrix) };
//
//							_vector			vScale, vQuaternion, vTranslation;
//							XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, EyeMatrix);
//
//							ImGui::SliderFloat3("ScaleEye", reinterpret_cast<_float*>(&vScale), 0.f, 2.f);
//							ImGui::SliderFloat3("QuataernionEye", reinterpret_cast<_float*>(&vQuaternion), -1.f, 1.f);
//							ImGui::SliderFloat3("TranslationEye", reinterpret_cast<_float*>(&vTranslation), -10.f, 10.f);
//
//							EyeMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);
//
//							if (ImGui::Button("ResetEye"))
//							{
//								EyeMatrix = XMMatrixIdentity();
//							}
//
//							XMStoreFloat4x4(&pMantisShrimp->m_EyeMatrix, EyeMatrix);
//						}
//						
//
//						pMantisShrimp = { dynamic_cast<CBoss_Mantis_Shrimp*>(pMonster) };
//						if (nullptr != pMantisShrimp)
//						{
//							////////////////////////////////////////////
//							////////////////////////////////////////////
//							////////////////////////////////////////////
//							/////////////////////TRAIL//////////////////
//							////////////////////////////////////////////
//							////////////////////////////////////////////
//							////////////////////////////////////////////
//
//							CTransform* pTrailLeftHandTrans = { dynamic_cast<CTransform*>(pMantisShrimp->m_PartTrails[CBoss_Mantis_Shrimp::TRAIL_L_HAND]->Get_Component(g_strComTransformTag)) };
//							CTransform* pTrailRightHandTrans = { dynamic_cast<CTransform*>(pMantisShrimp->m_PartTrails[CBoss_Mantis_Shrimp::TRAIL_R_HAND]->Get_Component(g_strComTransformTag)) };
//							
//							if (nullptr != pTrailLeftHandTrans && nullptr != pTrailRightHandTrans)
//							{
//								_float4			vPositionLeftHand = { pTrailLeftHandTrans->Get_State_Float4(CTransform::STATE_POSITION) };
//								ImGui::SliderFloat3("TrailLeftHandTranslation", (_float*)&vPositionLeftHand, -10.f, 10.f);
//
//								pTrailLeftHandTrans->Set_State(CTransform::STATE_POSITION, vPositionLeftHand);
//
//
//								_float4			vPositionRightHand = { pTrailRightHandTrans->Get_State_Float4(CTransform::STATE_POSITION) };
//								ImGui::SliderFloat3("TrailRightHandTranslation", (_float*)&vPositionRightHand, -10.f, 10.f);
//
//								pTrailRightHandTrans->Set_State(CTransform::STATE_POSITION, vPositionRightHand);
//							}
//
//
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							////////////Compute_Distance, angle//////////
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//
//							CModel*			pModel = { dynamic_cast<CModel*>(pMonster->Get_Component(g_strComModelTag)) };
//							CTransform*		pTransform = { dynamic_cast<CTransform*>(pMonster->Get_Component(g_strComTransformTag)) };
//							if (nullptr != pModel && nullptr != pTransform)
//							{
//								_matrix		CombinedBoneMatrix = { XMLoadFloat4x4(pModel->Get_CombinedMatrix(MATNIS_SHRIMP_HEAD_BONETAG)) };
//								_matrix		WorldMatrix = { CombinedBoneMatrix * pTransform->Get_WorldMatrix() };
//
//								_vector		vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };
//
//								_float		fAngleToTarget = { pMonster->Compute_TargetAangle_From_Bone(vPosition) };
//								_bool		isUpperThanHead = { pMonster->Compute_TargetDirection_UpperThanLook(vPosition) };
//
//								fAngleToTarget = XMConvertToDegrees(fAngleToTarget);
//
//								ImGui::Text("====================================");
//								ImGui::Text("Target Angle From Head = %f", fAngleToTarget);
//								ImGui::Text("====================================");
//
//								ImGui::Text("====================================");
//								ImGui::Text("Target Upper Than Head = %d", isUpperThanHead);
//								ImGui::Text("====================================");
//							}
//
//							_float		fDistance = { pMonster->Compute_TargetDistance_AxisX() };
//							ImGui::Text("====================================");
//							ImGui::Text("Target Distance From Root = %f", fDistance);
//							ImGui::Text("====================================");
//							
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							////////////PLAY_ANIM////////////////////////
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							/////////////////////////////////////////////
//							static _int		iCurrentAnim = { -1 };
//							if (ImGui::Button("Play_PUNCH01A"))
//								iCurrentAnim = 0;
//							if(0 == iCurrentAnim)
//								pMantisShrimp->Play_PUNCH01A();
//
//							if (ImGui::Button("Play_PUNCH01B"))
//								iCurrentAnim = 1;
//							if (1 == iCurrentAnim)
//								pMantisShrimp->Play_PUNCH01B();
//
//							if (ImGui::Button("Play_PUNCH02A"))
//								iCurrentAnim = 2;
//							if (2 == iCurrentAnim)
//								pMantisShrimp->Play_PUNCH02A();
//
//							if (ImGui::Button("Play_PUNCH02B"))
//								iCurrentAnim = 3;
//							if (3 == iCurrentAnim)
//								pMantisShrimp->Play_PUNCH02B();
//
//							if (ImGui::Button("Play_UPPERCUT"))
//								iCurrentAnim = 5;
//							if (5 == iCurrentAnim)
//								pMantisShrimp->Play_UPPERCUT();
//
//							if (ImGui::Button("Play_StarightMove"))
//								iCurrentAnim = 6;
//							if (6 == iCurrentAnim)
//								pMantisShrimp->Play_StarightMove();
//
//							if (ImGui::Button("Play_Staright"))
//								iCurrentAnim = 7;
//							if (7 == iCurrentAnim)
//								pMantisShrimp->Play_Staright();
//
//							if (ImGui::Button("Play_Stun"))
//								iCurrentAnim = 8;
//							if (8 == iCurrentAnim)
//								pMantisShrimp->Play_Stun();
//
//							if (ImGui::Button("Play_MoveA"))
//								iCurrentAnim = 9;
//							if (9 == iCurrentAnim)
//								pMantisShrimp->Play_MoveA();
//
//							if (ImGui::Button("Play_MoveB"))
//								iCurrentAnim = 10;
//							if (10 == iCurrentAnim)
//								pMantisShrimp->Play_MoveB();
//
//							if (ImGui::Button("Play_DamageA"))
//								iCurrentAnim = 11;
//							if (11 == iCurrentAnim)
//								pMantisShrimp->Play_DamageA();
//
//							if (ImGui::Button("Play_DamageB"))
//								iCurrentAnim = 12;
//							if (12 == iCurrentAnim)
//								pMantisShrimp->Play_DamageB();
//
//							if (ImGui::Button("Play_DamageC"))
//								iCurrentAnim = 13;
//							if (13 == iCurrentAnim)
//								pMantisShrimp->Play_DamageC();
//
//							if (ImGui::Button("Play_BackAttack"))
//								iCurrentAnim = 14;
//							if (14 == iCurrentAnim)
//								pMantisShrimp->Play_BackAttack();
//
//							if (ImGui::Button("Play_Pounding"))
//								iCurrentAnim = 15;
//							if (15 == iCurrentAnim)
//								pMantisShrimp->Play_Pounding();
//
//							if (ImGui::Button("Play_Angry"))
//								iCurrentAnim = 16;
//							if (16 == iCurrentAnim)
//								pMantisShrimp->Play_Angry();
//
//							if (ImGui::Button("Play_Hide"))
//								iCurrentAnim = 17;
//							if (17 == iCurrentAnim)
//								pMantisShrimp->Play_Hide();
//
//							if (ImGui::Button("Play_Appear"))
//								iCurrentAnim = 18;
//							if (18 == iCurrentAnim)
//								pMantisShrimp->Play_Appear();
//
//							if (ImGui::Button("Play_Die"))
//								iCurrentAnim = 19;
//							if (19 == iCurrentAnim)
//								pMantisShrimp->Play_Die();
//
//							if (ImGui::Button("Play_Dead"))
//								iCurrentAnim = 20;
//							if (20 == iCurrentAnim)
//								pMantisShrimp->Play_Dead();
//
//							if (ImGui::Button("Play_IdleA"))
//								iCurrentAnim = 21;
//							if (21 == iCurrentAnim)
//								pMantisShrimp->Play_IdleA();
//
//							if (ImGui::Button("Play_IdleB"))
//								iCurrentAnim = 22;
//							if (22 == iCurrentAnim)
//								pMantisShrimp->Play_IdleB();
//						}
//
//						CBoss_Hermit_Crab* pHermitCrab = { dynamic_cast<CBoss_Hermit_Crab*>(pMonster) };
//						if (nullptr != pHermitCrab)
//						{
//						}
//					}
//
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////COLLIDER////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					if (ImGui::CollapsingHeader(string(string("Collider##") + to_string(iIdx)).c_str()))
//					{
//						//	CBoss_Mantis_Shrimp* pMantisShrimp = { dynamic_cast<CBoss_Mantis_Shrimp*>(pMonster) };
//						CBoss_Hermit_Crab* pHermitCrab = { dynamic_cast<CBoss_Hermit_Crab*>(pMonster) };
//						if (nullptr != pHermitCrab)
//						{
//							_uint			iBoneIndex = { pHermitCrab->m_iBoneIndex };
//
//							if (ImGui::Button("NextBone"))
//							{
//								iBoneIndex += 1;
//							}
//
//							if (ImGui::Button("PreBone"))
//							{
//								if (iBoneIndex != 0)
//								{
//									iBoneIndex -= 1;
//								}
//							}
//
//							pHermitCrab->m_iBoneIndex = iBoneIndex;
//						}
//
//						vector<CCollider*>& PartColliders = { pMonster->Get_PartColliders() };
//						_uint		iColliderIdx = { 0 };
//						for (auto& pCollider : PartColliders)
//						{
//							CCollider::TYPE			eType = { pCollider->Get_Type() };
//
//							if (CCollider::TYPE::TYPE_AABB == eType)
//							{
//								ImGui::Text(string(string("ColliderAABB ") + to_string(iColliderIdx)).c_str());
//								ImGui::NewLine();
//
//								CBounding_AABB::BOUNDING_AABB_DESC			BoundingDesc{};
//								BoundingBox* pBoundingDesc = (BoundingBox*)pCollider->Get_OriginBoundingDesc();
//								_float3		vCenter = { pBoundingDesc->Center };
//								_float3		vExtents = { pBoundingDesc->Extents };
//								_float3		vSize = { vExtents.x * 2.f, vExtents.y * 2.f, vExtents.z * 2.f };
//
//								ImGui::InputFloat3(string(string("Center##") + to_string(iColliderIdx)).c_str(), (_float*)&vCenter);
//								ImGui::InputFloat3(string(string("Size##") + to_string(iColliderIdx)).c_str(), (_float*)&vSize);
//								ImGui::NewLine();
//
//								BoundingDesc.vCenter = vCenter;
//								BoundingDesc.vSize = vSize;
//
//								pCollider->Set_BoundingDesc(&BoundingDesc);
//							}
//
//							else if (CCollider::TYPE::TYPE_OBB == eType)
//							{
//								ImGui::Text(string(string("ColliderOBB ") + to_string(iColliderIdx)).c_str());
//								ImGui::NewLine();
//
//								CBounding_OBB::BOUNDING_OBB_DESC			BoundingDesc{};
//								BoundingOrientedBox* pBoundingDesc = (BoundingOrientedBox*)pCollider->Get_OriginBoundingDesc();
//								_float3		vCenter = { pBoundingDesc->Center };
//								_float3		vExtents = { pBoundingDesc->Extents };
//								_float3		vSize = { vExtents.x * 2.f, vExtents.y * 2.f, vExtents.z * 2.f };
//								_float4		vOrientation = { pBoundingDesc->Orientation };
//								_float3		vRotation = {};
//
//								_float sinp = 2.0f * (vOrientation.w * vOrientation.x + vOrientation.y * vOrientation.z);
//								_float cosp = 1.0f - 2.0f * (vOrientation.x * vOrientation.x + vOrientation.y * vOrientation.y);
//								vRotation.x = atan2(sinp, cosp);
//
//								// yaw (y-axis rotation)
//								_float siny = 2.0f * (vOrientation.w * vOrientation.z + vOrientation.x * vOrientation.y);
//								_float cosy = 1.0f - 2.0f * (vOrientation.y * vOrientation.y + vOrientation.z * vOrientation.z);
//								vRotation.y = atan2(siny, cosy);
//
//								// roll (z-axis rotation)
//								_float sinr = 2.0f * (vOrientation.w * vOrientation.y - vOrientation.z * vOrientation.x);
//								_float cosr = 1.0f - 2.0f * (vOrientation.y * vOrientation.y + vOrientation.z * vOrientation.z);
//								vRotation.z = atan2(sinr, cosr);
//
//								vRotation.x = XMConvertToDegrees(vRotation.x);
//								vRotation.y = XMConvertToDegrees(vRotation.y);
//								vRotation.z = XMConvertToDegrees(vRotation.z);
//
//								_bool		isChanged = { false };
//								if (ImGui::InputFloat3(string(string("Center##") + to_string(iColliderIdx)).c_str(), (_float*)&vCenter))
//									isChanged = true;
//								if (ImGui::InputFloat3(string(string("Size##") + to_string(iColliderIdx)).c_str(), (_float*)&vSize))
//									isChanged = true;
//								if (ImGui::InputFloat3(string(string("Rotation##") + to_string(iColliderIdx)).c_str(), (_float*)&vRotation))
//									isChanged = true;
//
//								ImGui::NewLine();
//
//								vRotation.x = XMConvertToRadians(vRotation.x);
//								vRotation.y = XMConvertToRadians(vRotation.y);
//								vRotation.z = XMConvertToRadians(vRotation.z);
//
//								BoundingDesc.vCenter = vCenter;
//								BoundingDesc.vSize = vSize;
//								BoundingDesc.vRotation = vRotation;
//
//								if (true == isChanged)
//									pCollider->Set_BoundingDesc(&BoundingDesc);
//							}
//
//							else if (CCollider::TYPE::TYPE_SPHERE == eType)
//							{
//								ImGui::Text(string(string("ColliderSphere ") + to_string(iColliderIdx)).c_str());
//								ImGui::NewLine();
//
//								CBounding_Sphere::BOUNDING_SPHERE_DESC			BoundingDesc{};
//								BoundingSphere* pBoundingDesc = (BoundingSphere*)pCollider->Get_OriginBoundingDesc();
//								_float3		vCenter = { pBoundingDesc->Center };
//								_float		fRadius = { pBoundingDesc->Radius };
//
//								ImGui::InputFloat3(string(string("Center##") + to_string(iColliderIdx)).c_str(), (_float*)&vCenter);
//								ImGui::InputFloat(string(string("Radius##") + to_string(iColliderIdx)).c_str(), (_float*)&fRadius);
//								ImGui::NewLine();
//
//								BoundingDesc.vCenter = vCenter;
//								BoundingDesc.fRadius = fRadius;
//
//								pCollider->Set_BoundingDesc(&BoundingDesc);
//							}
//
//							++iColliderIdx;
//						}
//					}
//
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////MESH////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					if (ImGui::CollapsingHeader(string(string("Mesh##") + to_string(iIdx)).c_str()))
//					{
//						CModel* pModel = { dynamic_cast<CModel*>(pMonster->Get_Component(g_strComModelTag)) };
//						if (nullptr != pModel)
//						{
//							set<string>			MeshTags = pModel->Get_MeshTags();
//							map<_uint, string>	MeshSortingTags;
//							for (auto& strMeshTag : MeshTags)
//							{
//								list<_uint>		MeshIndices;
//								MeshIndices = { pModel->Get_MeshIndices(strMeshTag) };
//
//								for (auto& iMeshIndex : MeshIndices)
//								{
//									MeshSortingTags.emplace(iMeshIndex, strMeshTag);
//								}
//							}
//
//							for (auto& Pair : MeshSortingTags)
//							{
//
//								ImGui::Text("Mesh Index : %d", Pair.first);
//								ImGui::Text(string(string("Mesh Tag : ") + Pair.second).c_str());
//
//								MATERIAL_DESC			MatDesc = { pModel->Get_Mesh_MaterialDesc(Pair.first) };
//								string			strSliderTag = { string(string("Matrial ##") + to_string(iIdx) + to_string(Pair.first) + Pair.second) };
//																									
//								if (ImGui::SliderFloat4(strSliderTag.c_str(), (_float*)&MatDesc, 0.f, 1.f))
//								{
//									pModel->Set_Mesh_MaterialDesc(Pair.first, MatDesc);
//								}
//
//
//								if (ImGui::Button(string(string("HideMesh ##") + to_string(iIdx) + to_string(Pair.first)).c_str()))
//								{
//									CBoss_Hermit_Crab* pHermitCrab = { dynamic_cast<CBoss_Hermit_Crab*>(pMonster) };
//									if (nullptr != pHermitCrab)
//									{
//										pHermitCrab->Add_HideParts(Pair.second);
//									}
//								}
//								ImGui::SameLine();
//								if (ImGui::Button(string(string("ShowMesh ##") + to_string(iIdx) + to_string(Pair.first)).c_str()))
//								{
//									CBoss_Hermit_Crab* pHermitCrab = { dynamic_cast<CBoss_Hermit_Crab*>(pMonster) };
//									if (nullptr != pHermitCrab)
//									{
//										pHermitCrab->Delete_HideParts(Pair.second);
//									}
//								}
//								ImGui::NewLine();
//							}
//						}
//					}
//				}
//
//				if (wstring(szLargeLayerTag) == TEXT("Layer_UI"))
//				{
//					if (ImGui::Button(string(string("Delete##") + to_string(iIdx)).c_str()))
//					{
//						CGameObject* pUI = dynamic_cast<CGameObject*>(pObject);
//
//						if (nullptr == pUI)
//							continue;
//
//						pUI->Set_Release();
//					}
//				}
//
//				if (wstring(szLargeLayerTag) == TEXT("Layer_Misc_Iron_Ball"))
//				{
//					CIron_Ball*			pIronBall = { dynamic_cast<CIron_Ball*>(pObject) };
//					if (nullptr == pIronBall)
//						continue;
//
//					/////////////////////////////////////////////////////
//					/////////////////////////////////////////////////////
//					/////////////////////TRANSFORM///////////////////////
//					/////////////////////////////////////////////////////
//					/////////////////////////////////////////////////////
//					CTransform*			pTransform = { dynamic_cast<CTransform*>(pIronBall->Get_Component(g_strComTransformTag)) };
//					_float4				vPosition = { pTransform->Get_State_Float4(CTransform::STATE_POSITION) };
//
//					ImGui::InputFloat3("Position Iron Ball", (_float*)&vPosition);
//					pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
//
//					_float3				vScale = { pTransform->Get_Scaled() };
//					ImGui::InputFloat3("Scale Iron Ball ", (_float*)&vScale);
//					if (vScale.x > 0.f && vScale.y > 0.f && vScale.z > 0.f)
//					{
//						pTransform->Set_Scaled(vScale);
//					}
//
//					/////////////////////////////////////////////////////
//					/////////////////////////////////////////////////////
//					/////////////////////ANIM_CON////////////////////////
//					/////////////////////////////////////////////////////
//					/////////////////////////////////////////////////////
//
//					CModel* pModel = { dynamic_cast<CModel*>(pIronBall->Get_Component(g_strComModelTag)) };
//					if (nullptr == pModel)
//						continue;
//
//					_int		iAnimIndex = { (_int)pModel->Get_CurrentAnimIndex() };
//					_int		iKeyFrameIndex = { (_int)pModel->Get_CurrentMaxKeyFrameIndex() };
//
//					ImGui::Text("CurAnimIndex : %d", iAnimIndex);
//					ImGui::NewLine();
//					ImGui::Text("CurKeyFrameIndex : %d", iKeyFrameIndex);
//
//					_int		iMaxAnimIndex = { (_int)pModel->Get_CurrentKeyFrameIndices().size() };
//					static _bool		isLoop = { false };
//
//					if(ImGui::RadioButton("Loop", isLoop))
//					{
//						isLoop = !isLoop;
//					}
//
//					static _uint		iTargetAnimIndex = { 0 };
//
//					if (ImGui::Button(string(string("NextAnim## IronBall") + to_string(iIdx)).c_str()))
//					{
//						if (iAnimIndex + 1 <= iMaxAnimIndex)
//							iTargetAnimIndex = iAnimIndex + 1;
//
//						pModel->Set_Animation(iTargetAnimIndex, isLoop);
//					}
//
//					if (ImGui::Button(string(string("PreAnim## IronBall") + to_string(iIdx)).c_str()))
//					{
//						_uint		iAnimIndex = { pModel->Get_CurrentAnimIndex() };
//
//						if (iAnimIndex - 1 >= 0)
//							iTargetAnimIndex = iAnimIndex - 1;
//
//						pModel->Set_Animation(iTargetAnimIndex, isLoop);
//					}
//
//					_float		fCurrentTrackPosition, fDuration;
//					fCurrentTrackPosition = { pModel->Get_TrackPosition() };
//					fDuration = { pModel->Get_Duration() };
//
//					if (ImGui::SliderFloat(string(string("TrackPosition## IronBall") + to_string(iIdx)).c_str(), &fCurrentTrackPosition, 0, fDuration))
//					{
//						pModel->Set_TrackPosition(fCurrentTrackPosition);
//						pModel->Play_Animation(fTimeDelta);
//					}
//
//
//					/////////////////////////////////////////////////////
//					/////////////////////////////////////////////////////
//					/////////////////////HIDE_MESH///////////////////////
//					/////////////////////////////////////////////////////
//					/////////////////////////////////////////////////////
//					pModel->Get_NumMeshes();
//					set<string>			MeshTags = { pModel->Get_MeshTags() };
//					_uint iCnt = { 0 };
//					for (auto& MeshTag : MeshTags)
//					{
//						list<_uint>			MeshIndices = { pModel->Get_MeshIndices(MeshTag) };
//
//						for (auto& iMeshIndex : MeshIndices)
//						{
//							string		strHideText = { string(string("Hide Mesh ## Iron Ball") + to_string(iIdx).c_str() + to_string(iCnt++)).c_str() };
//							string		strShowText = { string(string("Show Mesh ## Iron Ball") + to_string(iIdx).c_str() + to_string(iCnt++)).c_str() };
//							ImGui::Text(MeshTag.c_str());
//							if(ImGui::Button(strHideText.c_str()))
//							{
//								pIronBall->Hide_Mesh(iMeshIndex);
//							}
//
//							ImGui::SameLine();
//
//							ImGui::Text(MeshTag.c_str());
//							if (ImGui::Button(strShowText.c_str()))
//							{
//								pIronBall->Show_Mesh(iMeshIndex);
//							}
//						}
//					}
//					
//
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////COLLIDER////////////////////////////
//					////////////////////////////////////////////////////////////////
//					////////////////////////////////////////////////////////////////
//					if (ImGui::CollapsingHeader(string(string("Collider## Iron Ball") + to_string(iIdx)).c_str()))
//					{
//						/////////////////////////////////////////////////////
//						/////////////////////////////////////////////////////
//						/////////////////////PART_COLL///////////////////////
//						/////////////////////////////////////////////////////
//						/////////////////////////////////////////////////////
//
//						_int			iBoneIndex = { (_int)pIronBall->m_iBoneIndex };
//						_uint			iMaxBoneIndex = { (_uint)pModel->Get_BoneNames().size() - 1 };
//
//						string			strBoneName = { pModel->Get_BoneNames()[iBoneIndex] };
//						ImGui::Text(strBoneName.c_str());
//
//						if (ImGui::Button("NextBone ## Iron Ball"))
//						{
//							iBoneIndex += 1;
//							if (iMaxBoneIndex < (_uint)iBoneIndex)
//								iBoneIndex = iMaxBoneIndex;
//						}
//
//						if (ImGui::Button("PreBone ## Iron Ball"))
//						{
//							iBoneIndex -= 1;
//							if (0 > iBoneIndex)
//								iBoneIndex = 0;
//						}
//
//						pIronBall->m_iBoneIndex = iBoneIndex;
//
//						vector<CCollider*>& PartColliders = { pIronBall->m_PartColliders };
//						_uint		iColliderIdx = { 0 };
//						for (auto& pCollider : PartColliders)
//						{
//							CCollider::TYPE			eType = { pCollider->Get_Type() };
//
//							if (CCollider::TYPE::TYPE_AABB == eType)
//							{
//								ImGui::Text(string(string("ColliderAABB ") + to_string(iColliderIdx)).c_str());
//								ImGui::NewLine();
//
//								CBounding_AABB::BOUNDING_AABB_DESC			BoundingDesc{};
//								BoundingBox* pBoundingDesc = (BoundingBox*)pCollider->Get_OriginBoundingDesc();
//								_float3		vCenter = { pBoundingDesc->Center };
//								_float3		vExtents = { pBoundingDesc->Extents };
//								_float3		vSize = { vExtents.x * 2.f, vExtents.y * 2.f, vExtents.z * 2.f };
//
//								ImGui::InputFloat3(string(string("Center##") + to_string(iColliderIdx)).c_str(), (_float*)&vCenter);
//								ImGui::InputFloat3(string(string("Size##") + to_string(iColliderIdx)).c_str(), (_float*)&vSize);
//								ImGui::NewLine();
//
//								BoundingDesc.vCenter = vCenter;
//								BoundingDesc.vSize = vSize;
//
//								pCollider->Set_BoundingDesc(&BoundingDesc);
//							}
//
//							else if (CCollider::TYPE::TYPE_OBB == eType)
//							{
//								ImGui::Text(string(string("ColliderOBB ") + to_string(iColliderIdx)).c_str());
//								ImGui::NewLine();
//
//								CBounding_OBB::BOUNDING_OBB_DESC			BoundingDesc{};
//								BoundingOrientedBox* pBoundingDesc = (BoundingOrientedBox*)pCollider->Get_OriginBoundingDesc();
//								_float3		vCenter = { pBoundingDesc->Center };
//								_float3		vExtents = { pBoundingDesc->Extents };
//								_float3		vSize = { vExtents.x * 2.f, vExtents.y * 2.f, vExtents.z * 2.f };
//								_float4		vOrientation = { pBoundingDesc->Orientation };
//								_float3		vRotation = {};
//
//								_float sinp = 2.0f * (vOrientation.w * vOrientation.x + vOrientation.y * vOrientation.z);
//								_float cosp = 1.0f - 2.0f * (vOrientation.x * vOrientation.x + vOrientation.y * vOrientation.y);
//								vRotation.x = atan2(sinp, cosp);
//
//								// yaw (y-axis rotation)
//								_float siny = 2.0f * (vOrientation.w * vOrientation.z + vOrientation.x * vOrientation.y);
//								_float cosy = 1.0f - 2.0f * (vOrientation.y * vOrientation.y + vOrientation.z * vOrientation.z);
//								vRotation.y = atan2(siny, cosy);
//
//								// roll (z-axis rotation)
//								_float sinr = 2.0f * (vOrientation.w * vOrientation.y - vOrientation.z * vOrientation.x);
//								_float cosr = 1.0f - 2.0f * (vOrientation.y * vOrientation.y + vOrientation.z * vOrientation.z);
//								vRotation.z = atan2(sinr, cosr);
//
//								vRotation.x = XMConvertToDegrees(vRotation.x);
//								vRotation.y = XMConvertToDegrees(vRotation.y);
//								vRotation.z = XMConvertToDegrees(vRotation.z);
//
//								_bool		isChanged = { false };
//								if (ImGui::InputFloat3(string(string("Center##") + to_string(iColliderIdx)).c_str(), (_float*)&vCenter))
//									isChanged = true;
//								if (ImGui::InputFloat3(string(string("Size##") + to_string(iColliderIdx)).c_str(), (_float*)&vSize))
//									isChanged = true;
//								if (ImGui::InputFloat3(string(string("Rotation##") + to_string(iColliderIdx)).c_str(), (_float*)&vRotation))
//									isChanged = true;
//
//								ImGui::NewLine();
//
//								vRotation.x = XMConvertToRadians(vRotation.x);
//								vRotation.y = XMConvertToRadians(vRotation.y);
//								vRotation.z = XMConvertToRadians(vRotation.z);
//
//								BoundingDesc.vCenter = vCenter;
//								BoundingDesc.vSize = vSize;
//								BoundingDesc.vRotation = vRotation;
//
//								if (true == isChanged)
//									pCollider->Set_BoundingDesc(&BoundingDesc);
//							}
//
//							else if (CCollider::TYPE::TYPE_SPHERE == eType)
//							{
//								ImGui::Text(string(string("ColliderSphere ") + to_string(iColliderIdx)).c_str());
//								ImGui::NewLine();
//
//								CBounding_Sphere::BOUNDING_SPHERE_DESC			BoundingDesc{};
//								BoundingSphere* pBoundingDesc = (BoundingSphere*)pCollider->Get_OriginBoundingDesc();
//								_float3		vCenter = { pBoundingDesc->Center };
//								_float		fRadius = { pBoundingDesc->Radius };
//
//								ImGui::InputFloat3(string(string("Center##") + to_string(iColliderIdx)).c_str(), (_float*)&vCenter);
//								ImGui::InputFloat(string(string("Radius##") + to_string(iColliderIdx)).c_str(), (_float*)&fRadius);
//								ImGui::NewLine();
//
//								BoundingDesc.vCenter = vCenter;
//								BoundingDesc.fRadius = fRadius;
//
//								pCollider->Set_BoundingDesc(&BoundingDesc);
//							}
//
//							++iColliderIdx;
//						}
//					}
//				}
//
//				if (wstring(szLargeLayerTag) == TEXT("Layer_Misc_Thrown"))
//				{
//					CHermit_Crab_Throw* pThrown = { dynamic_cast<CHermit_Crab_Throw*>(pObject) };
//					if (nullptr == pThrown)
//						continue;
//
//					CModel* pModel = { dynamic_cast<CModel*>(pThrown->Get_Component(g_strComModelTag)) };
//					if (nullptr == pModel)
//						continue;
//
//					set<string>			MeshTags = { pModel->Get_MeshTags() };
//
//					for (auto& strTag : MeshTags)
//						ImGui::Text(strTag.c_str());
//
//					CCollider* pCollider = { dynamic_cast<CCollider*>(pThrown->Get_Component(g_strComColliderTag)) };
//					if (nullptr == pCollider)
//						continue;
//
//					CCollider::TYPE		eType = { pCollider->Get_Type() };
//
//					if (CCollider::TYPE_AABB == eType)
//					{
//
//					}
//
//					if (CCollider::TYPE_OBB == eType)
//					{
//						BoundingOrientedBox* pBoundingDesc = { static_cast<BoundingOrientedBox*>(pCollider->Get_OriginBoundingDesc()) };
//
//						_float3			vSize = { pBoundingDesc->Extents };
//
//						if (ImGui::InputFloat3(string(string("OBBSize_Collider_") + to_string(iIdx)).c_str(), (_float*)&vSize, "%.6f"))
//						{
//							if (vSize.x != 0.0f &&
//								vSize.y != 0.0f &&
//								vSize.z != 0.0f)
//								pBoundingDesc->Extents = vSize;
//
//						}
//
//						_float3			vPosition = { pBoundingDesc->Center };
//
//						if (ImGui::InputFloat3(string(string("OBBPosition_Collider_") + to_string(iIdx)).c_str(), (_float*)&vPosition, "%.6f"))
//						{
//							pBoundingDesc->Center = vPosition;
//						}
//					}
//
//					if (CCollider::TYPE_AABB == eType)
//					{
//
//					}
//
//					CTransform* pTransform = { dynamic_cast<CTransform*>(pThrown->Get_Component(g_strComTransformTag)) };
//					if (nullptr == pTransform)
//						continue;
//
//					_float3			vScale = { pTransform->Get_Scaled() };
//
//					if (ImGui::InputFloat3(string(string("Scale_") + to_string(iIdx)).c_str(), (_float*)&vScale, "%.6f"))
//					{
//						if (vScale.x != 0.0f &&
//							vScale.y != 0.0f &&
//							vScale.z != 0.0f)
//							pTransform->Set_Scaled(vScale);
//
//					}
//					if (ImGui::Button(string(string("Turn") + to_string(iIdx)).c_str()))
//						pTransform->Turn_Absolute(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
//
//					if (ImGui::Button(string(string("Release") + to_string(iIdx)).c_str()))
//						pObject->Set_Release();
//				}
//			}
//		}
//	}
//
//	/* For.Test Terrain */
//	/*const list<CGameObject*>* pBackGrounds = m_pGameInstance->Get_LayerList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"));
//	_float4 vPickPos = { 0.f, 0.f, 0.f, 1.f };
//	if (nullptr != pBackGrounds)
//	{
//		CGameObject* pTerrain = pBackGrounds->front();
//		if (nullptr != pTerrain)
//		{
//			CModel* pVIBuffer_Terrain = static_cast<CModel*>(pTerrain->Get_Component(g_strComModelTag));
//			CTransform* pTransform_Terrain = static_cast<CTransform*>(pTerrain->Get_Component(g_strComTransformTag));
//
//			if (nullptr != pVIBuffer_Terrain
//				&& nullptr != pTransform_Terrain)
//			{
//				pVIBuffer_Terrain->Compute_Picking(pTransform_Terrain, &vPickPos);
//			}
//		}
//	}
//
//	ImGui::Text("PickPos : %f \n PickPos : %f \n PickPos : %f", vPickPos.x, vPickPos.y, vPickPos.z);*/
//
//	const list<CGameObject*>* pMonsterList = m_pGameInstance->Get_LayerList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));
//
//	if (nullptr != pMonsterList)
//	{
//		if (false == pMonsterList->empty())
//		{
//			CGameObject* pMonster = pMonsterList->front();
//
//			if (nullptr != pMonster)
//			{
//				CModel* pModel = static_cast<CModel*>(pMonster->Get_Component(g_strComModelTag));
//
//				if (nullptr != pModel)
//				{
//					vector<string> BoneNames = pModel->Get_BoneNames();
//					vector<_float4> BoneTranslations = pModel->Get_Translations();
//
//					_uint iNumBones = (_uint)BoneNames.size();
//					for (_uint i = 0; i < iNumBones; ++i)
//					{
//						string BoneName = BoneNames[i];
//						_float4 BoneTranslation = BoneTranslations[i];
//
//						ImGui::Text((string("BoneName : ") + BoneName).c_str());
//						ImGui::Text("Translation X : %f, Y : %f, Z : %f", BoneTranslation.x, BoneTranslation.y, BoneTranslation.z);
//					}
//				}
//			}
//		}		
//	}

	ImGui::End();
}

HRESULT CDebuger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}
CDebuger* CDebuger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CDebuger* pInstance = new CDebuger(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Create : CDebuger"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebuger::Free()
{
	__super::Free();
}
