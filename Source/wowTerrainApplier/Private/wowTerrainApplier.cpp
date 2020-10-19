// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "wowTerrainApplier.h"
#include "wowTerrainApplierStyle.h"
#include "wowTerrainApplierCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "LevelEditor.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Misc/FileHelper.h"
#include <experimental/filesystem>
#include "HAL/FileManagerGeneric.h"
#include "Interfaces/IPluginManager.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/ScopedSlowTask.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

static const FName wowTerrainApplierTabName("wowTerrainApplier");

#define LOCTEXT_NAMESPACE "FwowTerrainApplierModule"

void FwowTerrainApplierModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FwowTerrainApplierStyle::Initialize();
	FwowTerrainApplierStyle::ReloadTextures();

	FwowTerrainApplierCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FwowTerrainApplierCommands::Get().PluginDocumentation,
		FExecuteAction::CreateRaw(this, &FwowTerrainApplierModule::PluginDocumentationAction),
		FCanExecuteAction()
	);

	PluginCommands->MapAction(
		FwowTerrainApplierCommands::Get().GenerateMaterials,
		FExecuteAction::CreateRaw(this, &FwowTerrainApplierModule::GenerateMaterialsAction),
		FCanExecuteAction()
	);

	PluginCommands->MapAction(
		FwowTerrainApplierCommands::Get().GenerateLevelStreaming,
		FExecuteAction::CreateRaw(this, &FwowTerrainApplierModule::GenerateLevelStreamingAction),
		FCanExecuteAction()
	);

	PluginCommands->MapAction(
		FwowTerrainApplierCommands::Get().SetSpecular,
		FExecuteAction::CreateRaw(this, &FwowTerrainApplierModule::SetSpecularAction),
		FCanExecuteAction()
	);

	PluginCommands->MapAction(
		FwowTerrainApplierCommands::Get().SetTwoSided,
		FExecuteAction::CreateRaw(this, &FwowTerrainApplierModule::SetTwoSidedAction),
		FCanExecuteAction()
	);

	PluginCommands->MapAction(
		FwowTerrainApplierCommands::Get().SetAlpha,
		FExecuteAction::CreateRaw(this, &FwowTerrainApplierModule::SetAlphaAction),
		FCanExecuteAction()
	);
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FwowTerrainApplierModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FwowTerrainApplierModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FwowTerrainApplierModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FwowTerrainApplierStyle::Shutdown();

	FwowTerrainApplierCommands::Unregister();
}
/*
void FwowTerrainApplierModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FwowTerrainApplierModule::PluginButtonClicked()")),
							FText::FromString(TEXT("wowTerrainApplier.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}*/

void FwowTerrainApplierModule::AddMenuExtension(FMenuBuilder& Builder)
{
	//Builder.AddMenuEntry(FwowTerrainApplierCommands::Get().ApplyTextures);
}

void FwowTerrainApplierModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	//Builder.AddToolBarButton(FwowTerrainApplierCommands::Get().PluginAbout);

	FUIAction TempCompileOptionsCommand;

	Builder.AddComboButton(
		TempCompileOptionsCommand,
		FOnGetContent::CreateRaw(this, &FwowTerrainApplierModule::FillComboButton, PluginCommands),
		LOCTEXT("ExampleComboButton", "WoW Terrain Tools"),
		LOCTEXT("ExampleComboButtonTootlip", "WoW Terrain Tools"),
		FSlateIcon(FwowTerrainApplierStyle::GetStyleSetName(), "wowTerrainApplier"),
		false
	);
}

void FwowTerrainApplierModule::FillSubmenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().PluginDocumentation);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().GenerateMaterials);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().GenerateLevelStreaming);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().SetSpecular);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().SetTwoSided);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().SetAlpha);
}

