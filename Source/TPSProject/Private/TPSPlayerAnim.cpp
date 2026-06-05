// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerAnim.h"

#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// 이 Animation Instance를 소유한 폰 (TPSPlayer) 가져오기
	APawn* pawn = TryGetPawnOwner();
	if (pawn == nullptr) return;
	
	// ATPSPlayer 폰의 상태 추출을 하기 위한 캐스팅 (속도, 점프중 등)
	ATPSPlayer* player= Cast<ATPSPlayer>(pawn);
	if (player)
	{
		// 속도가 있는 상태인지
		speed = player->GetVelocity().Size();
		
		// 공중에 떠 있는 상태인지
		UCharacterMovementComponent* movement = player->GetCharacterMovement();
		if (movement)
		{
			isInAir = movement->IsFalling();
		}
	}
	
	// [DEBUG] 프레임마다 변하는 speed 값 출력용
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,0.f,FColor::Green,
			FString::Printf(TEXT("Anim Speed: %f"), speed));
	}
	
	// 
}
