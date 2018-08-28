// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "ProjetaBimTypes.h"
#include "PBGameMode.generated.h"

UCLASS()
class APBGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

private:

	FName SetSelectionDefinedTag;
	FName AddedToSMMapTag;

	class ULevelStreaming* GetStreamingLevelFromName(const FString& LevelName) const;
	
protected:

	/** returns Mesh's discipline by looking at its streaming level name suffix. Returns MOB if level has no suffix. */
	FString GetMeshDiscipline(const class AStaticMeshActor* Mesh) const;

	/** returns the current level's name */
	FString GetPersistentLevelName() const;

	/** removes the discipline from LevelName, e.g. TESTMAP_ARQ becomes TESTMAP */
	FString GetLevelNameWithoutDisciplineSuffix(const FString& LevelName) const;

public:
	
	//APBGameMode();

	/** disciplines that need to be processed into the SetSelection map */
	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FString> DisciplinesToProcess;

	UFUNCTION(BlueprintCallable, Category="ProjetaBIM")
	void InitializeSetSelectionMap();

	/** note: returns a copy! */
	UFUNCTION(BlueprintPure, Category = "ProjetaBIM")
	bool GetSetSelection(const FString& SetIdentifier, FSetSelection& SetSelection) const;

	/** adds Mesh to SetIdentifier, returns true if success, false if SetIdentifier doesn't exists */
	bool AddStaticMeshToSetSelection(const FString& SetIdentifier, class AStaticMeshActor* Mesh);

	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FDiscipline> Disciplines;

	UPROPERTY(BlueprintReadOnly, Category = "ProjetaBIM")
	TMap<FString, AStaticMeshActor*> StaticMeshMap;
	
};
