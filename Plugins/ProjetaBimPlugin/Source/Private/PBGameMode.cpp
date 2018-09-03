// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PBGameMode.h"
#include "EngineUtils.h"
#include "Misc/FileHelper.h"
#include "Engine/LevelStreaming.h"
#include "ProjetaBimPluginBPLibrary.h"
#include "ProjetaBimPlugin.h"
#include "SelectionInterface.h"
#include "Kismet/GameplayStatics.h"
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
			TArray<AActor*> ActorList;
			UGameplayStatics::GetAllActorsWithInterface(this, USelectionInterface::StaticClass(), ActorList);
			for (const auto &Actor : ActorList) 
			{
				ISelectionInterface* Interface = Cast<ISelectionInterface>(Actor);
				if (Interface != nullptr && !Actor->ActorHasTag(SetSelectionDefinedTag) && UProjetaBimPluginBPLibrary::GetActorsStreamingLevelName(Actor) == LevelName)
				{
					//actor not yet processed, and belongs to the currently processed streaming level
					for (int32 i = 0; i < Interface->Execute_Length(Actor); i++)
					{
						const FString ObjectUniqueIdentifier = Interface->Execute_GetUniqueIdentifier(Actor,i);
						FObjectIdentifier Obj = Interface->Execute_GetObjectIdentifier(Actor,i);
						if (!Actor->ActorHasTag(AddedToSMMapTag))
						{
							ObjectsMap.Add(ObjectUniqueIdentifier, Obj);
							Actor->Tags.Add(AddedToSMMapTag);
						}

						const int32 OBJ_Position = ObjectUniqueIdentifier.Find(TEXT("OBJ_"));

						if (!JsonObject.IsValid())
						{
							const FString ActorDiscipline = Obj.DisciplineCode + TEXT("_Outros");
							AddObjectToSetSelection(ActorDiscipline, Obj);
						}
						else if (Obj.HasJsonIdentifier()) //mesh came from Revit
						{
							if (JsonObject->HasField(Obj.JsonIdentifier))
							{
								const TSharedPtr<FJsonObject> ThisObj = JsonObject->GetObjectField(Obj.JsonIdentifier);
								if (ThisObj->HasField("Parametros"))
								{
									const TSharedPtr<FJsonObject> Params = ThisObj->GetObjectField("Parametros");
									if (Params->HasField("SetSelection"))
									{
										const FString& SetSelectionValue = Params->GetStringField("SetSelection");
										if (!AddObjectToSetSelection(SetSelectionValue, Obj))
										{
											//invalid/undefined SetSelection value
											const FString ActorDiscipline = Obj.DisciplineCode + TEXT("_Outros");
											//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + ObjectUniqueIdentifier + TEXT(" tem valor de SetSelection invalido (") + SetSelectionValue + TEXT("), adicionando-o ao set ") + ActorDiscipline);
											AddObjectToSetSelection(ActorDiscipline, Obj);
										}
									}
									else
									{
										//has no SetSelection field
										const FString ActorDiscipline = Obj.DisciplineCode + TEXT("_Outros");
										//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + ObjectUniqueIdentifier + TEXT(" nao tem campo SetSelection, adicionando-o ao set ") + ActorDiscipline);
										AddObjectToSetSelection(ActorDiscipline, Obj);
									}
								}
								else
								{
									//has no field Parametros
									const FString ActorDiscipline = Obj.DisciplineCode + TEXT("_Outros");
									//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + ObjectUniqueIdentifier + TEXT(" nao tem campo Parametros, adicionando-o ao set ") + ActorDiscipline);
									AddObjectToSetSelection(ActorDiscipline, Obj);
								}
							}
							else
							{
								const FString ActorDiscipline = Obj.DisciplineCode + TEXT("_Outros");
								//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + ObjectUniqueIdentifier + TEXT(" nao encontrado no json, adicionando-o ao set ") + ActorDiscipline);
								AddObjectToSetSelection(ActorDiscipline, Obj);
							}
						}
						else //mesh added manually in editor
						{
							//UProjetaBimPluginBPLibrary::AddLogEntry(TEXT("Objeto ") + ObjectUniqueIdentifier + TEXT(" nao tem um ID valido, adicionando-o na disciplina Mobiliaria (MOB_Outros)."));
							AddObjectToSetSelection(TEXT("MOB_Outros"), Obj);
						}

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

bool APBGameMode::AddObjectToSetSelection(const FString& InSetIdentifier, FObjectIdentifier InObject)
{
	for (auto& Discipline : Disciplines)
	{
		for (auto& Set : Discipline.Sets)
		{
			if (Set.Identifier.Equals(InSetIdentifier,ESearchCase::IgnoreCase))
			{
				InObject.Actor->Tags.Add(SetSelectionDefinedTag);
				InObject.SetSelectionCode = InSetIdentifier;
				Set.Objects.Add(InObject);
				return true;
			}
		}
	}
	return false;
}
