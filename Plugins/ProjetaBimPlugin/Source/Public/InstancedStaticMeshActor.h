// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionInterface.h"
#include "InstancedStaticMeshActor.generated.h"

UCLASS(Blueprintable)
class PROJETABIMPLUGIN_API AInstancedStaticMeshActor : public AActor, public ISelectionInterface
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	class UInstancedStaticMeshComponent* ISMC;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	class UInstancedStaticMeshComponent* ISMC_Selected;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	class UInstancedStaticMeshComponent* ISMC_Transparent;

public:
	AInstancedStaticMeshActor();
	
	/** these objects' discipline */
	UPROPERTY(EditAnywhere, Category="Instanced Static Mesh Actor")
	FString Discipline;

	/** maps ISMC index to object (Revit) identifier */
	UPROPERTY(VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	TMap<int32, FString> IDMap;

	/** index of currently selected instances */
	UPROPERTY(BlueprintReadOnly, Category = "Instanced Static Mesh Actor")
	TArray<int32> SelectedIndexes;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Instanced Static Mesh Actor")
	void SetMaterialAndMesh(class AStaticMeshActor* SourceSMA);
	void SetMaterialAndMesh_Implementation(class AStaticMeshActor* SourceSMA);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void AddInstancePB(const FTransform& WorldTransform, const FString& JsonID, const FString& ObjectDiscipline);
	void AddInstancePB_Implementation(const FTransform& WorldTransform, const FString& JsonID, const FString& Discipline);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void UpdateAppearanceToSelected(int32 Index);
	void UpdateAppearanceToSelected_Implementation(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void UpdateAppearanceToDeselected(int32 Index);
	void UpdateAppearanceToDeselected_Implementation(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Instanced Static Mesh Actor")
	int32 ObjectIDToInstanceIndex(FString ObjectID) const;
	
	/* ISelectionInterface */
	virtual void Select_Implementation(int32 Index) override;
	virtual void Deselect_Implementation(int32 Index) override;
	FString GetJsonIdentifier_Implementation(int32 Index) override;
	FString GetDiscipline_Implementation() override;
	/* end ISelectionInterface */
};
