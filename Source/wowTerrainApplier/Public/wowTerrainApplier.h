// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SWidget;
class FReply;

class FwowTerrainApplierModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	void PluginDocumentationAction();
	TArray<FString> getCSVsFromFolder(FString Directory);
	TArray<FString> parseCSVtoStringArray(FString Directory);
	FString getTileName(FString CSVname);
	void createMaterial(TArray<FString> chunkProperties, FString tileName);
	void GenerateMaterialsAction();
	void GenerateLevelStreamingAction();
	void SetSpecularAction();
	void SetTwoSidedAction();
	void SetAlphaAction();
	void FillSubmenu(FMenuBuilder& MenuBuilder);
	void AddMenuEntry(FMenuBuilder& MenuBuilder);

private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<SWidget> FillComboButton(TSharedPtr<class FUICommandList> Commands);
	TSharedPtr<class FUICommandList> PluginCommands;
};
