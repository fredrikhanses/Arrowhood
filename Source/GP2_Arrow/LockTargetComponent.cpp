#include "LockTargetComponent.h"
#include "GP2_ArrowCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

ULockTargetComponent2::ULockTargetComponent2()
{
	PrimaryComponentTick.bCanEverTick = true;
	bUseMarkers = true;
	TargettedHitMarkerIndex = -1;

}

void ULockTargetComponent2::RemoveTargetableTarget(AActor* Target)
{
	if (TargetableTargets.Contains(Target))
	{
		TargetableTargets.Remove(Target);
	}
}

void ULockTargetComponent2::AddTargetableTarget(AActor* Target)
{
	if (!TargetableTargets.Contains(Target))
	{
		TargetableTargets.Add(Target);
	}
}

void ULockTargetComponent2::LockTarget()
{
	bToggle = true;
	TargetedCharacter = nullptr;
	Offset = FVector::ZeroVector;

	if (GetWorld()->GetTimerManager().IsTimerActive(LockTargetHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(LockTargetHandle);
	}
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("FindTarget"));
	GetWorld()->GetTimerManager().SetTimer(LockTargetHandle, TimerDelegate, UpdateRate, true, 0.0f);
}

void ULockTargetComponent2::LockTargetFunc()
{
	if (TargetedCharacter && !TargettedTarget)
	{
		if (!TargetedCharacter->CanBeTargeted())
		{
			TargetedCharacter = nullptr;
			StoppedTargetingEnemy.Broadcast();
			Offset = FVector::ZeroVector;
		}
		else
		{
			DistanceToTarget = FVector::Distance(Owner->GetActorLocation(), TargetedCharacter->GetActorLocation());
			if (DistanceToTarget < MaxTargetingDistance && DistanceToTarget > MinTargetingDistance)
			{
				FHitResult HitResult;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Owner);
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargetedCharacter->GetActorLocation());
				FVector Dir = LookAt.Vector();
				FVector Dir2 = Owner->GetController()->GetControlRotation().Vector();
				float Dot = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Dir, Dir2))));
				if (Dot < MaxDegrees)
				{
					if (GetWorld()->LineTraceSingleByChannel(
						HitResult,
						Owner->GetActorLocation() + FVector(0, 0, Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
						TargetedCharacter->GetActorLocation(), ECC_WorldDynamic, Params))
					{
						if (HitResult.Actor == TargetedCharacter)
						{
							FVector RightDirection = FVector::CrossProduct(Owner->GetController()->GetControlRotation().Vector(), FVector::UpVector);
							FVector RealOffset = RightDirection * -Offset.X * Sensitivity;
							RealOffset += Owner->GetActorUpVector() * Offset.Z * Sensitivity;

							FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargetedCharacter->GetActorLocation() + RealOffset);

							float AngleBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Owner->GetController()->GetControlRotation().Vector(), Rot.Vector())));
	
							Owner->GetController()->SetControlRotation(
								UKismetMathLibrary::RInterpTo(
									Owner->GetController()->GetControlRotation(),
									Rot,
									GetWorld()->GetDeltaSeconds(),
									MaxCameraRotationPerSecond));




							for (int i = 0; i < HitMarkers.Num(); i++)
							{
								if (HitMarkerPlayerPtr[i])
								{
									if (HitMarkerPlayerPtr[i] == HitResult.GetActor())
									{
										TargettedHitMarkerIndex = i;
										TargettedHitMarker = HitMarkers[i];
									}
								}
							}
							return;
						}
					}
				}
			}
			TargettedHitMarker = nullptr;
			TargettedHitMarkerIndex = -1;
			TargetedCharacter = nullptr;
			StoppedTargetingEnemy.Broadcast();
		}
	}
	if (TargettedTarget)
	{
		DistanceToTarget = FVector::Distance(Owner->GetActorLocation(), TargettedTarget->GetActorLocation());
		if (DistanceToTarget < MaxTargetingDistance && DistanceToTarget > MinTargetingDistance)
		{
			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Owner);
			FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargettedTarget->GetActorLocation());
			FVector Dir = LookAt.Vector();
			FVector Dir2 = Owner->GetController()->GetControlRotation().Vector();
			float Dot = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Dir, Dir2))));
			if (Dot < MaxDegrees)
			{
				if (GetWorld()->LineTraceSingleByChannel(
					HitResult,
					Owner->GetActorLocation() + FVector(0, 0, Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
					TargettedTarget->GetActorLocation(), ECC_WorldDynamic, Params))
				{
					if (HitResult.Actor == TargettedTarget)
					{
						FVector RightDirection = FVector::CrossProduct(Owner->GetController()->GetControlRotation().Vector(), FVector::UpVector);
						FVector RealOffset = RightDirection * -Offset.X * Sensitivity;
						RealOffset += Owner->GetActorUpVector() * Offset.Z * Sensitivity;

						FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargettedTarget->GetActorLocation() + RealOffset);

						float AngleBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Owner->GetController()->GetControlRotation().Vector(), Rot.Vector())));

						Owner->GetController()->SetControlRotation(
							UKismetMathLibrary::RInterpTo(
								Owner->GetController()->GetControlRotation(),
								Rot,
								GetWorld()->GetDeltaSeconds(),
								MaxCameraRotationPerSecond));
						return;
					}
				}
			}
		}
	}
	TargettedTarget = nullptr;
}

