// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSProject/Public/TPSPlayer.h"

#include "Bullet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// TPS 카메라를 SpringArm 컴포넌트에 부착
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	springArmComp->SetupAttachment(RootComponent); // CapsuleComponent
	springArmComp->SetRelativeLocation(FVector(.0f, 70.0f, 90.0f)); // 암 컴포넌트의 시작점
	springArmComp->TargetArmLength = 400.0f;
	
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	cameraComp->SetupAttachment(springArmComp);
	
	// 총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh Component"));
	// 캐릭터 메시 컴포넌트(GetMesh()) 부모에 부착
	gunMeshComp->SetupAttachment(GetMesh());
	// LineTracd가 총에 막히지 않도록 충돌 해제
	gunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 스켈레탈메시 데이터 동적로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/GrenadeLauncher/Meshes/SKM_GrenadeLauncher.SKM_GrenadeLauncher'"));
	if (TempGunMesh.Succeeded())
	{
		// 해당 경로의 스켈레탈메시를 찾았다면, 메시 할당 + 임시 위치 보정
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-14.f,52.f,120.f));
	}
	
	// 스나이퍼 총 스태틱메시 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sniper Gun Component"));
	// 캐릭터 메시 컴포넌트 부모에 부착
	sniperGunComp->SetupAttachment(GetMesh());
	// LineTracd가 총에 막히지 않도록 충돌 해제
	sniperGunComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 스태틱메시 데이터 동적로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/Weapons/Sniper/source/sniper1.sniper1'"));
	if (TempSniperGunMesh.Succeeded())
	{
		// 해당 경로의 스태틱메시를 찾았다면, 메시 할당 + 임시 위치 보정
		sniperGunComp->SetStaticMesh(TempSniperGunMesh.Object);
		sniperGunComp->SetRelativeLocation(FVector(-14.f,52.f,120.f));
		sniperGunComp->SetRelativeScale3D(FVector(.8f));
	}
	
	// (개발용) 시작 시 기본 무기로 스나이퍼건을 사용 (유탄총을 숨김)
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
	
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
	
	// 스나이퍼 UI 위젯 인스턴스 생성 (화면에 보이기 위해서는 AddToViewport() 호출이 필요)
	sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	// 일반조준 크로스헤어 UI 위젯 인스턴스 생성. AddtoViewport()로 호출
	crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	if (crosshairUI)
	{
		crosshairUI->AddToViewport();
	}
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 플레이어 이동 처리
	// GetControlRotation() 함수는 좌우(YAW) 말고도 상하(PITCH)까지 포함된 카메라 전체 회전을 가져옴
	// PITCH 움직임에 X축을 기울이는 성질이 있어서 카메라가 아래를 향하는 상태에서 전방 이동시,
	// "앞으로 가는 이동 입력" + "아래로 박는 방향 입력"이 섞임 -> 수평 속도가 cos(Pitch)로 줄어듬.
	
	// 컨트롤러의 현재 회전 값들(YAW, PITCH, ROLL)을 가져온 후 분할하여 필요한 것만 이동시 이용.
	FRotator controlRot = GetControlRotation();
	controlRot.Pitch = 0.0f;
	controlRot.Roll = 0.0f;
	
	direction = FRotationMatrix(controlRot).TransformFVector4(direction);

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
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);
		PlayerInput->BindAction(ia_SniperZoom, ETriggerEvent::Started, this, &ATPSPlayer::SniperZoom);
		//PlayerInput->BindAction(ia_SniperZoom, ETriggerEvent::Completed, this, &ATPSPlayer::SniperZoom);
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

