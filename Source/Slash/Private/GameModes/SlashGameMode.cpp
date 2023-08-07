#include "GameModes/SlashGameMode.h"
#include "Kismet/GameplayStatics.h"

ASlashGameMode* ASlashGameMode::GetSlashGameMode(const UWorld* World)
{
	AGameModeBase* CaptureGameMode = UGameplayStatics::GetGameMode(World); 
	return Cast<ASlashGameMode>(CaptureGameMode);
}

bool ASlashGameMode::GetDebugState(const UWorld* World, EDebugElement DebugElement)
{
	ASlashGameMode* GameMode = GetSlashGameMode(World);
	if (GameMode)
	{
		switch (DebugElement)
		{
		case EDebugElement::EAS_Combat: return GameMode->GetDebugCombat();
		case EDebugElement::EAS_Warping: return GameMode->GetDebugWarping();
		default:
			return false;
		}
	}
	return false;
}
