// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Neo4jFilters.h"

int UNeo4jFilters::FilterIntProperty(FNeo4jNode inNode, FString property)
{

	return inNode.properties.Find(property)->ToSharedRef()->AsNumber();

}

FString UNeo4jFilters::FilterStringProperty(FNeo4jNode inNode, FString property)
{
	return inNode.properties.Find(property)->ToSharedRef()->AsString();
}

bool UNeo4jFilters::FilterBoolProperty(FNeo4jNode inNode, FString property)
{
	return inNode.properties.Find(property)->ToSharedRef()->AsBool();
}
