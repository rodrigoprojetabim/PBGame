// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionInterface.h"
#include "ProjetaBimTypes.h"
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
	
	/** maps ISM index to object (Revit) identifier */
	UPROPERTY(VisibleAnywhere, Category = "ProjetaBIM")
	TMap<int32, FObjectIdentifier> IDMap;

	/** index of currently selected instances */
	UPROPERTY(BlueprintReadOnly, Category = "Instanced Static Mesh Actor")
	TArray<int32> SelectedIndexes;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Instanced Static Mesh Actor")
	void SetMaterialAndMesh(class AStaticMeshActor* SourceSMA);
	void SetMaterialAndMesh_Implementation(class AStaticMeshActor* SourceSMA);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void AddInstancePB(const FTransform& WorldTransform, const FObjectIdentifier& ObjectIdentifier);
	void AddInstancePB_Implementation(const FTransform& WorldTransform, const FObjectIdentifier& ObjectIdentifier);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void UpdateAppearanceToSelected(int32 Index);
	void UpdateAppearanceToSelected_Implementation(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void UpdateAppearanceToDeselected(int32 Index);
	void UpdateAppearanceToDeselected_Implementation(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Instanced Static Mesh Actor", meta=(Keywords="json link"))
	int32 ObjectIdentifierToInstanceIndex(const FObjectIdentifier& ObjectID) const;
	
	/* ISelectionInterface */
	virtual void Select_Implementation(int32 Index) override;
	virtual void Deselect_Implementation(int32 Index) override;
	FString GetJsonIdentifier_Implementation(int32 Index) const override;
	FString GetDiscipline_Implementation() const override;
	FString GetUniqueIdentifier_Implementation(int32 Index) const override;
	FObjectIdentifier GetObjectIdentifier_Implementation(int32 Index) const override;
	/* end ISelectionInterface */
};
