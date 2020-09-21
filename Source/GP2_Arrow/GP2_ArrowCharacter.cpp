
#include "GP2_ArrowCharacter.h"
#include "GP2_ArrowProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "PlayerDelegates.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include <Engine/Engine.h>


AGP2_ArrowCharacter::AGP2_ArrowCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Create a CameraComponent
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
    Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

    // Create a gun mesh component
    FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
    FP_Gun->SetOnlyOwnerSee(true); // only the owning player will see this mesh
    FP_Gun->bCastDynamicShadow = false;
    FP_Gun->CastShadow = false;
    // FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
    FP_Gun->SetupAttachment(RootComponent);

    FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    FP_MuzzleLocation->SetupAttachment(FP_Gun);
    FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

    GunOffset = FVector(100.0f, 0.0f, 10.0f);

	LockTargetComponent = CreateDefaultSubobject<ULockTargetComponent2>(TEXT("LockTargetComponentvvvv"));
	HookShotComponent2 = CreateDefaultSubobject<UHookShotComponent>(TEXT("HookShotComponentV2"));
	PlayerDelegates = CreateDefaultSubobject<UPlayerDelegates>(TEXT("PlayerDelegates"));
	bClimbOnBSPs = true;
}


void AGP2_ArrowCharacter::Slow(float Duration, float Amount)
{
	if ((Duration > SlowDuration && Amount > SlowAmount) || SlowAmount > Amount)
	{
		if (Amount > 0)
		{
			SlowDuration = Duration;
			SlowAmount = Amount;
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed / Amount;
			if (!bFreezeArrow)
			{
				PlayerDelegates->FreezeStarted.Broadcast();
			}
			bFreezeArrow = true;

		}
	}
}

void AGP2_ArrowCharacter::ClearSlow()
{
	SlowDuration = 0;
	if (SlowAmount > 0)
	{
		float CurrentMaxSpeed = GetCharacterMovement()->GetMaxSpeed();
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		SlowAmount = 0;
		if (bFreezeArrow)
		{
			PlayerDelegates->FreezeEnded.Broadcast();
		}
		bFreezeArrow = false;
	}
}

void AGP2_ArrowCharacter::BeginPlay()
{
    Super::BeginPlay();

	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	LungeAccu = LungeCooldown;
	MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (!HookShotComponent2)
	{
		HookShotComponent2 = FindComponentByClass<UHookShotComponent>();
	}
	HookShotComponent2->HookShotAccu = HookShotComponent2->HookShotCooldown;
	FOnTimelineFloatStatic TimelineCallback;
	FOnTimelineEventStatic TimelineFinishedCallback;

	TimelineCallback.BindUFunction(this, FName("WallRunZPos"));
	TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("EndWallRun") });
	WallrunTimeline.AddInterpFloat(WallrunCurve, TimelineCallback);
	WallrunTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
}

void AGP2_ArrowCharacter::ResetScore()
{
    Score = 0;
}

int AGP2_ArrowCharacter::GetScore()
{
    return Score;
}

int AGP2_ArrowCharacter::GetHealth()
{
	return Health;
}

int AGP2_ArrowCharacter::GetArrowsShot()
{
	return ArrowsShot;
}

int AGP2_ArrowCharacter::GetArrowsHit()
{
	return ArrowsHit;
}

void AGP2_ArrowCharacter::OnScoreAdded(int InScore)
{
    Score += InScore;
}

void AGP2_ArrowCharacter::OnArrowHit()
{   
	ArrowsHit++;
}

void AGP2_ArrowCharacter::OnHitByArrow(AGP2_ArrowProjectile* ArrowRef)
{
	Health -= 100;
	if (Health <= 0)
	{
		if (Score > 0)
		{
			Score -= 1;
			if (ArrowRef)
			{
				if (ArrowRef->Player)
				{
					ArrowRef->Player->Score += 1;
				}
			}
		}
	}
}

bool AGP2_ArrowCharacter::CanBeTargeted() const
{
	return PlayerMovementState != EPlayerMovementState::PMS_Stun;
}

void AGP2_ArrowCharacter::AddTargetableEnemy(AGP2_ArrowCharacter* Target)
{
	if (LockTargetComponent)
	{
		if (Target != nullptr)
		{
			LockTargetComponent->AddTargetableEnemy(Target);
		}
	}
}

