// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "wowTerrainLevelStreamActor.generated.h"

class ACharacter;
UCLASS()
class WOWTERRAINAPPLIER_API AwowTerrainLevelStreamActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AwowTerrainLevelStreamActor();
	TArray<FString> GetLevelAndConnections(FString LevelName);
	FString GetLevelConnection(FString LevelName, bool secondNumber, bool isPositive);
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
		FName LevelToLoad;

private:
	// Overlap volume to trigger level streaming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent * OverlapVolume;

};