void FwowTerrainApplierModule::AddMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("CustomMenu", TAttribute<FText>(FText::FromString("TestMenu")));

	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().GenerateMaterials);
	MenuBuilder.AddSubMenu(FText::FromString("My Submenu"),
		FText::FromString("My submenu toolkit"),
		FMenuExtensionDelegate::CreateRaw(this, &FwowTerrainApplierModule::FillSubmenu));

	MenuBuilder.EndSection();
}

TSharedRef<SWidget> FwowTerrainApplierModule::FillComboButton(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder(true, Commands);

	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().PluginDocumentation);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().GenerateMaterials);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().GenerateLevelStreaming);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().SetSpecular);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().SetTwoSided);
	MenuBuilder.AddMenuEntry(FwowTerrainApplierCommands::Get().SetAlpha);

	return MenuBuilder.MakeWidget();
}

void FwowTerrainApplierModule::PluginDocumentationAction()
{
	FString TheURL = "http://www.google.com/";
	FPlatformProcess::LaunchURL(*TheURL, nullptr, nullptr);
}

TArray<FString> FwowTerrainApplierModule::getCSVsFromFolder(FString Directory)
{
	TArray<FString> csvFilePaths;
	csvFilePaths.Empty();
	FString path = IPluginManager::Get().FindPlugin(TEXT("wowTerrainApplier"))->GetContentDir() + Directory;
	if (FPaths::DirectoryExists(path))
	{
		FString extension = "*csv";
		FFileManagerGeneric::Get().FindFiles(csvFilePaths, *path, *extension); // Here we find all the .csv files from directory and copy the paths
	}
	return csvFilePaths;
}

TArray<FString> FwowTerrainApplierModule::parseCSVtoStringArray(FString File)
{
	TArray<FString> csvArray;
	FString path = IPluginManager::Get().FindPlugin(TEXT("wowTerrainApplier"))->GetContentDir() + File;
	if (FFileHelper::LoadANSITextFileToStrings(*path, NULL, csvArray)) // Here we copy our .csv to csvArray string array
	{
		//UE_LOG(LogTemp, Warning, TEXT("Processing the file: %s"), *path);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't parse the file: %s"), *path);
	}
	return csvArray;
}

// Converts azeroth_29_28_layers to azeroth_29_28 etc.
FString FwowTerrainApplierModule::getTileName(FString CSVname)
{
	FString tempString = CSVname;
	int len;
	if (tempString.Contains("layers")){
		len = 11; // _layers = 7 symbols, but we also include .csv, overall 11 symbols
	}
	else{
		len = 10; // no info for modelplacement yet
	}
	tempString.RemoveAt(tempString.Len() - len, len);
	return tempString;
}