void AGP2_ArrowCharacter::Freeze()
{
	bFreezeArrow = true;
}

void AGP2_ArrowCharacter::UnFreeze()
{
	bFreezeArrow = false;
}

void AGP2_ArrowCharacter::OnFire()
{
    if (ProjectileClass != NULL)
    {
        UWorld *const World = GetWorld();
        if (World != NULL)
        {
            const FRotator SpawnRotation = GetControlRotation();
            const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

            FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation, FVector::OneVector);

			FHitResult Result;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			for (int i = 0; i < LockTargetComponent->EnemyPlayers.Num(); i++)
			{
				Params.AddIgnoredActor(LockTargetComponent->EnemyPlayers[i]);
			}
	
			if (!World->LineTraceSingleByChannel(Result, GetActorLocation(), SpawnLocation + SpawnRotation.Vector(), ECC_Visibility, Params))
			{
				CurrentProjectile = World->SpawnActorDeferred<AGP2_ArrowProjectile>(ProjectileClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				CurrentProjectile->Player = this;
				ArrowsShot++;
				UGameplayStatics::FinishSpawningActor(CurrentProjectile, SpawnTransform);
			}
			else
			{
				SpawnTransform.SetLocation(Result.ImpactPoint);
				CurrentProjectile = World->SpawnActorDeferred<AGP2_ArrowProjectile>(ProjectileClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				CurrentProjectile->Player = this;
				CurrentProjectile->HasHitCPP = true;
				ArrowsShot++;
				UGameplayStatics::FinishSpawningActor(CurrentProjectile, SpawnTransform);
				CurrentProjectile->SetActorLocation(Result.ImpactPoint - SpawnRotation.Vector() * 50.0f );
				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CurrentProjectile->FindComponentByClass<UStaticMeshComponent>());
				MeshComp->SetVisibility(true);
				CurrentProjectile->ArrowStuckOnSpawn.Broadcast();
			}
        }
    }

    if (FireSound != NULL)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }

    if (FireAnimation != NULL)
    {
        UAnimInstance *AnimInstance = Mesh1P->GetAnimInstance();
        if (AnimInstance != NULL)
        {
            AnimInstance->Montage_Play(FireAnimation, 1.f);
        }
    }
}

void AGP2_ArrowCharacter::Tick(float DeltaTime)
{
	LungeAccu += DeltaTime;
	Super::Tick(DeltaTime);
	UpdateState(DeltaTime);
	WallrunTimeline.TickTimeline(DeltaTime);

	if (HookShotComponent2)
	{
		if (HookShotComponent2->HookShotAccu >= HookShotComponent2->HookShotCooldown)
		{
			FHitResult HitResult;
			CheckHookShot(HitResult);
		}
	}

}

void AGP2_ArrowCharacter::Stun(float StunDuration)
{
	StunTimer = StunDuration;
	SetNextPMS(EPlayerMovementState::PMS_Stun);
}

void AGP2_ArrowCharacter::AttachHookShot(FVector HookPosition)
{
	HookShotComponent2->HookShotEndPosition = HookPosition;
	HookShotComponent2->bHasHookShotPosition = true;
}

void AGP2_ArrowCharacter::DetachHookShot()
{
	if (PlayerMovementState == EPlayerMovementState::PMS_HookShot)
	{
		HookShotComponent2->bHasHookShotPosition = false;
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		return;
	}
}

void AGP2_ArrowCharacter::UseHookShot()
{
	SetNextPMS(EPlayerMovementState::PMS_HookShot);
}

bool AGP2_ArrowCharacter::HasHookShotPosition() const
{
	return HookShotComponent2->bHasHookShotPosition;
}

void AGP2_ArrowCharacter::OnFireHookShot()
{
	FHitResult HitResult;
	if (CheckHookShot(HitResult))
	{
		AttachHookShot(HitResult.Location);

		HookShotComponent2->HookShotSuccess.Broadcast();
		HookShotComponent2->HookShotAccu = 0;
	}
}

