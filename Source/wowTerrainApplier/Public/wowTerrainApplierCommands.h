// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "wowTerrainApplierStyle.h"

class FwowTerrainApplierCommands : public TCommands<FwowTerrainApplierCommands>
{
public:

	FwowTerrainApplierCommands()
		: TCommands<FwowTerrainApplierCommands>(TEXT("wowTerrainApplier"), NSLOCTEXT("Contexts", "wowTerrainApplier", "wowTerrainApplier Plugin"), NAME_None, FwowTerrainApplierStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > PluginDocumentation;
	TSharedPtr< FUICommandInfo > GenerateMaterials;
	TSharedPtr< FUICommandInfo > GenerateLevelStreaming;
	TSharedPtr< FUICommandInfo > SetSpecular;
	TSharedPtr< FUICommandInfo > SetTwoSided;
	TSharedPtr< FUICommandInfo > SetAlpha;
};
