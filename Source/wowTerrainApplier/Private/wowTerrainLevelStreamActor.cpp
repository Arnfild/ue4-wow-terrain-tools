#include "wowTerrainLevelStreamActor.h"
// Level.h?
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
AwowTerrainLevelStreamActor::AwowTerrainLevelStreamActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AwowTerrainLevelStreamActor::OverlapBegins);
}
// Called when the game starts or when spawned
void AwowTerrainLevelStreamActor::BeginPlay()
{
	Super::BeginPlay();

	/*
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	// if (OtherActor == MyCharacter && LevelToLoad != "")
	TArray<FString> levels = GetLevelAndConnections("azeroth_29_28");
	FLatentActionInfo LatentInfo;
	for (int i = 0; i < levels.Num(); i++)
	{
		UGameplayStatics::LoadStreamLevel(this, FName(*levels[i]), true, true, LatentInfo);
	}*/
}

// Called every frame
void AwowTerrainLevelStreamActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/* Example: azeroth_29_28
if secondNumber is true, then we edit 28
if isPositive is true, then we add, else we subtract. We use it with GetLevelAndConnections to build a level matrix.
*/
FString AwowTerrainLevelStreamActor::GetLevelConnection(FString LevelName, bool secondNumber, bool isPositive)
{
	FString temp = LevelName;
	//TArray<FChar> temp2;
	int lenShift = 3;

	if (secondNumber)
	{
		lenShift = 0;
	}

	if (isPositive)
	{
		if (temp[temp.Len() - lenShift - 1] == '9')
		{
			temp[temp.Len() - lenShift - 1] = '0';
			temp[temp.Len() - lenShift - 2]++;
		}
		else
		{
			temp[temp.Len() - lenShift - 1]++;
		}
	}
	else
	{
		if (temp[temp.Len() - lenShift - 1] == '0')
		{
			temp[temp.Len() - lenShift - 1] = '9';
			temp[temp.Len() - lenShift - 2]--;
		}
		else
		{
			temp[temp.Len() - lenShift - 1]--;
		}
	}
	return temp;
}

/*
X X X
X L X
X X X
We get an array that contains all these levels, where L is LevelName
*/
TArray<FString> AwowTerrainLevelStreamActor::GetLevelAndConnections(FString LevelName)
{
	TArray<FString> levelAndConnections;
	levelAndConnections.Init("", 9);
	levelAndConnections[0] = LevelName;
	levelAndConnections[1] = GetLevelConnection(LevelName, true, false); // Level below
	levelAndConnections[2] = GetLevelConnection(LevelName, true, true); // Level above
	levelAndConnections[3] = GetLevelConnection(LevelName, false, false); // Level to the left
	levelAndConnections[4] = GetLevelConnection(LevelName, false, true); // Level to the right
	levelAndConnections[5] = GetLevelConnection(levelAndConnections[3], true, false); // Upper left level
	levelAndConnections[6] = GetLevelConnection(levelAndConnections[3], true, true); // Left down level
	levelAndConnections[7] = GetLevelConnection(levelAndConnections[4], true, false); // Bottom right level
	levelAndConnections[8] = GetLevelConnection(levelAndConnections[4], true, true); // Right down level
	return levelAndConnections;
}

void AwowTerrainLevelStreamActor::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	// if (OtherActor == MyCharacter && LevelToLoad != "")
	if (LevelToLoad != "")
	{
		int LastLevelStreamingID = 0;
		TArray<FString> levels = GetLevelAndConnections(LevelToLoad.ToString());
		for (int i = 0; i < levels.Num(); i++)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.UUID = ++LastLevelStreamingID;
			UGameplayStatics::LoadStreamLevel(this, FName(*levels[i]), true, true, LatentInfo);
			UE_LOG(LogTemp, Warning, TEXT("Loading: %s"), *levels[i]);

			//ULevelStreaming* LevelStream = UGameplayStatics::GetStreamingLevel(this, FName(*levels[i])); // ? We need to wait for levels to load one after another. OnLevelLoaded???

			/*
			FLatentActionInfo NewActionInfo;
			NewActionInfo.UUID = ++LastLevelStreamingID;
			FStreamLevelAction* NewAction = new FStreamLevelAction(true, GetWorldAssetFName(WorldAsset), bShow, bBlock, NewActionInfo, GetWorld());
			*/

			// Добавить это в BeginPlay, включив bShouldBlockOnLoad. Таким образом игрок будет заморожен при первоначальной загрузке тайлов.
			// Затем отключить bShouldBlockOnLoad здесь, а также реализовать функцию перехода от одного тайла к другому. Нужно как-то запоминать уровень с которого приходим
		}
	}
}