bool AGP2_ArrowCharacter::CheckHookShot(FHitResult& HitResult)
{
	if (PlayerMovementState == EPlayerMovementState::PMS_Falling ||
		PlayerMovementState == EPlayerMovementState::PMS_Idle ||
		PlayerMovementState == EPlayerMovementState::PMS_Jumping ||
		PlayerMovementState == EPlayerMovementState::PMS_Climbing) {
		if (HookShotComponent2->HookShotAccu >= HookShotComponent2->HookShotCooldown)
		{
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			FVector StartLocation = GetActorLocation();
			FVector EndLocation = StartLocation + GetController()->GetControlRotation().Vector() * HookShotComponent2->HookShotDistance;
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 0.1f, 0, 1.0f);
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldDynamic, Params))
			{
				if (HitResult.Actor->IsValidLowLevel())
				{
					if (Cast<AGP2_ArrowCharacter>(HitResult.GetActor()))
					{
						return false;
					}
				}
				HookShotInRange.Broadcast();
				return true;
			}
			else
			{
				HookShotComponent2->HookShotOutOfRange.Broadcast();
			}
		}
		else
		{
			HookShotComponent2->HookShotStillOnCooldown.Broadcast();
		}
	}
	return false;
}


void AGP2_ArrowCharacter::UpdateState(float DeltaTime)
{
	if (SlowDuration > 0)
	{
		SlowDuration -= DeltaTime;
		if (SlowDuration <= 0)
		{
			ClearSlow();
		}
	}

	HookShotComponent2->HookShotAccu += DeltaTime;

	switch (PlayerMovementState)
	{
	case EPlayerMovementState::PMS_Idle:
		Idle(DeltaTime);
		break;
	case EPlayerMovementState::PMS_Jumping:
		Jumping(DeltaTime);
		break;
	case EPlayerMovementState::PMS_Falling:
		FallingState(DeltaTime);
		break;
	case EPlayerMovementState::PMS_Wallrunning:
		WallRunning(DeltaTime);
		break;
	case EPlayerMovementState::PMS_Climbing:
		WallClimbing(DeltaTime);
		break;
	case EPlayerMovementState::PMS_Stun:
		Stunned(DeltaTime);
		break;
	case EPlayerMovementState::PMS_HookShot:
		HookShotComponent2->HookShotUpdate.Broadcast();
		break;
	case EPlayerMovementState::PMS_Vaulting:
		VaultState(DeltaTime);
		break;
	default:
		break;
	}
}

void AGP2_ArrowCharacter::OnPMSEnter(EPlayerMovementState PMS)
{
	switch (PMS)
	{
	case EPlayerMovementState::PMS_Idle:
		break;
	case EPlayerMovementState::PMS_Jumping:
		break;
	case EPlayerMovementState::PMS_Falling:
		break;

	case EPlayerMovementState::PMS_Wallrunning:
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		WallrunTimeline.PlayFromStart();
	}
	break;
	case EPlayerMovementState::PMS_Climbing:
	{
		WallClimbAccu = 0.0f;
	}
		break;
	case EPlayerMovementState::PMS_Stun:
		PlayerDelegates->StunStarted.Broadcast();
		StunAccu = 0;
		break;
	case EPlayerMovementState::PMS_HookShot:
	{
		HookShotComponent2->HookShotStarted.Broadcast();
		HookShotComponent2->HookShotLaunchDir = HookShotComponent2->HookShotEndPosition - GetActorLocation();
		HookShotComponent2->HookShotForceAccu = 0;
	}
		break;
	case EPlayerMovementState::PMS_Vaulting:
		break;
	default:
		break;
	}
}

void AGP2_ArrowCharacter::OnPMSExit(EPlayerMovementState PMS)
{
	switch (PMS)
	{
	case EPlayerMovementState::PMS_Idle:
		break;
	case EPlayerMovementState::PMS_Jumping:
		WaitUntilEngageWallRunAccu = 0;
		break;
	case EPlayerMovementState::PMS_Falling:
		break;
	case EPlayerMovementState::PMS_Wallrunning:
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		WallrunTimeline.Stop();
	}
		break;
	case EPlayerMovementState::PMS_Climbing:
		WallClimbEnded.Broadcast();
		break;
	case EPlayerMovementState::PMS_Stun:
		PlayerDelegates->StunEnded.Broadcast();
		break;
	case EPlayerMovementState::PMS_HookShot:
		HookShotComponent2->HookShotEnded.Broadcast();
		break;
	case EPlayerMovementState::PMS_Vaulting:
		break;
	default:
		break;



	}
}

void AGP2_ArrowCharacter::SetNextPMS(EPlayerMovementState PMS)
{
	OnPMSExit(PlayerMovementState);
	PlayerMovementState = PMS;
	OnPMSEnter(PMS);
}


