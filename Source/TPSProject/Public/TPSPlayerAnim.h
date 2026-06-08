// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UTPSPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 매 프레임 자동 호출 - 소유 폰의 상태를 읽어서 변수 갱신(Tick)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	// 캐릭터의 현재 전후 이동 속도 (ABP State Machine의 전이 조건에서 사용) - Blend Space의 세로축
	UPROPERTY(BlueprintReadOnly, Category = PlayerAnim)
	float speed = 0.f;
	
	// 캐릭터의 좌우 이동 방향 (양수 = 우측, 음수 = 좌측) - Blend Space의 가로축
	UPROPERTY(BlueprintReadOnly, Category = PlayerAnim)
	float direction = 0.f;
	
	// 캐릭터가 현재 공중에 있는지 여부 (점프/낙하 중)
	UPROPERTY(BlueprintReadOnly, Category = PlayerAnim)
	bool isInAir = false;
	
	// 재생할 발사 애니메이션 몽타주 선언
	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;
	
	// 발사 애니메이션 재생 함수 (TPSPlayer가 발사 할 때 호출)
	void PlayerAttackAnim();
};
