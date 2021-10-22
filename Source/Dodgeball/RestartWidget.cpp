// Fill out your copyright notice in the Description page of Project Settings.


#include "RestartWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void URestartWidget::OnRestartClicked()
{
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void URestartWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

void URestartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (RestartButton != nullptr)
	{
		RestartButton->OnClicked.AddDynamic(this, &URestartWidget::OnRestartClicked);
		ExitButton->OnClicked.AddDynamic(this, &URestartWidget::OnExitClicked);
	}
}