void FwowTerrainApplierModule::createMaterial(TArray<FString> chunkInfo, FString tileName)
{
	switch (chunkInfo.Num())
	{
	case 2: // One texture
		{
		FString MaterialBaseName = tileName + "_" + chunkInfo[0];
		FString PackageName = "/Game/Warcraft/";
		PackageName += MaterialBaseName;
		UPackage* Package = CreatePackage(NULL, *PackageName);

		UMaterial* parentPointer = LoadObject<UMaterial>(NULL, *FString("Material'/wowTerrainApplier/wowTerrainMasterMaterial.wowTerrainMasterMaterial'"));
		UTexture2D* tex0Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[1] + "." + chunkInfo[1] + "'"));

		auto MaterialFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
		MaterialFactory->InitialParent = parentPointer;
		UMaterialInstanceConstant* UnrealMaterial = (UMaterialInstanceConstant*)MaterialFactory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), Package, *MaterialBaseName, RF_Standalone | RF_Public, NULL, GWarn);
		FAssetRegistryModule::AssetCreated(UnrealMaterial);
		Package->FullyLoad();
		Package->SetDirtyFlag(true);

		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex0"), tex0Pointer);
		UnrealMaterial->PreEditChange(NULL);
		UnrealMaterial->PostEditChange();

		FGlobalComponentReregisterContext RecreateComponents;

		break;
		}
	case 3: // Two textures
		{
		FString MaterialBaseName = tileName + "_" + chunkInfo[0];
		FString PackageName = "/Game/Warcraft/";
		PackageName += MaterialBaseName;
		UPackage* Package = CreatePackage(NULL, *PackageName);

		UMaterial* parentPointer = LoadObject<UMaterial>(NULL, *FString("Material'/wowTerrainApplier/wowTerrainMasterMaterial.wowTerrainMasterMaterial'"));
		UTexture2D* alphamapPointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + tileName + "_" + chunkInfo[0] + "." + tileName + "_" + chunkInfo[0] + "'"));
		UTexture2D* tex0Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[1] + "." + chunkInfo[1] + "'"));
		UTexture2D* tex1Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[2] + "." + chunkInfo[2] + "'"));

		auto MaterialFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
		MaterialFactory->InitialParent = parentPointer;
		UMaterialInstanceConstant * UnrealMaterial = (UMaterialInstanceConstant*)MaterialFactory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), Package, *MaterialBaseName, RF_Standalone | RF_Public, NULL, GWarn);
		FAssetRegistryModule::AssetCreated(UnrealMaterial);
		Package->FullyLoad();
		Package->SetDirtyFlag(true);

		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("alphamap"), alphamapPointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex0"), tex0Pointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex1"), tex1Pointer);
		UnrealMaterial->PreEditChange(NULL);
		UnrealMaterial->PostEditChange();

		FGlobalComponentReregisterContext RecreateComponents;

		break;
		}
	case 4: // Three textures
		{
		FString MaterialBaseName = tileName + "_" + chunkInfo[0];
		FString PackageName = "/Game/Warcraft/";
		PackageName += MaterialBaseName;
		UPackage* Package = CreatePackage(NULL, *PackageName);

		UMaterial* parentPointer = LoadObject<UMaterial>(NULL, *FString("Material'/wowTerrainApplier/wowTerrainMasterMaterial.wowTerrainMasterMaterial'"));
		UTexture2D* alphamapPointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + tileName + "_" + chunkInfo[0] + "." + tileName + "_" + chunkInfo[0] + "'"));
		UTexture2D* tex0Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[1] + "." + chunkInfo[1] + "'"));
		UTexture2D* tex1Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[2] + "." + chunkInfo[2] + "'"));
		UTexture2D* tex2Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[3] + "." + chunkInfo[3] + "'"));

		auto MaterialFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
		MaterialFactory->InitialParent = parentPointer;
		UMaterialInstanceConstant * UnrealMaterial = (UMaterialInstanceConstant*)MaterialFactory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), Package, *MaterialBaseName, RF_Standalone | RF_Public, NULL, GWarn);
		FAssetRegistryModule::AssetCreated(UnrealMaterial);
		Package->FullyLoad();
		Package->SetDirtyFlag(true);

		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("alphamap"), alphamapPointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex0"), tex0Pointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex1"), tex1Pointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex2"), tex2Pointer);
		UnrealMaterial->PreEditChange(NULL);
		UnrealMaterial->PostEditChange();

		FGlobalComponentReregisterContext RecreateComponents;

		break;
		}
	case 5: // Four textures
		{
		FString MaterialBaseName = tileName + "_" + chunkInfo[0];
		FString PackageName = "/Game/Warcraft/";
		PackageName += MaterialBaseName;
		UPackage* Package = CreatePackage(NULL, *PackageName);

		UMaterial* parentPointer = LoadObject<UMaterial>(NULL, *FString("Material'/wowTerrainApplier/wowTerrainMasterMaterial.wowTerrainMasterMaterial'"));
		UTexture2D* alphamapPointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + tileName + "_" + chunkInfo[0] + "." + tileName + "_" + chunkInfo[0] + "'"));
		UTexture2D* tex0Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[1] + "." + chunkInfo[1] + "'"));
		UTexture2D* tex1Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[2] + "." + chunkInfo[2] + "'"));
		UTexture2D* tex2Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[3] + "." + chunkInfo[3] + "'"));
		UTexture2D* tex3Pointer = LoadObject<UTexture2D>(NULL, *FString("Texture2D'/Game/Warcraft/TerrainTextures/" + chunkInfo[4] + "." + chunkInfo[4] + "'"));

		auto MaterialFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
		MaterialFactory->InitialParent = parentPointer;
		UMaterialInstanceConstant * UnrealMaterial = (UMaterialInstanceConstant*)MaterialFactory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), Package, *MaterialBaseName, RF_Standalone | RF_Public, NULL, GWarn);
		FAssetRegistryModule::AssetCreated(UnrealMaterial);
		Package->FullyLoad();
		Package->SetDirtyFlag(true);

		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("alphamap"), alphamapPointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex0"), tex0Pointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex1"), tex1Pointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex2"), tex2Pointer);
		UnrealMaterial->SetTextureParameterValueEditorOnly(FName("tex3"), tex3Pointer);
		UnrealMaterial->PreEditChange(NULL);
		UnrealMaterial->PostEditChange();

		FGlobalComponentReregisterContext RecreateComponents;

		break;
		}
	default:
		{
		break;
		}
	}
}

