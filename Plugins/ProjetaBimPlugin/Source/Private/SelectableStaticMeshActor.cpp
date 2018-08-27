// (C) 2018 - ProjetaBIM

#include "SelectableStaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void ASelectableStaticMeshActor::Select_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(true);

	for (int i = 0; i < GetStaticMeshComponent()->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = GetStaticMeshComponent()->CreateDynamicMaterialInstance(i);
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue(FName(TEXT("Highlight")), 1.0f);
		}
	}
}

void ASelectableStaticMeshActor::Deselect_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(false);

	for (int i = 0; i < GetStaticMeshComponent()->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = GetStaticMeshComponent()->CreateDynamicMaterialInstance(i);
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue(FName(TEXT("Highlight")), 0.0f);
		}
	}
}

FString ASelectableStaticMeshActor::GetJsonIdentifier_Implementation(int32 Index)
{
	return JsonID;
}

FString ASelectableStaticMeshActor::GetDiscipline_Implementation()
{
	return Discipline;
}

void ASelectableStaticMeshActor::SetDisciplineAndJsonID(const FString & NewDiscipline, const FString & NewJsonID)
{
	Discipline = NewDiscipline;
	JsonID = NewJsonID;
}
