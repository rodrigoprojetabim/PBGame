// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PBGameMode.h"
#include "EngineUtils.h"
#include "Misc/FileHelper.h"
#include "Engine/LevelStreaming.h"
#include "ProjetaBimPluginBPLibrary.h"
#include "ProjetaBimPlugin.h"
#include "Json.h"

APBGameMode::APBGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetSelectionDefinedTag = FName(TEXT("SetSelectionDefined"));
	AddedToSMMapTag = FName(TEXT("AddedToSMMap"));
}

ULevelStreaming * APBGameMode::GetStreamingLevelFromName(const FString & LevelName) const
{
	for (auto Level : GetWorld()->GetStreamingLevels())
	{
		FString ThisLevelName = Level->GetWorldAssetPackageFName().ToString();		
		int32 SlashPos = ThisLevelName.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		ThisLevelName = ThisLevelName.RightChop(SlashPos + 1);
		ThisLevelName = ThisLevelName.Replace(TEXT("UEDPIE_0_"), TEXT(""));

		if (ThisLevelName == LevelName)
		{
			return Level;
		}
	}
	return nullptr;
}

FString APBGameMode::GetMeshDiscipline(const AStaticMeshActor * Mesh) const
{
	if (Mesh == nullptr)
	{
		return TEXT("");
	}
	ULevel* MeshLevel = Mesh->GetLevel();
	if (MeshLevel == nullptr || MeshLevel->bIsLightingScenario)
	{
		return TEXT("MOB");
	}
	const FString LevelName = MeshLevel->GetOuter()->GetName();
	const int32 UnderlineLoc = LevelName.Find(TEXT("_"));
	if (UnderlineLoc != -1)
	{
		return LevelName.RightChop(UnderlineLoc+1);
	}
	return TEXT("MOB");
}

FString APBGameMode::GetPersistentLevelName() const
{
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	return LevelName;
}

FString APBGameMode::GetLevelNameWithoutDisciplineSuffix(const FString& LevelName) const
{
	FString ThisLevelName = LevelName.Replace(TEXT("UEDPIE_0_"), TEXT(""));
	int32 SlashPos = ThisLevelName.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	return ThisLevelName.RightChop(SlashPos + 1);
}

