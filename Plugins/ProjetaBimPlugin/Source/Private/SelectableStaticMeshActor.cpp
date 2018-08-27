// (C) 2018 - ProjetaBIM

#include "SelectableStaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

void ASelectableStaticMeshActor::Select_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(true);
}

void ASelectableStaticMeshActor::Deselect_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(false);
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
