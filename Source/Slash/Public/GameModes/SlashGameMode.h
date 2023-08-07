#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/CharacterTypes.h"
#include "SlashGameMode.generated.h"


UCLASS()
class SLASH_API ASlashGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	static ASlashGameMode* GetSlashGameMode(const UWorld* World);
	static bool GetDebugState(const UWorld* World, EDebugElement DebugElement);
	FORCEINLINE bool GetDebugCombat() const { return DebugCombat; }
	FORCEINLINE bool GetDebugWarping() const { return DebugWarping; }

private:
	UPROPERTY(EditAnywhere, Category = Debug)
	bool DebugCombat;
	UPROPERTY(EditAnywhere, Category = Debug)
	bool DebugWarping;
};
