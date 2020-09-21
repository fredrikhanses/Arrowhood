#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddScore, int, Score);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FArrowHit);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2_ARROW_API UTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	//UPROPERTY(BlueprintAssignable, Category = Delegates)
	//FAddScore OnAddScore;

	//UPROPERTY(BlueprintAssignable, Category = Delegates)
	//FArrowHit OnArrowHit;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//int ScoreToAdd = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActivated = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShot = false;
	
public:	

	UTargetComponent();

	///**
	//* Sets the new material
	//*/
	//UFUNCTION(BlueprintCallable)
	//void SetMaterial(UStaticMeshComponent* InMesh, UMaterial* InMaterial);

	UFUNCTION(BlueprintCallable)
	void OnHit();

	//UFUNCTION(BlueprintCallable)
	//void OnOverlap(AActor* OtherActor, UStaticMeshComponent* Pickup);

	UFUNCTION(BlueprintCallable)
	void SetActivated(bool Activated);

	UFUNCTION(BlueprintCallable)
	bool GetActivated();

	UFUNCTION(BlueprintCallable)
	bool GetShot();
};