void ULockTargetComponent2::UnLockTarget()
{
	bToggle = false;
	TargetedCharacter = nullptr;
	if (GetWorld()->GetTimerManager().IsTimerActive(LockTargetHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(LockTargetHandle);
	}
	StoppedTargetingEnemy.Broadcast();
}

void ULockTargetComponent2::AddTargetableEnemy(AGP2_ArrowCharacter* AddTarget)
{
	if (!EnemyPlayers.Contains(AddTarget))
	{
		EnemyPlayers.Add(AddTarget);
	}
}

void ULockTargetComponent2::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!Owner)
	{
		Owner = Cast<AGP2_ArrowCharacter>(GetOwner());
	}
	if (bToggle)
	{
		LockTargetFunc();
	}
	if (bUseMarkers)
	{
		TestTargetting();
		for (int i = 0; i < HitMarkers.Num(); i++)
		{
			if (HitMarkers[i])
			{
				if (TargetableEnemies.Num() > i)
				{
					HitMarkerPlayerPtr[i] = TargetableEnemies[i];
					HitMarkers[i]->SetActorHiddenInGame(false);
					HitMarkers[i]->SetActorLocation(TargetableEnemies[i]->GetActorLocation() + FVector(0, 0, TargetableEnemies[i]->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f) + HitMarkerOffset);
				}
				else
				{
					HitMarkerPlayerPtr[i] = nullptr;
					HitMarkers[i]->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

void ULockTargetComponent2::TestTargetting()
{
	for (int i = 0; i < EnemyPlayers.Num(); i++)
	{
		if (EnemyPlayers[i]->CanBeTargeted())
		{
			float Distance = FVector::Distance(Owner->GetActorLocation(), EnemyPlayers[i]->GetActorLocation());
			if (Distance < MaxTargetingDistance && Distance > MinTargetingDistance)
			{
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), EnemyPlayers[i]->GetActorLocation());
				FVector Dir = LookAt.Vector();
				FVector Dir2 = Owner->GetController()->GetControlRotation().Vector();
				float Dot = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Dir, Dir2))));
				if (Dot < MaxDegrees)
				{
					FHitResult HitResult;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(Owner);
					//if (GetWorld()->LineTraceSingleByChannel(HitResult, Owner->GetActorLocation() + FVector(0, 0, Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), EnemyPlayers[i]->GetActorLocation(), ECC_WorldDynamic, Params))
					{
						if (!TargetableEnemies.Contains(EnemyPlayers[i])) {
							//if (HitResult.Actor == EnemyPlayers[i])
							CanTargetEnemy.Broadcast(EnemyPlayers[i]);
							TargetableEnemies.Add(EnemyPlayers[i]);
						}
					}
				}
				else
				{
					if (TargetableEnemies.Contains(EnemyPlayers[i]))
					{
						CantTargetEnemy.Broadcast(EnemyPlayers[i]);
						TargetableEnemies.Remove(EnemyPlayers[i]);
					}
				}
			}
			else
			{
				if (TargetableEnemies.Contains(EnemyPlayers[i]))
				{
					CantTargetEnemy.Broadcast(EnemyPlayers[i]);
					TargetableEnemies.Remove(EnemyPlayers[i]);
				}
			}
		}
		else
		{
			if (TargetableEnemies.Contains(EnemyPlayers[i]))
			{
				CantTargetEnemy.Broadcast(EnemyPlayers[i]);
				TargetableEnemies.Remove(EnemyPlayers[i]);
			}
		}
	}
}