// Main CSV processing method
void FwowTerrainApplierModule::GenerateMaterialsAction()
{
	TArray<FString> textureLayers = FwowTerrainApplierModule::getCSVsFromFolder("/csvfiles/textureLayers");

	FString parsedCSVname; // temp for storing the name of csv that is being processed
	TArray<FString> parsedCSV; // temp for storing the data of csv that is being processed

	TArray<FString> splitCSVline;

	// Background worker for CSV processing
	FScopedSlowTask CsvProcessTask(textureLayers.Num(), FText::Format(LOCTEXT("CsvProcessTaskText", "Processing {0} textureLayers file(s)"), textureLayers.Num()));
	CsvProcessTask.MakeDialog(false); // We're working at full power, meaning that Cancel button simply won't work. bruh

	// We iterate through csv files in textureLayers folder and make sure they are all processed, the textures are created and applied
	for (int i = 0; i < textureLayers.Num(); i++)
	{
		CsvProcessTask.EnterProgressFrame();

		parsedCSVname = getTileName(textureLayers[i]);
		parsedCSV = FwowTerrainApplierModule::parseCSVtoStringArray("/csvfiles/textureLayers/" + textureLayers[i]);

		// Background worker for material generation
		FScopedSlowTask MaterialGenerationTask(parsedCSV.Num() - 2, FText::Format(LOCTEXT("MaterialGenerationTaskText", "Generating {0} chunk materials for {1}"), parsedCSV.Num() - 2, FText::FromString(parsedCSVname)));
		MaterialGenerationTask.MakeDialog(false);

		// We iterate through the csv file and create materials for chunks
		for (int j = 1; j < parsedCSV.Num() - 1; j++) // 0th row is the header!
		{
			MaterialGenerationTask.EnterProgressFrame();
			splitCSVline.Empty();
			parsedCSV[j].ParseIntoArrayWS(splitCSVline, TEXT(";"), true);
			FwowTerrainApplierModule::createMaterial(splitCSVline, parsedCSVname);
		}
	}
}

void FwowTerrainApplierModule::GenerateLevelStreamingAction()
{
	UE_LOG(LogTemp, Warning, TEXT("GenerateLevelStreamingAction works"));
}

