#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "HookShotComponent.h"
#include "LockTargetComponent.h"
#include "TargetComponent.h"
#include "GP2_ArrowProjectile.h"
#include "GP2_ArrowCharacter.generated.h"


#pragma region PlayerStates
UENUM(BlueprintType)
enum class EPlayerMovementState : uint8
{
    PMS_Idle,
    PMS_Jumping,
    PMS_Falling,
    PMS_Stun,
    PMS_Wallrunning,
    PMS_Climbing,
    PMS_HookShot,
    PMS_Vaulting,
    PMS_Crouching,
};
#pragma endregion

class UInputComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStunEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAddBombArrow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAddIceArrow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArrowHit, AGP2_ArrowCharacter*, Arrow);

UCLASS()
class GP2_ARROW_API AGP2_ArrowCharacter : public ACharacter
{
    GENERATED_BODY()
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    class USkeletalMeshComponent *Mesh1P;
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    class USkeletalMeshComponent *FP_Gun;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent *FirstPersonCameraComponent;

public:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    AGP2_ArrowCharacter();

    UPROPERTY(EditAnywhere, Category = "Parkour|WallRun")
    UCurveFloat* WallrunCurve;
    UPROPERTY(EditAnywhere, Category = "Parkour|WallRun")
    FTimeline WallrunTimeline;

    float SlowDuration = 0;
    float SlowAmount = 0;

    float MaxWalkSpeed = 0;

    FTimerHandle LungeTimer;

    UFUNCTION(BlueprintCallable, Category = Slow)
    void Slow(float Duration, float Amount);

    UFUNCTION(BlueprintCallable, Category = Slow)
    void ClearSlow();

    UPROPERTY(BlueprintAssignable)
    FStateAction LungeStarted;

	UPROPERTY(BlueprintAssignable)
    FStateAction LungeEnded;

	UPROPERTY(BlueprintAssignable)
	FStateAction Landed;

	UPROPERTY(BlueprintAssignable)
    FStateAction Jumped;

	UPROPERTY(BlueprintAssignable)
    FStateAction WallrunStarted;

	UPROPERTY(BlueprintAssignable)
	FStateAction WallrunEnded;

	UPROPERTY(BlueprintAssignable)
	FStateAction WallrunJumped;

	UPROPERTY(BlueprintAssignable)
	FStateAction WallClimbStarted;
	
    UPROPERTY(BlueprintAssignable)
	FStateAction WallClimbEnded;

	UPROPERTY(BlueprintAssignable)
    FStateAction HookShotInRange;
    EPlayerMovementState PlayerMovementState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Targetting)
	ULockTargetComponent2* LockTargetComponent;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = HookShot)
    UHookShotComponent* HookShotComponent2;

protected:
    virtual void BeginPlay() override;

#pragma region SM_Logic
    void OnPMSEnter(EPlayerMovementState PMS);
    void OnPMSExit(EPlayerMovementState PMS);
    UFUNCTION(BlueprintCallable)
    void SetNextPMS(EPlayerMovementState PMS);

#pragma endregion

