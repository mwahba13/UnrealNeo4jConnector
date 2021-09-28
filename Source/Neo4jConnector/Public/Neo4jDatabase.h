// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "UObject/NoExportTypes.h"
#include "Neo4jNode.h"
#include "Neo4jDatabase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestCompletedDelegate);

/**
* An abstraction of a neo4j database. This is the main class through which queries can be processed.
* Database must first be initialized.
*/
UCLASS(Blueprintable, BlueprintType)
class NEO4JCONNECTOR_API UNeo4jDatabase : public UObject
{
	GENERATED_BODY()

public:

#pragma region DELEGATES
	//DELEGATES
	UPROPERTY(BlueprintAssignable, meta = (Tooltip = "Delegate fires when String Query completes"))
		FOnRequestCompletedDelegate OnStringQueryCompleteDelegate;

	//NODE DELEGATES
	UPROPERTY(BlueprintAssignable, meta = (Tooltip = "Delegate fires when Get Node query completes"))
		FOnRequestCompletedDelegate OnGetNodeCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnRequestCompletedDelegate OnGetNeighbourCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnRequestCompletedDelegate OnMergeNodeCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnRequestCompletedDelegate OnUpdateNodeCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnRequestCompletedDelegate OnCreateNodeCompleteDelegate;


	//RELATION DELEGATES


#pragma endregion DELEGATES


#pragma region OUTPUT_ARRAYS

	//query outputs will be written to this variable
	UPROPERTY(BlueprintReadWrite)
		TArray<FNeo4jNode> stringQueryOutput;


	//returning from node functions
	UPROPERTY(BlueprintReadWrite)
		TArray<FNeo4jNode> getNodeQueryOutput;

	UPROPERTY(BlueprintReadWrite)
		TArray<FNeo4jNode> getNeighboursQueryOutput;

	UPROPERTY(BlueprintReadWrite)
		TArray<FNeo4jNode> createNodeQueryOutput;

	UPROPERTY(BlueprintReadWrite)
		TArray<FNeo4jNode> updateNodeQueryOutput;

	UPROPERTY(BlueprintReadWrite)
		TArray<FNeo4jNode> mergeNodeQueryOutput;


	//returning from relation functions




#pragma endregion OUTPUT_ARRAYS


private:
	FString URL;
	FString b64Auth;

public:

#pragma region GENERAL_FUNCTIONS

	//same functionality as QueryStrings but we can override the delegate function
	void QueryStrings(TArray<FString> inStrings, TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest);

	UFUNCTION(BlueprintCallable, Category = "Neo4j")
		void InitializeDatabase(FString IP, FString HTTPport, FString user, FString pass);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Posts array of strings as seperate queries"))
		void QueryStrings(TArray<FString> queries);



#pragma endregion GENERAL_FUNCTIONS

#pragma region NODE_FUNCTIONS

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Adds node to graph database then returns node. Maps the property name to the property value"))
		void CreateNode(TArray<FString> labels, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties,
			TMap<FString, bool> boolProperties);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Try to create new node, if node already exists it will return said node"))
		void MergeNode(TArray<FString> labels, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties,
			TMap<FString, bool> boolProperties);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Retrieves Neo4j Nodes/Relations by their IDs"))
		void GetNodesByID(TArray<int> elementIDs);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Finds an element by ID then deletes element"))
		void DeleteNodesByID(TArray<int> ElementID);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Finds element by ID then adds properties."))
		void AddPropertiesToNodes(TArray<int> ElementID, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties,
			TMap<FString, bool> boolProperties);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Finds element by ID then removes properties."))
		void RemovePropertiesFromNodes(TArray<int> ElementID, TArray<FString> propertyNames);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Matches all input properties to a set of nodes then detaches and deletes all nodes."))
		void DeleteNodesByProperties(TArray<FString> labels, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties,
			TMap<FString, bool> boolProperties);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Finds an element by ID then adds labels"))
		void AddLabelsToNodes(TArray<int> ElementID, TArray<FString> Labels);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Finds an element by ID then removes labels"))
		void RemoveLabelsFromNodes(TArray<int> ElementID, TArray<FString> Labels);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Finds all nodes matching the input labels"))
		void GetNodesByLabels(TArray<FString> Labels);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Gets all relations attached to node regardless of direction"))
		void GetNodeNeighbours(int nodeID);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Gets all relations attached to node of a certain type"))
		void GetNodeNeighboursByTypes(int nodeID, TArray<FString> relationTypes);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Gets a list of relationships going out from the input node."))
		void GetOutgoingNeighboursFromNode(int nodeID);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Gets a list of relationships going into input node"))
		void GetIncomingNeighboursFromNode(int nodeID);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Gets a list of relationships containing the input relation types going out from the input node."))
		void GetOutgoingNeighboursByTypes(int nodeID, TArray<FString> relationTypes);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Gets a list of relationships containing the input relation types going into input node"))
		void GetIncomingNeighboursByTypes(int nodeID, TArray<FString> relationTypes);

#pragma endregion NODE_FUNCTIONS


#pragma region RELATION_FUNCTIONS

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Adds relationships from root node to other nodes. Other nodes denoted by node ID"))
		void CreateRelations(int rootNodeID, TMap<FString, int> relationships);

	UFUNCTION(BlueprintCallable, Category = "Neo4j", meta = (Tooltip = "Trys to insert relation, if relation already exists updates current relation"))
		void MergeRelations(int relationID, TMap<FString, int> relationships);




#pragma endregion RELATION_FUNCTIONS


private:


#pragma region HELPERS


	//sends string to neo4j as a query.
	void _SendQuery(FString query, TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest);


#pragma endregion HELPERS

#pragma region DELEGATES

	void _OnStringQueryProcessed(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


#pragma region NODE_DELEGATE_FUNCTIONS

	void _OnCreateNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void _OnUpdateNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void _OnGetNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void _OnMergeNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void _OnGetNeighbour(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

#pragma endregion NODE_DELEGATE_FUNCTIONS


#pragma endregion DELEGATES
};