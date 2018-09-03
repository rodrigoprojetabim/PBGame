// (C) 2018 - ProjetaBIM

#include "InstancedStaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "ProjetaBimPluginBPLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

#define LIMBO FTransform(FVector(WORLD_MAX-10.f, WORLD_MAX-10.f, WORLD_MAX-10.f))

bool AInstancedStaticMeshActor::IsAnyInstanceTransparent() const
{
	for (int32 i=0; i < NumberOfInstances; i++)
	{
		if (ObjectsOpacity[i] == EOpacityLevel::Transparent || SetSelectionsOpacity[i] == EOpacityLevel::Transparent)
		{
			return true;
		}
	}
	return false;
}


AInstancedStaticMeshActor::AInstancedStaticMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMC"));
	ISMC_Selected = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMC_Selected"));
	ISMC_Transparent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMC_Transparent"));
	
	RootComponent = ISMC;
	ISMC_Selected->SetupAttachment(ISMC);
	ISMC_Transparent->SetupAttachment(ISMC);

	ISMC_Selected->SetVisibility(false);
	ISMC_Transparent->SetVisibility(false);

	ISMC_Selected->SetRenderCustomDepth(true);

	ISMC->SetMobility(EComponentMobility::Static);
	ISMC_Selected->SetMobility(EComponentMobility::Static);
	ISMC_Transparent->SetMobility(EComponentMobility::Static);
}

void AInstancedStaticMeshActor::SetMaterialAndMesh_Implementation(AStaticMeshActor * SourceSMA)
{
	if (SourceSMA == nullptr)
	{
		return;
	}
	const UStaticMeshComponent* SMC = SourceSMA->GetStaticMeshComponent();
	
	ISMC->SetStaticMesh(SMC->GetStaticMesh());
	ISMC_Transparent->SetStaticMesh(SMC->GetStaticMesh());
	ISMC_Selected->SetStaticMesh(SMC->GetStaticMesh());

	const int32 NumMaterials = SMC->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		ISMC->SetMaterial(i, SMC->GetMaterial(i));
		ISMC_Transparent->SetMaterial(i, SMC->GetMaterial(i));
		ISMC_Selected->SetMaterial(i, SMC->GetMaterial(i));
	}
}

void AInstancedStaticMeshActor::AddInstancePB_Implementation(const FTransform& WorldTransform, FObjectIdentifier ObjectIdentifier)
{
	const int32 Index = ISMC->AddInstanceWorldSpace(WorldTransform);
	
	ObjectIdentifier.Actor = this;
	ObjectIdentifier.Index = Index;
	ObjectIdentifier.LevelName = FName(*UProjetaBimPluginBPLibrary::GetActorsStreamingLevelName(this));

	IDMap.Add(Index, ObjectIdentifier);
	ISMC_Selected->AddInstanceWorldSpace(LIMBO);
	ISMC_Transparent->AddInstanceWorldSpace(LIMBO);

	ObjectsOpacity.Add(EOpacityLevel::Opaque);
	SetSelectionsOpacity.Add(EOpacityLevel::Opaque);
	OriginalWorldTransforms.Add(WorldTransform);
	NumberOfInstances = NumberOfInstances + 1;
	
}

int32 AInstancedStaticMeshActor::ObjectIdentifierToInstanceIndex(const FObjectIdentifier& ObjectID) const
{
	TArray<int32> Keys;
	IDMap.GetKeys(Keys);
	for (int32 i : Keys)
	{
		if (*IDMap.Find(i) == ObjectID)
		{
			return i;
		}
	}
	return -1;
}

void AInstancedStaticMeshActor::Highlight_Implementation(int32 Index)
{
	if (!Execute_IsFullyOpaque(this,Index))
	{
		return;
	}
	if (MatInstancesSelected.Num() == 0)
	{
		for (int32 i = 0; i < ISMC_Selected->GetNumMaterials(); i++)
		{
			MatInstancesSelected.Add(ISMC_Selected->CreateAndSetMaterialInstanceDynamic(i));
		}
	}
	for (int32 i=0; i < MatInstancesSelected.Num(); i++)
	{
		MatInstancesSelected[i]->SetScalarParameterValue(FName("Highlight"), 1.0f);
	}
	HighlightedIndexes.AddUnique(Index);

	ISMC->UpdateInstanceTransform(Index, LIMBO, true, true, true);
	ISMC_Selected->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
	ISMC_Selected->SetVisibility(true);
}

void AInstancedStaticMeshActor::RemoveHighlight_Implementation(int32 Index)
{
	if (!HighlightedIndexes.Contains(Index) )
	{
		return;
	}
	HighlightedIndexes.Remove(Index);

	if (Execute_IsFullyOpaque(this,Index))
	{
		ISMC->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
		ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);

		if (HighlightedIndexes.Num() == 0)
		{
			ISMC_Selected->SetVisibility(false);
		}
	}
}

FObjectIdentifier AInstancedStaticMeshActor::GetObjectIdentifier_Implementation(int32 Index) const
{
	return *IDMap.Find(Index);
}