void AGP2_ArrowCharacter::Jumping(float DeltaTime)
{
	WaitUntilEngageWallRunAccu += DeltaTime;
	if (WaitUntilEngageWallRunAccu >= WaitUntilEngageWallRun)
	{
		if (bDebugEnabled)
		{
			//GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Cyan, GetVelocity().ToString());
		}

		if (CheckWallClimb())
		{
			WallClimbStarted.Broadcast();
			SetNextPMS(EPlayerMovementState::PMS_Climbing);
			return;
		}
		FVector Velocity = GetVelocity();
		//Velocity.Z = 0;
		Velocity.Normalize(SMALL_NUMBER);
		float ForwardVelocity = FVector::DotProduct(GetActorForwardVector(), Velocity);
		if (ForwardVelocity > 0.7f && bSpaceIsHeld)
		{
			//Right Test
			if (WallrunTest(WallDistance) )
			{
				WallrunStarted.Broadcast();
				SetNextPMS(EPlayerMovementState::PMS_Wallrunning);
				bWallRunRight = true;
				return;
			}

			//Left test
			if (WallrunTest(-WallDistance))
			{
				WallrunStarted.Broadcast();
				SetNextPMS(EPlayerMovementState::PMS_Wallrunning);
				bWallRunRight = false;
				return;
			}
		}
	}
	if (GetVelocity().Z < -0.15f || CanJump())
	{
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		return;
	}
}

bool AGP2_ArrowCharacter::CheckWallClimb()
{
	if (bFreezeArrow)
	{
		return false;
	}

	FVector Velocity = GetVelocity();
	//Velocity.Z = 0;
	Velocity.Normalize(SMALL_NUMBER);
	float ForwardVelocity = FVector::DotProduct(GetActorForwardVector(), Velocity);
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() * CapsuleRadiusMultiplier;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	if (bDebugEnabled)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true);
	}
 	if (ForwardAxis > 0.1f)
	{
		if (ForwardVelocity > RequiredVelocityForWallClimb || !bRequireVelocityForWallClimb)
		{
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldDynamic, Params))
			{
				if (HitResult.Actor->IsValidLowLevel())
				{
					if (!bUseClimbableTag || HitResult.GetActor()->ActorHasTag(FName("Climbable")))
					{
						if (bSpaceIsHeld)
						{
							ReverseWallNormal = -HitResult.ImpactNormal;
							return true;
						}
					}
				}
				else if(bClimbOnBSPs)
				{
					if (bSpaceIsHeld)
					{
						ReverseWallNormal = -HitResult.ImpactNormal;
						return true;
					}
				}
			}
		}
	}
	return false;
}


void AGP2_ArrowCharacter::FallingState(float DeltaTime)
{
	if (bSpaceIsHeld)
	{
		if (CheckWallClimb())
		{
			WallClimbStarted.Broadcast();
			SetNextPMS(EPlayerMovementState::PMS_Climbing);
			return;
		}

		//if (WallrunTest(WallDistance))
		//{
		//	SetNextPMS(EPlayerMovementState::PMS_Wallrunning);
		//	return;
		//}
	}
	
	if (!GetCharacterMovement()->IsFalling())
	{
		SetNextPMS(EPlayerMovementState::PMS_Idle);
		bCanLunge = true;
		return;
	}
}

void AGP2_ArrowCharacter::Idle(float DeltaTime)
{
	if (CheckWallClimb())
	{
		WallClimbStarted.Broadcast();
		SetNextPMS(EPlayerMovementState::PMS_Climbing);
		return;
	}
}


void AGP2_ArrowCharacter::WallClimbing(float DeltaTime)
{
	FaceRotation(ReverseWallNormal.ToOrientationRotator());
	LaunchCharacter(FVector(0, 0, WallClimbLaunchForce), true, true);
	WallClimbAccu += DeltaTime;
	if (WallClimbAccu >= WallClimbTimer && bSpaceIsHeld)
	{
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		return;
	}

	if (bFreezeArrow)
	{
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		return;
	}

	FHitResult HitResult;

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() * CapsuleRadiusMultiplier * 1.5f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (bDebugEnabled)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true);
	}
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldDynamic, Params) || !bSpaceIsHeld)
	{
		SetNextPMS(EPlayerMovementState::PMS_Falling);
	}
}

