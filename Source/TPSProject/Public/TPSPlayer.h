// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 스프링 암 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;
	
	// 카메라 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* cameraComp;
	
	// 총 스켈레탈 메시 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;
	
	// 스나이퍼 총 스태틱 메시 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;
	
	// IMC 선택 필드 선언
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* imc_TPS;
	
	// 상하 마우스 회전IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_LookUp;
	
	// 좌우 마우스 회전IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Turn;

	// 전후좌우 키보드 이동 IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Move;
	
	// 키보드 점프 IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Jump;
	
	// 마우스 발사 IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Fire;
	
	// 유탄총 교체 IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_GrenadeGun;
	
	// 스나이퍼건 교체 IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_SniperGun;
	
	// 스나이퍼건 줌 IA 필드
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_SniperZoom;
	
	// 현재 유탄총을 사용중인지 여부 (true : 유탄총 / false : 스나이퍼건)
	bool bUsingGrenadeGun = true;
	
	// 현재 스나이퍼 줌(조준) 중인지 여부 (FOV 줌 + UI 표시되는 상태)
	bool bSniperZoom = false;
	
	// 런타임에 생성될 UI 인스턴스
	UPROPERTY()
	class UUserWidget* sniperUI;
	
	// 이동 속도
	UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
	float walkSpeed = 600.f;
	
	// 총알 스폰 팩토리
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;
	
	// 스나이퍼 줌 UI 위젯 팩토리 (WBP_SniperUI를 에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;
	
	// 이동 방향
	FVector direction;
	
	// 상하 회전 입력 함수 선언
	void LookUp(const struct FInputActionValue& inputValue);
	
	// 좌우 회전 입력 함수 선언
	void Turn(const struct FInputActionValue& inputValue);
	
	// 전후좌우 이동 입력 함수 선언
	void Move(const struct FInputActionValue& inputValue);
	
	// 점프 입력 함수 선언
	void InputJump(const struct FInputActionValue& inputValue);
	
	// 총알 발사 입력 함수 선언
	void InputFire(const struct FInputActionValue& inputValue);
	
	// 유탄총으로 교체 입력 함수 선언
	void ChangeToGrenageGun(const struct FInputActionValue& inputValue);
	
	// 스나이퍼건으로 교체 입력 함수 선언
	void ChangeToSniperGun(const struct FInputActionValue& inputValue);
	
	// 스나이퍼건 줌 입력 함수 선언
	void SniperZoom(const struct FInputActionValue& inputValue);
	
	// 스나이퍼 사격 명중 시 표시할 총알 파편 효과 선언
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect)
	class UNiagaraSystem* bulletEffectFactory;
	
};
