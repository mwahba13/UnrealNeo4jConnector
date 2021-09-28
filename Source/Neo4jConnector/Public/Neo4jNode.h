// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Neo4jNode.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNeo4jElement
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly)
		int id;

	TMap<FString, TSharedPtr<FJsonValue>> properties;

};


struct FNeo4jRelationship;
//describes a neo4j node
USTRUCT(BlueprintType)
struct FNeo4jNode : public FNeo4jElement
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
		TArray<FString> labels;

};

//describes a neo4j relationship
USTRUCT(BlueprintType)
struct FNeo4jRelationship : public FNeo4jElement
{
	GENERATED_BODY()
public:

	FString type;

	int startNode;
	int endNode;


};
