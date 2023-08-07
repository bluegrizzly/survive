#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGoldCount(int32 Count);
	void SetSoulsCount(int32 Count);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> StaminaProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GoldCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SoulsCountText;

};
