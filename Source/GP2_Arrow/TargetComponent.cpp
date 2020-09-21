#include "TargetComponent.h"
#include <Engine/Engine.h>
#include "GP2_ArrowCharacter.h"
#include "GP2_ArrowProjectile.h"
#include <Components/StaticMeshComponent.h>
#include <Materials/Material.h>

UTargetComponent::UTargetComponent()
{
}

//void UTargetComponent::SetMaterial(UStaticMeshComponent* InMesh, UMaterial* InMaterial)
//{
//	UMaterial* Material = InMaterial;
//	UStaticMeshComponent* Mesh = InMesh;
//	if (Mesh != nullptr && Material != nullptr)
//	{
//		Mesh->SetMaterial(0, Material);
//	}
//}

void UTargetComponent::OnHit()
{
	if (bActivated == true)
	{
		bActivated = false;
		bShot = true;
		//if (ArrowRef != nullptr)
		//{
		//	if (ArrowRef->Player != nullptr)
		//	{
		//		ArrowRef->Player->SetTargetComponent(this);
				//OnArrowHit.Broadcast();
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, Player->GetFName().ToString());
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Hit by");
			//}
			//else
			//{
			//	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Player is nullptr");
			//}
		//}
		//else
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Arrow is nullptr");
		//}
	}
	/*else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Target is not activated");
	}*/
}

//void UTargetComponent::OnOverlap(AActor* OtherActor, UStaticMeshComponent* Pickup)
//{
//	AGP2_ArrowCharacter* Player = Cast<AGP2_ArrowCharacter>(OtherActor);
//	if (Player != nullptr)
//	{
//		Pickup->SetSimulatePhysics(false);
//		Pickup->SetEnableGravity(false);
//		Pickup->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		Pickup->SetHiddenInGame(true);
//		Player->SetTargetComponent(this);
//		OnAddScore.Broadcast(ScoreToAdd);
//		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "AddScore");
//	}
//}

void UTargetComponent::SetActivated(bool Activated)
{
	bActivated = Activated;
}

bool UTargetComponent::GetActivated()
{
	return bActivated;
}

bool UTargetComponent::GetShot()
{
	return bShot;
}