// 총알발사 입력에 따른 콜백 함수 구현
void ATPSPlayer::InputFire(const struct FInputActionValue& inputValue)
{
	if (bUsingGrenadeGun)
	{
		// 유탄총을 사용하는 경우
		// 총 스켈레탈메시에 FirePosition이란 이름의 소켓의 월드 transform(위치/회전)을 가져옴
		//FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		FVector spawnLocation = gunMeshComp->GetSocketLocation(TEXT("FirePosition"));
		FRotator spawnRotation = cameraComp->GetComponentRotation();
		// 위 위치/회전으로 BulletFactory가 BP_Bullet 인스턴스를 월드에 스폰
		GetWorld()->SpawnActor<ABullet>(bulletFactory,spawnLocation,spawnRotation);
	}
	else
	{
		// 스나이퍼건을 사용하는 경우
		// 라인의 시작/종료 위치 설정 - 카메라부터 카메라 정면 50m 까지
		FVector startPosition = cameraComp->GetComponentLocation();
		FVector endPosition = startPosition + cameraComp->GetForwardVector() * 5000.f; //50m
		
		// 충돌 결과 저장, 자기자신은 충돌 검사에서 제외
		FHitResult hitResult;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		
		// LineTraceSingleByChannel(결과그릇, 시작위치, 종료위치, 트레이스채널, 충돌옵션)
		// Visibility 채널로 라인트레이스를 실행하고, 처음 부딫힌 액터 하나만 검출
		bool bHit= GetWorld()->LineTraceSingleByChannel(hitResult,startPosition,endPosition,ECC_Visibility, params);
		
		// [DEBUG] LineTrace 경로 시각화 - 빨강(미충돌) / 초록(충돌)
		DrawDebugLine(GetWorld(),startPosition,endPosition,bHit ? FColor::Green : FColor::Red, false, 1.f, 0,.2f);
		
		if (bHit)
		{
			// [DEBUG] 충돌 정보
			/*
			UE_LOG(LogTemp, Warning, TEXT("HIT: Actor=%s, Component=%s, Distance=%.1f, ImpactPoint=%s"),
				hitResult.GetActor() ? *hitResult.GetActor()->GetName() : TEXT("None"),
				hitResult.GetComponent() ? *hitResult.GetComponent()->GetName() : TEXT("None"),
				hitResult.Distance,
				hitResult.ImpactPoint.ToString()
				);
			*/
			// [DEBUG] 타격 위치 시각화
			DrawDebugSphere(GetWorld(),hitResult.ImpactPoint, 20.f, 12, 
				FColor::Yellow, false, 2.f);
			
			// 타격 위치에 Niagara 이펙트 스폰
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),bulletEffectFactory,hitResult.ImpactPoint);
			
			// 타격 물체에 물리 엔진 적용
			UPrimitiveComponent* hitComp= hitResult.GetComponent();
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 1. 조준 방향 : 시작점에서 종료점 방향
				FVector dir = (endPosition - startPosition).GetSafeNormal();
				// 2. 날려보낼 힘 F= ma, 방향 * 질량 * 가속도
				FVector force = dir * hitComp->GetMass() * 20000.f;
				// 3. 타격된 지점에 힘을 적용
				// AddForce(F) : 무게중심에 힘을 적용. 회전없고, 평행이동
				// AddForceAtLocation(F, pos) : 특정위치에 힘을 적용. 회전(토크)가 발생한다. ex 모서리 맞으면 빙글빙글 돌면서 뒤로밀림
				hitComp->AddForceAtLocation(force, hitResult.ImpactPoint);
				
			}
			
		}
		
	}
}

// 유탄총으로 스왑
void ATPSPlayer::ChangeToGrenadeGun(const struct FInputActionValue& inputValue)
{
	if (bSniperZoom==false)
	{
		// 사용 중 플래그를 유탄총으로 변경
		bUsingGrenadeGun = true;
		// 스나이퍼 숨기고 / 유탄총 보이기
		sniperGunComp->SetVisibility(false);
		gunMeshComp->SetVisibility(true);
	}
	
}

// 스나이퍼건으로 스왑
void ATPSPlayer::ChangeToSniperGun(const struct FInputActionValue& inputValue)
{
	// 사용 중 플래그를 유탄총으로 변경
	bUsingGrenadeGun = false;
	// 스나이퍼 숨기고 / 유탄총 보이기
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

// 조준 입력에 따른 콜백 함수 구현
void ATPSPlayer::SniperZoom(const struct FInputActionValue& inputValue)
{
	// 스나이퍼 총이 아닐때는 동작하지 않음
	if (bUsingGrenadeGun)
	{
		return;
	}
	
	if (bSniperZoom==false)
	{
		// 키 누름 - 줌 모드에 진입
		if (crosshairUI)
		{
			crosshairUI->RemoveFromViewport();
		}		
		bSniperZoom = true;
		sniperUI->AddToViewport(); // 조준경 UI 화면에 나타남
		cameraComp->SetFieldOfView(45.f); // FOV 시야각을 좁혀서 줌인 효과
	}
	else
	{
		// 키 해제 - 줌 모드에서 해제
		bSniperZoom = false;
		sniperUI->RemoveFromViewport(); // 조준경 UI 제거
		if (crosshairUI)
		{
			crosshairUI->AddToViewport();
		}
		
		cameraComp->SetFieldOfView(90.f); // FOV 시야각 복구
	}
}
