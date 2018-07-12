// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PBGameMode.h"
#include "EngineUtils.h"
#include "FileManager.h"
#include "Misc/FileHelper.h"
#include "ProjetaBimPlugin.h"

DEFINE_LOG_CATEGORY(LogPB);

APBGameMode::APBGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetSelectionDefinedTag = FName(TEXT("SetSelectionDefined"));
	AddedToSMMapTag = FName(TEXT("AddedToSMMap"));
}

FString APBGameMode::GetMeshDiscipline(const AStaticMeshActor * Mesh)
{
	if (Mesh == nullptr)
	{
		return TEXT("");
	}
	const FString LevelName = Mesh->GetLevel()->GetOuter()->GetName();
	const int32 UnderlineLoc = LevelName.Find(TEXT("_"));
	if (UnderlineLoc != -1)
	{
		return LevelName.RightChop(UnderlineLoc+1);
	}
	return TEXT("MOB");
}

void APBGameMode::InitializeSetSelectionMap()
{
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	TSharedPtr<FJsonObject> JsonObject;
	const FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("json/") + LevelName + TEXT(".json");
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		UE_LOG(LogPB, Fatal, TEXT("Arquivo Json nao encontrado: %s"), *JsonFilePath);
		return;
	}
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (! FJsonSerializer::Deserialize(JsonReader, JsonObject) ||
		! JsonObject.IsValid())
	{
		UE_LOG(LogPB, Fatal, TEXT("Erro de leitura no arquivo %s, verifique a sintaxe."), *JsonFilePath);
	}

	for (TActorIterator<AStaticMeshActor> It(GetWorld(), AStaticMeshActor::StaticClass()); It; ++It)
	{
		AStaticMeshActor* Mesh = *It;

		if (Mesh->ActorHasTag(SetSelectionDefinedTag))
		{
			//already processed
			continue;
		}

		const FString MeshName = Mesh->GetName();

		if (!Mesh->ActorHasTag(AddedToSMMapTag))
		{
			StaticMeshMap.Add(MeshName, Mesh);
			Mesh->Tags.Add(AddedToSMMapTag);
		}

		const int32 OBJ_Position = MeshName.Find(TEXT("OBJ_"));

		if (OBJ_Position != -1) //mesh came from Revit
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
							UE_LOG(LogPB, Warning, TEXT("Objeto %s tem valor de SetSelection invalido (%s), adicionando-o ao set %s."), *ObjectID, *SetSelectionValue, *MeshDiscipline);
							AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
						}
					}
					else
					{
						//has no SetSelection field
						const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
						UE_LOG(LogPB, Warning, TEXT("Objeto %s nao tem campo SetSelection, adicionando-o ao set %s."), *ObjectID, *MeshDiscipline);
						AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
					}
				}
				else
				{
					//has no field Parametros
					const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
					UE_LOG(LogPB, Warning, TEXT("Objeto %s nao tem campo Parametros, adicionando-o ao set %s."), *ObjectID, *MeshDiscipline);
					AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
				}
			}
			else
			{
				const FString MeshDiscipline = GetMeshDiscipline(Mesh) + TEXT("_Outros");
				UE_LOG(LogPB, Warning, TEXT("Objeto %s nao encontrado no json, adicionando-o ao set %s."), *ObjectID, *MeshDiscipline);
				AddStaticMeshToSetSelection(MeshDiscipline, Mesh);
			}
		}
		else //mesh added manually in editor
		{ 
			UE_LOG(LogPB, Warning, TEXT("Objeto %s nao tem um ID valido, adicionando-o na disciplina Mobiliaria (MOB_Outros)."), *MeshName);
			AddStaticMeshToSetSelection(TEXT("MOB_Outros"), Mesh);
		}
	}
}

bool APBGameMode::GetSetSelection(const FString & SetIdentifier, FSetSelection& SetSelection)
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
