// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "wowTerrainApplierCommands.h"

#define LOCTEXT_NAMESPACE "FwowTerrainApplierModule"

void FwowTerrainApplierCommands::RegisterCommands()
{
	UI_COMMAND(PluginDocumentation, "Documentation", "Open the documentation", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(GenerateMaterials, "Generate materials from .csv", "Generates materials for tiles from .csv textureLayers files", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(GenerateLevelStreaming, "Generate tile-based level streaming", "Generates basic level streaming based on tiles.\nWARNING: Experienced users only!", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SetSpecular, "Set 'Specular' value for chosen materials", "Sets a new 'Specular' value for materials chosen in content browser", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SetTwoSided, "Set 'Two Sided' value for chosen materials", "Sets a new 'Two Sided' value for materials chosen in content browser", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(SetAlpha, "Enable transparency for chosen materials", "Changes the blendmode and connects alpha for materials chosen in content browser", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