EOpacityLevel AInstancedStaticMeshActor::GetObjectOpacity_Implementation(int32 Index) const
{
	return ObjectsOpacity[Index];
}

int32 AInstancedStaticMeshActor::Length_Implementation() const
{
	return NumberOfInstances;
}

void AInstancedStaticMeshActor::SetCollisionProfileName_Implementation(FName NewCollisionProfile)
{
	ISMC->SetCollisionProfileName(NewCollisionProfile);
	ISMC_Selected->SetCollisionProfileName(NewCollisionProfile);
}

void AInstancedStaticMeshActor::GetObjectBounds_Implementation(int32 Index, bool bOnlyCollidingComponents, FVector & OutOrigin, FVector & OutExtent) const
{
	if (Index < NumberOfInstances)
	{
		FBox Box;
		if (HighlightedIndexes.Find(Index) != -1)
		{
			Box = ISMC_Selected->InstanceBodies[Index]->GetBodyBounds();		
		}
		else
		{
			Box = ISMC->InstanceBodies[Index]->GetBodyBounds();
		}
		OutOrigin = Box.GetCenter();
		OutExtent = Box.GetExtent();
	}
}

EOpacityLevel AInstancedStaticMeshActor::GetSetSelectionOpacity_Implementation(int32 Index) const
{
	return SetSelectionsOpacity[Index];
}


void AInstancedStaticMeshActor::SetObjectOpacity_Internal(int32 Index)
{
	EOpacityLevel NewOpacityLevel = FMath::Max(ObjectsOpacity[Index], SetSelectionsOpacity[Index]);

	switch (NewOpacityLevel)
	{
		case EOpacityLevel::Opaque:
		{
			ISMC->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
			ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);
			ISMC_Transparent->UpdateInstanceTransform(Index, LIMBO, true, true, true);
			if (HighlightedIndexes.Find(Index) != INDEX_NONE)
			{
				Execute_Highlight(this,Index);
			}
		} break;
		case EOpacityLevel::Transparent:
		{
				ISMC->UpdateInstanceTransform(Index, LIMBO, true, true, true);
				ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);
				ISMC_Transparent->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
				ISMC_Transparent->SetVisibility(true);

				if (MatInstancesTransparent.Num() == 0)
				{
					for (int32 i = 0; i < ISMC_Transparent->GetNumMaterials(); i++)
					{
						MatInstancesTransparent.Add(ISMC_Transparent->CreateAndSetMaterialInstanceDynamic(i));
						MatInstancesTransparent[i]->SetScalarParameterValue(FName("UserSetOpacity"), 0.5f);
					}
				}
		} break;
		case EOpacityLevel::Invisible:
		{
				ISMC->UpdateInstanceTransform(Index, LIMBO, true, true, true);
				ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);
				ISMC_Transparent->UpdateInstanceTransform(Index, LIMBO, true, true, true);
		} break;
	}

	if (!IsAnyInstanceTransparent())
	{
		ISMC_Transparent->SetVisibility(false);
	}
}


void AInstancedStaticMeshActor::SetSetSelectionOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel)
{
	if (Index != -1)
	{
		SetSelectionsOpacity[Index] = NewOpacityLevel;
		SetObjectOpacity_Internal(Index);
	}
	else
	{
		for (int i = 0; i < NumberOfInstances; i++)
		{
			SetSelectionsOpacity[i] = NewOpacityLevel;
			SetObjectOpacity_Internal(i);
		}
	}
}

bool AInstancedStaticMeshActor::IsFullyOpaque_Implementation(int32 Index) const
{
	return (ObjectsOpacity[Index] == EOpacityLevel::Opaque && SetSelectionsOpacity[Index] == EOpacityLevel::Opaque);
}

void AInstancedStaticMeshActor::SetObjectOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel)
{
	if (Index != -1)
	{
		ObjectsOpacity[Index] = NewOpacityLevel;
		SetObjectOpacity_Internal(Index);
	}
	else
	{
		for (int i = 0; i < NumberOfInstances; i++)
		{
			ObjectsOpacity[i] = NewOpacityLevel;
			SetObjectOpacity_Internal(i);
		}
	}
}

FString AInstancedStaticMeshActor::GetJsonIdentifier_Implementation(int32 Index) const
{
	const FObjectIdentifier Obj = *IDMap.Find(Index);
	return Obj.JsonIdentifier;
}

FString AInstancedStaticMeshActor::GetDiscipline_Implementation() const
{
	TArray<int32> Keys;
	IDMap.GetKeys(Keys);
	if (Keys.Num() > 0)
	{
		return (*IDMap.Find(Keys[0])).DisciplineCode;
	}
	return TEXT("");
}

FString AInstancedStaticMeshActor::GetUniqueIdentifier_Implementation(int32 Index) const
{
	const FObjectIdentifier Obj = *IDMap.Find(Index);
	return Obj.GetUniqueIdentifier();
}

void AInstancedStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();

}

