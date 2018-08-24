// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstancedStaticMeshActor.generated.h"

UCLASS()
class AInstancedStaticMeshActor : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	AInstancedStaticMeshActor();

	class UInstancedStaticMeshComponent* ISMC;

	class UInstancedStaticMeshComponent* ISMC_Selected;

	class UInstancedStaticMeshComponent* ISMC_Transparent;
	
	/** maps ISMC index to object (Revit) identifier */
	UPROPERTY(BlueprintReadOnly, Category = "Instanced Static Mesh Actor")
	TMap<int32, int32> IDMap;

	/** index of currently selected instances */
	UPROPERTY(BlueprintReadOnly, Category = "Instanced Static Mesh Actor")
	TArray<int32> SelectedIndexes;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Instanced Static Mesh Actor")
	void SetMaterialAndMesh(class AStaticMeshActor* SourceSMA);
	void SetMaterialAndMesh_Implementation(class AStaticMeshActor* SourceSMA);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void AddInstancePB(const FTransform& WorldTransform, const FString& JsonID);
	void AddInstancePB_Implementation(const FTransform& WorldTransform, const FString& JsonID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void Select(int32 Index);
	void Select_Implementation(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void Deselect(int32 Index);
	void Deselect_Implementation(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Instanced Static Mesh Actor")
	int32 ObjectIDToInstanceIndex(int32 ObjectID) const;
};
