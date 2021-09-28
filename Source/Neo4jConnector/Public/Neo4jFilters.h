// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Neo4jNode.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Neo4jFilters.generated.h"

/**
 * 
 */
UCLASS()
class NEO4JCONNECTOR_API UNeo4jFilters : public UObject
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Neo4jFilter")
		static FString FilterStringProperty(FNeo4jNode inNode, FString property);

	UFUNCTION(BlueprintCallable, Category = "Neo4jFilter")
		static int FilterIntProperty(FNeo4jNode inNode, FString property);

	UFUNCTION(BlueprintCallable, Category = "Neo4jFilter")
		static bool FilterBoolProperty(FNeo4jNode inNode, FString property);
	
};