// To-do: Edit material properties action. A dialogue window where the user could choose: edit specular, edit two-sided. That way you won't need to load the assets two times, only one time
void FwowTerrainApplierModule::SetSpecularAction()
{
	UE_LOG(LogTemp, Warning, TEXT("SetSpecularAction works"));

	TArray<FAssetData> AssetDatas;
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	ContentBrowserSingleton.GetSelectedAssets(AssetDatas);

	// Background worker for SetSpecularTask
	FScopedSlowTask SetSpecularTask(AssetDatas.Num(), FText::Format(LOCTEXT("SetSpecularTask", "Setting 'Specular' value for {0} material(s)"), AssetDatas.Num()));
	SetSpecularTask.MakeDialog(false);
	for (int i = 0; i < AssetDatas.Num(); i++) {
		SetSpecularTask.EnterProgressFrame();
		UMaterial* MaterialToEdit = Cast<UMaterial>(AssetDatas[i].GetAsset());

		if (MaterialToEdit){
			UMaterialExpressionScalarParameter* Specular = NewObject<UMaterialExpressionScalarParameter>(MaterialToEdit);
			Specular->ParameterName = "Specular";
			Specular->DefaultValue = 0.0f;
			Specular->Desc = "This node was generated with WoW Terrain Tools";
			Specular->MaterialExpressionEditorX = -130;
			Specular->MaterialExpressionEditorY = 100;

			MaterialToEdit->Specular.Expression = Specular; // If there previously was a node, it's simply disconnected
			
			MaterialToEdit->PreEditChange(NULL);
			MaterialToEdit->PostEditChange();

			FGlobalComponentReregisterContext RecreateComponents;
		}
	}
}

void FwowTerrainApplierModule::SetTwoSidedAction()
{
	TArray<FAssetData> AssetDatas;
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	ContentBrowserSingleton.GetSelectedAssets(AssetDatas);

	// Background worker for SetTwoSidedTask
	FScopedSlowTask SetTwoSidedTask(AssetDatas.Num(), FText::Format(LOCTEXT("SetTwoSidedTask", "Setting 'Two Sided' value for {0} material(s)"), AssetDatas.Num()));
	SetTwoSidedTask.MakeDialog(false);
	for (int i = 0; i < AssetDatas.Num(); i++) {
		SetTwoSidedTask.EnterProgressFrame();
		UMaterial* MaterialToEdit = Cast<UMaterial>(AssetDatas[i].GetAsset());
		if (MaterialToEdit)
		{
			MaterialToEdit->TwoSided = true;
			MaterialToEdit->PreEditChange(NULL);
			MaterialToEdit->PostEditChange();

			FGlobalComponentReregisterContext RecreateComponents;
		}
	}
}

void FwowTerrainApplierModule::SetAlphaAction()
{
	TArray<FAssetData> AssetDatas;
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	ContentBrowserSingleton.GetSelectedAssets(AssetDatas);

	// Background worker for SetTwoSidedTask
	FScopedSlowTask SetAlphaTask(AssetDatas.Num(), FText::Format(LOCTEXT("SetAlphaTask", "Enabling transparency for {0} material(s)"), AssetDatas.Num()));
	SetAlphaTask.MakeDialog(false);
	for (int i = 0; i < AssetDatas.Num(); i++) {
		SetAlphaTask.EnterProgressFrame();
		UMaterial* MaterialToEdit = Cast<UMaterial>(AssetDatas[i].GetAsset());
		if (MaterialToEdit)
		{
			MaterialToEdit->BlendMode = BLEND_Masked;
			MaterialToEdit->OpacityMask.Expression = MaterialToEdit->BaseColor.Expression;
			MaterialToEdit->OpacityMask.Mask = 1;
			MaterialToEdit->OpacityMask.MaskR = 0;
			MaterialToEdit->OpacityMask.MaskG = 0;
			MaterialToEdit->OpacityMask.MaskB = 0;
			MaterialToEdit->OpacityMask.MaskA = 1;

			MaterialToEdit->BaseColor.Expression->MaterialExpressionEditorX = -500;
			MaterialToEdit->BaseColor.Expression->MaterialExpressionEditorY = 0;
			MaterialToEdit->BaseColor.Expression->Desc = "This node was edited with WoW Terrain Tools";

			MaterialToEdit->PreEditChange(NULL);
			MaterialToEdit->PostEditChange();

			FGlobalComponentReregisterContext RecreateComponents;
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FwowTerrainApplierModule, wowTerrainApplier)