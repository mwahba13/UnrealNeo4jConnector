// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Neo4jNode.h"
#include "UObject/NoExportTypes.h"
#include "Neo4jUtilities.generated.h"

/**
 * 
 */
UCLASS()
class NEO4JCONNECTOR_API UNeo4jUtilities : public UObject
{
	GENERATED_BODY()

public:

	static FString _ConstructJSONQueryString(FString stringToSerialize);

	static FString SerializeLabelsIntoQuery(TArray<FString> labels);

	static FString SerializePropertiesIntoQuery(TMap<FString, FString> stringProps, TMap<FString, int> intProps,
		TMap<FString, bool> boolProps);


	static TArray<FNeo4jNode> DeserializeNodeQueryResult(FString resultString);


	//returns FNeo4jNode Struct as a string inluding all labels and properties
	static FString SerializeNode(FNeo4jNode inNode);

	
};