void AGP2_ArrowCharacter::Stunned(float DeltaTime)
{
	StunAccu += DeltaTime;
	if (StunAccu >= StunTimer)
	{
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		PlayerDelegates->StunEnded.Broadcast();
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

bool AGP2_ArrowCharacter::IsStunned() const
{
	return StunAccu < StunTimer;
}

void AGP2_ArrowCharacter::MoveForward(float Value)
{
	ForwardAxis = Value;
	if (Value > 0)
	{
		AddMovementInput(GetActorForwardVector(), MovementSpeedForward);
	}
	else if (Value < 0)
	{
		AddMovementInput(-GetActorForwardVector(), MovementSpeedBackwards);
	}
}

void AGP2_ArrowCharacter::MoveRight(float Value)
{
	RightAxis = Value;
	if (Value != 0)
	{
		AddMovementInput(Value > 0 ? GetActorRightVector() : -GetActorRightVector(), MovementSpeedStrafe);
	}
}

void AGP2_ArrowCharacter::OnJumpPressed()
{
	bSpaceIsHeld = true;
	if (PlayerMovementState == EPlayerMovementState::PMS_HookShot)
	{
		DetachHookShot();
	}
	if (PlayerMovementState == EPlayerMovementState::PMS_Wallrunning)
	{
		WallRunJump();
		WallrunJumped.Broadcast();
		SetNextPMS(EPlayerMovementState::PMS_Jumping);
		return;
	}

	if (PlayerMovementState == EPlayerMovementState::PMS_Idle)
	{
		Jump();
		Jumped.Broadcast();
		SetNextPMS(EPlayerMovementState::PMS_Jumping);
		return;
	}
	if (bCanLunge && LungeAccu >= LungeCooldown)
	{
		FVector InputDirection = GetActorRotation().Vector();
		if (ForwardAxis != 0 || RightAxis != 0)
		{
			InputDirection = FVector(ForwardAxis, RightAxis, 0);
			FRotator Rot = GetActorRotation();
			InputDirection = Rot.RotateVector(InputDirection);
		}
		if (PlayerMovementState == EPlayerMovementState::PMS_Jumping || PlayerMovementState == EPlayerMovementState::PMS_Falling)
		{
			JumpDir = InputDirection;
			JumpDir.Normalize(SMALL_NUMBER);

			LungeCounter = 0;
			FTimerDelegate Deleg;
			Deleg.BindUFunction(this, FName("LungeUpdate"));
			GetWorld()->GetTimerManager().SetTimer(LungeTimer, Deleg, LungeUpdateRate, true);
			PlayerDelegates->LungeStarted.Broadcast();


			bCanLunge = false;
			LungeAccu = 0;
			return;
		}
	}
}

void AGP2_ArrowCharacter::OnJumpReleased()
{
	bSpaceIsHeld = false;
}

#pragma region WallRun

void AGP2_ArrowCharacter::WallRunning(float DeltaTime)
{
	WallrunTimeline.Play();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorRightVector() * (bWallRunRight ? WallDistance : -WallDistance);
	if (bDebugEnabled)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true);
	}
	FHitResult WallCheck;
	if (!GetWorld()->LineTraceSingleByChannel(WallCheck, StartLocation, EndLocation, ECC_WorldDynamic, Params))
	{
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		return;
	}
	if (bWallRunRight)
	{
		FVector Forward = FVector::CrossProduct(GetActorUpVector(), WallCheck.ImpactNormal);
		LaunchCharacter(Forward * WallRunForce, true, true);
		SetActorRotation(Forward.ToOrientationRotator());
	}
	else
	{
		FVector Forward = FVector::CrossProduct(WallCheck.ImpactNormal, GetActorUpVector());
		LaunchCharacter(Forward * WallRunForce, true, true);
		SetActorRotation(Forward.ToOrientationRotator());
	}



	if (ForwardAxis < 0.3f) {
		SetNextPMS(EPlayerMovementState::PMS_Falling);
		return;
	}
}