/*
void APBGameMode::BeginPlay()
{
	for (auto Level : GetWorld()->StreamingLevels)
	{
		FString LevelName = Level->PackageNameToLoad.ToString();
		int32 SlashPos = LevelName.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		LevelName = LevelName.RightChop(SlashPos + 1);
		DisciplinesToProcess.Add(LevelName);
	}
	Super::BeginPlay();
}
*/
void APBGameMode::InitializeSetSelectionMap()
{
	TArray<FString> ProcessedDisciplines;

	for (auto LevelName : DisciplinesToProcess)
	{
		ULevelStreaming* Level = GetStreamingLevelFromName(LevelName);
		if (Level != nullptr && Level->IsLevelVisible())
		{
			UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("\n*** Lendo paramatros da disciplina ") + LevelName + TEXT(" ***"));

			TSharedPtr<FJsonObject> JsonObject;
			FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("json/") + GetPersistentLevelName() + TEXT("/") + LevelName + TEXT(".json");
			const FString DesiredFilePath = JsonFilePath;
			bool bFoundJsonFile = true;
			FString JsonString;
			if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
			{
				JsonFilePath = FPaths::ProjectContentDir() + TEXT("json/") + GetPersistentLevelName() + TEXT("/") + GetLevelNameWithoutDisciplineSuffix(LevelName) + TEXT(".json");
				if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
				{
					JsonFilePath = FPaths::ProjectContentDir() + TEXT("json/") + GetPersistentLevelName() + TEXT("/") + GetPersistentLevelName() + TEXT(".json");
					if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
					{
						UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Arquivo Json nao encontrado: ") + DesiredFilePath + TEXT("; objetos dessa disciplina irao para o set Outros"));
						bFoundJsonFile = false;
					}
				}
			}

			if (bFoundJsonFile)
			{
				TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);

				if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) ||
					!JsonObject.IsValid())
				{
					UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Erro de leitura, verifique a sintaxe no arquivo ") + JsonFilePath);
				}
			}

			for (TActorIterator<AStaticMeshActor> It(GetWorld(), AStaticMeshActor::StaticClass()); It; ++It)
			{
				AStaticMeshActor* Mesh = *It;

				if (!Mesh->ActorHasTag(SetSelectionDefinedTag) && UProjetaBimPluginBPLibrary::GetActorsStreamingLevelName(Mesh) == LevelName)
				{
					//mesh not yet processed, and belongs to the currently processed streaming level

					const FString MeshName = Mesh->GetName();

					if (!Mesh->ActorHasTag(AddedToSMMapTag))
					{
						StaticMeshMap.Add(MeshName, Mesh);
						Mesh->Tags.Add(AddedToSMMapTag);
					}

					const int32 OBJ_Position = MeshName.Find(TEXT("OBJ_"));

					if (!JsonObject.IsValid())
					{
						const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
						AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
					}
					else if (OBJ_Position != -1) //mesh came from Revit
					{
						const FString ObjectID = MeshName.RightChop(OBJ_Position + 4);

						if (JsonObject->HasField(ObjectID))
						{
							const TSharedPtr<FJsonObject> ThisObj = JsonObject->GetObjectField(ObjectID);
							if (ThisObj->HasField("Parametros"))
							{
								const TSharedPtr<FJsonObject> Params = ThisObj->GetObjectField("Parametros");
								if (Params->HasField("SetSelection"))
								{
									const FString& SetSelectionValue = Params->GetStringField("SetSelection");
									if (!AddStaticMeshToSetSelection(SetSelectionValue, Mesh))
									{
										//invalid/undefined SetSelection value
										const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
										//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + MeshName + TEXT(" tem valor de SetSelection invalido (") + SetSelectionValue + TEXT("), adicionando-o ao set ") + MeshDiscipline);
										AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
									}
								}
								else
								{
									//has no SetSelection field
									const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
									//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + MeshName + TEXT(" nao tem campo SetSelection, adicionando-o ao set ") + MeshDiscipline);
									AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
								}
							}
							else
							{
								//has no field Parametros
								const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
								//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + MeshName + TEXT(" nao tem campo Parametros, adicionando-o ao set ") + MeshDiscipline);
								AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
							}
						}
						else
						{
							const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
							//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + MeshName + TEXT(" nao encontrado no json, adicionando-o ao set ") + MeshDiscipline);
							AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
						}
					}
					else //mesh added manually in editor
					{
						//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + MeshName + TEXT(" nao tem um ID valido, adicionando-o na disciplina Mobiliaria (MOB_Outros)."));
						AddStaticMeshToSetSelection(TEXT("MOB_Outros"), Mesh);
					}
				}
			}
			ProcessedDisciplines.Add(LevelName);
			UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("*** Concluida a leitura da disciplina ") + LevelName + TEXT(" ***\n"));
		}
	}
	for (auto Processed : ProcessedDisciplines)
	{
		DisciplinesToProcess.Remove(Processed);
	}
}

bool APBGameMode::GetSetSelection(const FString & SetIdentifier, FSetSelection& SetSelection) const
{
	for (auto& Discipline : Disciplines)
	{
		for (auto& Set : Discipline.Sets)
		{
			if (Set.Identifier == SetIdentifier)
			{
				SetSelection = Set;
				return true;
			}
		}
	}
	return false;
}

bool APBGameMode::AddStaticMeshToSetSelection(const FString & SetIdentifier, AStaticMeshActor * Mesh)
{
	for (auto& Discipline : Disciplines)
	{
		for (auto& Set : Discipline.Sets)
		{
			if (Set.Identifier.Equals(SetIdentifier,ESearchCase::IgnoreCase))
			{
				Mesh->Tags.Add(SetSelectionDefinedTag);
				Set.Meshes.Add(Mesh);
				return true;
			}
		}
	}
	return false;
}
