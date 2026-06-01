// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSProject/Public/TPSPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// TPS 카메라를 SpringArm 컴포넌트에 부착
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	springArmComp->SetupAttachment(RootComponent); // CapsuleComponent
	springArmComp->SetRelativeLocation(FVector(.0f, 70.0f, 90.0f)); // 암 컴포넌트의 시작점
	springArmComp->TargetArmLength = 400.0f;
	
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	cameraComp->SetupAttachment(springArmComp);
		
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// Enhanced Input 시스템이 IMC_TPS 사용하도록 설정
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS,0);
		}
	}
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 플레이어 이동 처리
	direction = FTransform(GetControlRotation()).TransformFVector4(direction);

	// 언리얼 엔진에서 제공하는 등속 운동 구현 함수 AddMovementInput()
	AddMovementInput(direction);
	direction = FVector::ZeroVector; // FVector(.0f,.0f,.0f)
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);
	}
	
}

// 상하 회전 입력에 따른 콜백 함수 구현
void ATPSPlayer::LookUp(const struct FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value); // PITCH(Y축) 회전
}

// 좌우 회전 입력에 따른 콜백 함수 구현
void ATPSPlayer::Turn(const struct FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value); // YAW(Z축) 회전
}

// 전후좌우 이동 입력에 따른 콜백 함수 구현
void ATPSPlayer::Move(const struct FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2d>(); // 전달받는 2D 값
	direction.X = value.X;
	direction.Y = value.Y;
}

// 점프 입력에 따른 콜백 함수 구현
void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
{
	Jump(); // ACharacter 클래스가 제공하는 기본 점프 함수 호출
}