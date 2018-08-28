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

FObjectIdentifier ASelectableStaticMeshActor::GetObjectIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier;
}

FString ASelectableStaticMeshActor::GetJsonIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier.JsonIdentifier;
}

FString ASelectableStaticMeshActor::GetDiscipline_Implementation() const
{
	return Discipline;
}

FString ASelectableStaticMeshActor::GetUniqueIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier.GetUniqueIdentifier();
}

void ASelectableStaticMeshActor::SetDisciplineAndObjectIdentifier(const FString& NewDiscipline, const FObjectIdentifier& NewObjectIdentifier)
{
	Discipline = NewDiscipline;
	ObjectIdentifier = NewObjectIdentifier;
}