bool AGP2_ArrowCharacter::WallrunTest(float Direction)
{
	float Dir = Direction > 0 ? 1 : -1;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorRightVector() * Direction;
	if (bDebugEnabled)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true);
	}
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldDynamic, Params))
	{
		if (HitResult.Actor->IsValidLowLevel())
		{
			if (!HitResult.Actor->ActorHasTag("WallRunnable"))
			{
				return false;
			}
		}
		float Dot = FVector::DotProduct(HitResult.ImpactNormal, GetActorRightVector() * Dir);
		if (Dot < -0.7f)
		{
			FVector DiagonalDown = GetActorRightVector() * Dir + -FVector::UpVector;
			DiagonalDown.Normalize(SMALL_NUMBER);
			DiagonalDown *= FMath::Abs(Direction) * 1.334f;
			DiagonalDown += StartLocation;
			FVector DiagonalUp = GetActorRightVector() * Dir + FVector::UpVector;
			DiagonalUp.Normalize(SMALL_NUMBER);
			DiagonalUp *= FMath::Abs(Direction) * 1.334f;
			DiagonalUp += StartLocation;

			FVector DiagonalForward = (GetActorForwardVector() * 2.0f) + (GetActorRightVector() * Dir);
			DiagonalForward.Normalize(SMALL_NUMBER);
			DiagonalForward *= FMath::Abs(Direction) * 1.334f;
			DiagonalForward += StartLocation;
			if (bDebugEnabled)
			{
				DrawDebugLine(GetWorld(), StartLocation, DiagonalDown, FColor::Blue, false, 3.0f);
				DrawDebugLine(GetWorld(), StartLocation, DiagonalForward, FColor::Blue, false, 3.0f);
				DrawDebugLine(GetWorld(), StartLocation, DiagonalUp, FColor::Blue, false, 3.0f);
			}
			FHitResult DownResult;
			FHitResult UpResult;
			FHitResult ForwardResult;

			bool Down = GetWorld()->LineTraceSingleByChannel(DownResult, StartLocation, DiagonalDown, ECC_WorldDynamic, Params);
			bool Up = GetWorld()->LineTraceSingleByChannel(UpResult, StartLocation, DiagonalUp, ECC_WorldDynamic, Params);
			bool Forward = GetWorld()->LineTraceSingleByChannel(ForwardResult, StartLocation, DiagonalForward, ECC_WorldDynamic, Params);

			if (Down && Up && Forward)
			{
				float DownDot = FVector::DotProduct(DownResult.ImpactNormal, GetActorRightVector() * Dir);
				float UpDot = FVector::DotProduct(ForwardResult.ImpactNormal, GetActorRightVector() * Dir);
				float ForwardDot = FVector::DotProduct(UpResult.ImpactNormal, GetActorRightVector() * Dir);
				if (DownDot < -0.7f && UpDot < -0.7f && ForwardDot < -0.7f)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void AGP2_ArrowCharacter::WallRunZPos()
{
	FVector ActorLoc = GetActorLocation();
	auto Pos = WallrunTimeline.GetPlaybackPosition();
	SetActorLocation(FVector(ActorLoc.X, ActorLoc.Y, ActorLoc.Z + WallrunCurve->GetFloatValue(Pos) * 50), false, nullptr, ETeleportType::TeleportPhysics);
}

void AGP2_ArrowCharacter::EndWallRun()
{
	WallrunEnded.Broadcast();
	SetNextPMS(EPlayerMovementState::PMS_Falling);
	return;
}

void AGP2_ArrowCharacter::LungeUpdate()
{
	if (LungeCounter > LungeDuration / LungeUpdateRate)
	{
		PlayerDelegates->LungeEnded.Broadcast();
		GetWorld()->GetTimerManager().ClearTimer(LungeTimer);
		return;
	}

	LungeCounter++;
	if (PlayerMovementState == EPlayerMovementState::PMS_Falling || PlayerMovementState == EPlayerMovementState::PMS_Jumping)
	{
		LaunchCharacter(JumpDir * LungeForceFunc(JumpDir), true, true);
	}
	else
	{
		PlayerDelegates->LungeEnded.Broadcast();
		GetWorld()->GetTimerManager().ClearTimer(LungeTimer);
	}
}

void AGP2_ArrowCharacter::EndLunge()
{
}

void AGP2_ArrowCharacter::Crouching(float DeltaTime)
{
}

void AGP2_ArrowCharacter::VaultState(float DeltaTime)
{
	LaunchCharacter((FVector::UpVector) * 500, true, true);
	SetNextPMS(EPlayerMovementState::PMS_Falling);
	return;
}


void AGP2_ArrowCharacter::VaultCheck()
{
	FHitResult Result;
	FVector StartLoc = GetActorLocation();
	float VaultDistance = GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.0f;
	FVector EndLoc = StartLoc + GetActorForwardVector() * VaultDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(Result, StartLoc, EndLoc, ECC_WorldDynamic, Params))
	{
		if (Result.Actor->IsValidLowLevel())
		{
			if (Result.Actor->ActorHasTag(FName("Vaultable")))
			{
				SetNextPMS(EPlayerMovementState::PMS_Vaulting);
				return;
			}
		}
	}
}


float AGP2_ArrowCharacter::LungeForceFunc_Implementation(const FVector& Direction)
{
	return 0;
}

#pragma endregion WallRun


void AGP2_ArrowCharacter::WallRunJump()
{
	FVector RightVector = (bWallRunRight ? -1.0f : 1.0f) * GetActorRightVector();
	RightVector += WallRunJumpUpFactor * FVector::UpVector;
	RightVector += GetActorForwardVector() * 0.5f;
	RightVector.Normalize(SMALL_NUMBER);
	LaunchCharacter(RightVector * WallRunJumpForce, true, true);
}

void AGP2_ArrowCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGP2_ArrowCharacter::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AGP2_ArrowCharacter::OnJumpReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGP2_ArrowCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGP2_ArrowCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AGP2_ArrowCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGP2_ArrowCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AGP2_ArrowCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGP2_ArrowCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("LockTarget", IE_Pressed, this, &AGP2_ArrowCharacter::OnLockTarget);
	PlayerInputComponent->BindAction("LockTarget", IE_Released, this, &AGP2_ArrowCharacter::OnLockReleased);

	PlayerInputComponent->BindAction("HookShot", IE_Pressed, this, &AGP2_ArrowCharacter::OnFireHookShot);
	PlayerInputComponent->BindAction("ReleaseHookShot", IE_Pressed, this, &AGP2_ArrowCharacter::DetachHookShot);

	PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &AGP2_ArrowCharacter::VaultCheck);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGP2_ArrowCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AGP2_ArrowCharacter::ToggleCrouch);
}


