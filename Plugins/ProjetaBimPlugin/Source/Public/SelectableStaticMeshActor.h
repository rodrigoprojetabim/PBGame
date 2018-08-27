// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SelectionInterface.h"
#include "SelectableStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJETABIMPLUGIN_API ASelectableStaticMeshActor : public AStaticMeshActor, public ISelectionInterface
{
	GENERATED_BODY()

public:
	/* ISelectionInterface */
	virtual void Select_Implementation(int32 Index) override;
	virtual void Deselect_Implementation(int32 Index) override;
	FString GetJsonIdentifier_Implementation(int32 Index) override;
	FString GetDiscipline_Implementation() override;
	/* end ISelectionInterface */
	
	UPROPERTY(EditAnywhere, Category="ProjetaBIM")
	FString Discipline;
	
	UPROPERTY(EditAnywhere, Category="ProjetaBIM")
	FString JsonID;

	UFUNCTION(BlueprintCallable, Category="ProjetaBIM")
	void SetDisciplineAndJsonID(const FString& NewDiscipline, const FString& NewJsonID);
};
