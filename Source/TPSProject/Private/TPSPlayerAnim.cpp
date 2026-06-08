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
		// 속도, 전후좌우 벡터가져오기
		FVector velocity = player->GetVelocity();
		FVector forward = player->GetActorForwardVector();
		FVector right = player->GetActorRightVector();
		
		speed = FVector::DotProduct(velocity, forward); // 앞=양수, 뒤=음수
		direction = FVector::DotProduct(velocity, right); // 우=양수, 좌=음수
		
		// 공중에 떠 있는 상태인지
		UCharacterMovementComponent* movement = player->GetCharacterMovement();
		if (movement)
		{
			isInAir = movement->IsFalling();
		}
	}
	
	// [DEBUG] 프레임마다 변하는 변수 값 출력용
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,0.f,FColor::Green,
			FString::Printf(TEXT("Speed: %.1f, direction: %.1f, isInAir: %s"), 
				speed, direction, isInAir ? TEXT("true") : TEXT("false")));
	}
	
	// 
}