void AGP2_ArrowCharacter::TurnAtRate(float Rate)
{
	StickInput.X = Rate;
	if (Rate != 0)
	{
		LockTargetComponent->Offset.X += Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
		if (LockTargetComponent->Offset.X > LockTargetComponent->Max)
		{
			LockTargetComponent->Offset.X = LockTargetComponent->Max;
		}
		else if (LockTargetComponent->Offset.X < -LockTargetComponent->Max)
		{
			LockTargetComponent->Offset.X = -LockTargetComponent->Max;
		}
	}
	if (LockTargetComponent->TargetedCharacter)
	{
		
	}
	else
	{
		AddControllerYawInput(Rate);
	}
}

void AGP2_ArrowCharacter::Turn(float Value)
{
	LockTargetComponent->Offset.X += Value * GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Value);
}

void AGP2_ArrowCharacter::LookUp(float Value)
{
	LockTargetComponent->Offset.Z -= Value * GetWorld()->GetDeltaSeconds();
	AddControllerPitchInput(Value);
}

void AGP2_ArrowCharacter::LookUpAtRate(float Rate)
{
	if (Rate != 0)
	{
		LockTargetComponent->Offset.Z -= Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
		if (LockTargetComponent->Offset.Z > LockTargetComponent->Max)
		{
			LockTargetComponent->Offset.Z = LockTargetComponent->Max;
		}
		else if (LockTargetComponent->Offset.Z < -LockTargetComponent->Max)
		{
			LockTargetComponent->Offset.Z = -LockTargetComponent->Max;
		}
	}
	StickInput.Y = Rate;
	if (LockTargetComponent->TargetedCharacter)
	{

	}
	else
	{
		AddControllerPitchInput((bInvertedCamera ? -Rate : Rate));
	}
	
}

void AGP2_ArrowCharacter::OnLockTarget()
{
	LockTargetComponent->LockTarget();
}

void AGP2_ArrowCharacter::OnLockReleased()
{
	LockTargetComponent->UnLockTarget();
}

void AGP2_ArrowCharacter::ToggleCrouch()
{
	if (CanCrouch())
	{
		bCrouching = true;
		Crouch();
	}
	else
	{
		bCrouching = false;
		UnCrouch();
	}
}

