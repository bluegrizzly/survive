#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

//-----------------------------------------------------------------------------
void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

//-----------------------------------------------------------------------------
void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

//-----------------------------------------------------------------------------
void USlashOverlay::SetGoldCount(int32 Count)
{
	if (GoldCountText)
	{
		const FString String = FString::Printf(TEXT("%d"), Count);
		const FText Text = (FText::FromString(String));
		GoldCountText->SetText(Text);
	}
}

//-----------------------------------------------------------------------------
void USlashOverlay::SetSoulsCount(int32 Count)
{
	if (SoulsCountText)
	{
		const FString String = FString::Printf(TEXT("%d"), Count);
		const FText Text = (FText::FromString(String));
		SoulsCountText->SetText(Text);
	}
}	