public:

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    class UPlayerDelegates* PlayerDelegates;

    UFUNCTION(BlueprintCallable)
    void ResetScore();

    UFUNCTION(BlueprintCallable)
    int GetScore();

	UFUNCTION(BlueprintCallable)
	int GetHealth();

	UFUNCTION(BlueprintCallable)
	int GetArrowsShot();

	UFUNCTION(BlueprintCallable)
	int GetArrowsHit();

    UFUNCTION(BlueprintCallable)
    void OnScoreAdded(int InScore);

	UFUNCTION(BlueprintCallable)
	void OnArrowHit();

    UFUNCTION(BlueprintCallable)
    void OnHitByArrow(AGP2_ArrowProjectile* ArrowRef);

    UFUNCTION()
    bool CanBeTargeted() const;

    UFUNCTION(BlueprintCallable)
    void AddTargetableEnemy(AGP2_ArrowCharacter* Target);

    UFUNCTION(BlueprintCallable)
    void Freeze();

    UFUNCTION(BlueprintCallable)
    void UnFreeze();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Health = 100;

    UPROPERTY(BlueprintReadOnly)
    int Score = 0;

    UPROPERTY(BlueprintReadOnly)
    int ArrowsShot = 0;

	UPROPERTY(BlueprintReadOnly)
	int ArrowsHit = 0;

    UPROPERTY(BlueprintReadWrite)
    AGP2_ArrowProjectile *CurrentProjectile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector GunOffset;

    UPROPERTY(BlueprintAssignable)
    FArrowHit ArrowHit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
    TSubclassOf<class AGP2_ArrowProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    class USoundBase *FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    class UAnimMontage *FireAnimation;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float MovementSpeedForward = 1.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float MovementSpeedBackwards = 0.8f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float MovementSpeedStrafe = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate = 60.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate = 60.0f;

    UPROPERTY(EditAnywhere, Category = Camera)
    uint32 bInvertedCamera : 1;

    uint32 bSpaceIsHeld : 1;
    uint32 bWallRunRight : 1;
    UPROPERTY(EditAnywhere, Category = "Parkour|Debug")
    uint32 bDebugEnabled : 1;
    UPROPERTY(EditAnywhere, Category = "Parkour|WallClimb")
    uint32 bRequireVelocityForWallClimb : 1;
	UPROPERTY(EditAnywhere, Category = "Parkour|Lunge")
	uint32 bCanLunge : 1;



	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|FreezeArrow")
	uint32 bFreezeArrow : 1;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|Climbing")
	uint32 bClimbOnBSPs : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|Climbing")
	uint32 bUseClimbableTag : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|Crouch")
	uint32 bCrouching : 1;

	UPROPERTY()
	FTimerHandle HookShotTimer;

    UPROPERTY(EditAnywhere, Category = "Parkour|WallRun")
    float WallDistance = 125;
    UPROPERTY(EditAnywhere, Category = "Parkour|WallRun")
    float MaxWallRunTime = 5.0f;
    UPROPERTY(EditAnywhere, Category = "Parkour|WallRun")
    float WaitUntilEngageWallRun = 0.1f;
    float WaitUntilEngageWallRunAccu = 0.0f;
    UPROPERTY(EditAnywhere, Category = "Parkour|WallClimb")
    float RequiredVelocityForWallClimb = 0.3f;
    UPROPERTY(EditAnywhere, Category = "Parkour|Climbing")
    float WallClimbTimer = 0.7f;
    float WallClimbAccu = 0.0f;
    UPROPERTY(EditAnywhere, Category = "Parkour|Climbing")
    float WallClimbLaunchForce = 650;
    UPROPERTY(EditAnywhere, Category = "Parkour|WallJump")
    float WallJumpForce = 2050;
    UPROPERTY(EditAnywhere, Category = "Parkour|Parkour")
    float CapsuleRadiusMultiplier = 1.35f;
	UPROPERTY(EditAnywhere, Category = "Parkour|Lunge")
    float LungeForce = 1050.0f;
	


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Parkour|Lunge")
	float LungeCooldown = 4.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|Lunge")
	float LungeAccu = 4.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|WallRunJump")
    float WallRunJumpForce = 3050;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|WallRunJump")
	float WallRunJumpUpFactor = 0.3f;


    UPROPERTY(EditAnywhere, Category = "Parkour|Lunge")
    float LungeUpFactor = .3f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|WallRun")
    float WallRunForce = 1350;

 
    FTimerHandle WallJumpCameraRotation;

    FVector ReverseWallNormal;
    FVector ClimbToLocation;
    FVector ClimbStartingPosition;

    float StunTimer = 0.0f;
    float StunAccu = 0;

    float ForwardAxis = 0;
    float RightAxis = 0;
public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void Stun(float StunDuration);

    UFUNCTION(BlueprintCallable)
    bool IsStunned() const;

    UFUNCTION(BlueprintCallable)
    void AttachHookShot(FVector HookPosition);

	UFUNCTION(BlueprintCallable)
	void DetachHookShot();

    UFUNCTION(BlueprintCallable)
    void UseHookShot();

    UFUNCTION(BlueprintCallable)
        bool HasHookShotPosition() const;

    void OnFireHookShot();

    bool CheckHookShot(FHitResult& HitResult);
   

    UFUNCTION(BlueprintCallable)
    void UpdateState(float DeltaTime);
	void Idle(float DeltaTime);
    void Jumping(float DeltaTime);
    void FallingState(float DeltaTime);
    void Stunned(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnFire();

	void WallClimbing(float DeltaTime);
	bool CheckWallClimb();
    void WallRunning(float DeltaTime);
    bool WallrunTest(float Direction);

    UFUNCTION()
    void WallRunZPos();
    UFUNCTION()
    void EndWallRun();

    FVector JumpDir;
	UFUNCTION()
	void LungeUpdate();
	UFUNCTION()
	void EndLunge();

    void Crouching(float DeltaTime);

    int LungeCounter = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|Lunge")
    float LungeDuration = 0.06f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parkour|Lunge")
    float LungeUpdateRate = 0.02f;


    void VaultState(float DeltaTime);
    void VaultCheck();

	UFUNCTION(BlueprintNativeEvent, Category = Lunge)
    float LungeForceFunc(const FVector& Direction);

    UFUNCTION()
    void WallRunJump();


#pragma region Input
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	UFUNCTION()
	void OnJumpPressed();
	UFUNCTION()
	void OnJumpReleased();
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
    UFUNCTION(BlueprintCallable)
	void TurnAtRate(float Rate);
	UFUNCTION()
	void Turn(float Value);
	UFUNCTION()
	void LookUp(float Value);
	UFUNCTION(BlueprintCallable)
	void LookUpAtRate(float Rate);
    UFUNCTION()
    void OnLockTarget();
    UFUNCTION()
    void OnLockReleased();

    UFUNCTION()
    void ToggleCrouch();

    FVector StickInput;
#pragma endregion Input
};