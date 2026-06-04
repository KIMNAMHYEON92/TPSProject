// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "Enemy.h"
#include "TPSPlayer.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 월드에서 ATPSPlayer 타겟 찾기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(),ATPSPlayer::StaticClass());
	// 찾은 actor를 ATPSPlayer 타입으로 캐스팅
	target = Cast<ATPSPlayer>(actor);
	// 소유 객체
	me = Cast<AEnemy>(GetOwner());
	
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// [DEBUG]
	if (GEngine)
	{
		FString logMsg = UEnum::GetValueAsString(mState);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, logMsg);
	}

	// 현재 상태에 따라 해상 함수만 실행 - FSM
	switch (mState)
	{
	case EEnemyState::Idle: IdleState(); break;
	case EEnemyState::Move: MoveState(); break;
	case EEnemyState::Attack: AttackState(); break;
	case EEnemyState::Damage: DamageState(); break;
	case EEnemyState::Die: DieState(); break;
	}
}

// 대기 상태 : 일정 시간 후 추적으로 전환
void UEnemyFSM::IdleState()
{
	// 시간 누적
	currentTime+= GetWorld()->GetDeltaSeconds();
	
	if (currentTime > idleDelayTime)
	{
		// Move 상태로 전환
		mState = EEnemyState::Move;
		// 경과 시간 초기화
		currentTime = 0.f;
	}
}

// 추적 상태 : 플레이어 방향으로 이동, 공격 범위 진입 시 공격 상태로 전환
void UEnemyFSM::MoveState()
{
	// 타겟 목적지 설정
	FVector destination = target->GetActorLocation();
	// 타겟 방향 설정
	FVector direction = destination - me->GetActorLocation();
	// 타겟 방향으로 이동
	me->AddMovementInput(direction.GetSafeNormal());
	
	// Attack 상태로 전환 (공격범위 안으로 들어오면)
	if (direction.Size() <= attackRange)
	{
		mState = EEnemyState::Attack;
	}
}

// 공격 상태 : 일정 주기로 공격, 플레이어가 공격 범위 이탈 시 추적 상태로 전환
void UEnemyFSM::AttackState()
{
	// 주기적으로 플레이어를 공격(로그)
	currentTime+= GetWorld()->GetDeltaSeconds();
	
	if (currentTime > attackDelayTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack!"));
		currentTime = 0.f;
	}
	
	// 타겟이 공격 범위를 벗어나면, Move 상태로 전이
	float distance = FVector::Distance(me->GetActorLocation(), target->GetActorLocation());
	if (distance > attackRange)
	{
		mState = EEnemyState::Move;
	}
}

// 피격 상태 : 잠시 멈춤 후 대기로 복귀
void UEnemyFSM::DamageState()
{
	currentTime+= GetWorld()->GetDeltaSeconds();
	if (currentTime > damagedDelayTime)
	{
		mState = EEnemyState::Idle;
		currentTime = 0.f;
	}
}

//
void UEnemyFSM::OnDamagedProcess()
{
	// 체력 감소
	hp--;
	// 체력이 남아있다면
	if (hp>0)
	{
		mState = EEnemyState::Damage;
	}
	else
	{
		mState = EEnemyState::Die;
	}
}

// 사망 상태 : 사망 처리 (종착 상태, 더이상 전이 없음)
void UEnemyFSM::DieState()
{
	
}