void ULockTargetComponent2::FindTarget()
{
	float Best = 1000.0f;
	int BestIndex = -1;
	bool FoundPlayer = false;
	for (int i = 0; i < EnemyPlayers.Num(); i++)
	{
		if (EnemyPlayers[i] && EnemyPlayers[i]->CanBeTargeted())
		{
			float Distance = FVector::Distance(Owner->GetActorLocation(), EnemyPlayers[i]->GetActorLocation());
			if (Distance < MaxTargetingDistance && Distance > MinTargetingDistance)
			{
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), EnemyPlayers[i]->GetActorLocation());
				FVector Dir = LookAt.Vector();
				FVector Dir2 = Owner->GetController()->GetControlRotation().Vector();
				float Dot = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Dir, Dir2))));
				if (Dot < Best && Dot < MaxDegrees)
				{
					FHitResult HitResult;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(Owner);
					if (GetWorld()->LineTraceSingleByChannel(HitResult, Owner->GetActorLocation() + FVector(0, 0, Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), EnemyPlayers[i]->GetActorLocation(), ECC_WorldDynamic, Params))
					{
						if (HitResult.Actor == EnemyPlayers[i])
						{
							Best = Dot;
							BestIndex = i; 
							FoundPlayer = true;
						}
					}
				}
			}
		}
	}
	if (BestIndex == -1)
	{
		for (int i = 0; i < TargetableTargets.Num(); i++)
		{
			if (TargetableTargets[i])
			{
				float Distance = FVector::Distance(Owner->GetActorLocation(), TargetableTargets[i]->GetActorLocation());
				if (Distance < MaxTargetingDistance && Distance > MinTargetingDistance)
				{
					FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargetableTargets[i]->GetActorLocation());
					FVector Dir = LookAt.Vector();
					FVector Dir2 = Owner->GetController()->GetControlRotation().Vector();
					float Dot = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Dir, Dir2))));
					if (Dot < Best && Dot < MaxDegrees)
					{
						FHitResult HitResult;
						FCollisionQueryParams Params;
						Params.AddIgnoredActor(Owner);
						if (GetWorld()->LineTraceSingleByChannel(HitResult, Owner->GetActorLocation() + FVector(0, 0, Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), TargetableTargets[i]->GetActorLocation(), ECC_WorldDynamic, Params))
						{
							if (HitResult.Actor == TargetableTargets[i])
							{
								Best = Dot;
								BestIndex = i;
							}
						}
					}
				}
			}
		}
	}			

	if (BestIndex != -1)
	{
		if (FoundPlayer)
		{
			TargetedCharacter = EnemyPlayers[BestIndex];
			StartedTargetingEnemy.Broadcast(TargetedCharacter);
		}
		else
		{
			TargettedTarget = TargetableTargets[BestIndex];
		}

		if (GetWorld()->GetTimerManager().IsTimerActive(LockTargetHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(LockTargetHandle);
		}
	}
	
}

void ULockTargetComponent2::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<AGP2_ArrowCharacter>(GetOwner());
	for (int i = 0; i < NumberOfEnemyPlayers; i++)
	{
		FActorSpawnParameters Params;
		Params.Owner = Owner;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* Actor = GetWorld()->SpawnActor(TargetableType, &Owner->GetActorTransform(), Params);
		HitMarkers.Add(Actor);
		Actor->SetOwner(Owner);
		Actor->SetActorHiddenInGame(true);
		HitMarkerPlayerPtr.Add(nullptr);
	} 
}
