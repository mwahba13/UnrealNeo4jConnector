// Fill out your copyright notice in the Description page of Project Settings.


#include "Neo4jDatabase.h"

#include "Neo4jUtilities.h"

#pragma region GENERAL_FUNCTIONS

void UNeo4jDatabase::InitializeDatabase(FString IP, FString HTTPport, FString user, FString pass)
{
	URL = "http://" + IP + ":" + HTTPport + "/db/neo4j/tx";


	b64Auth = FBase64::Encode(user.Append(":").Append(pass));
	b64Auth = "Basic " + b64Auth;

}



void UNeo4jDatabase::QueryStrings(TArray<FString> queries)
{

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();
	httpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnStringQueryProcessed);
	
	QueryStrings(queries, httpRequest);
}



#pragma endregion GENERAL_FUNCTIONS

#pragma region NODE_FUNCTIONS

void UNeo4jDatabase::CreateNode(TArray<FString> labels, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties,
	TMap<FString, bool> boolProperties)
{

	TArray<FString> queryArray;

	FString query = "Create (" + UNeo4jUtilities::SerializeLabelsIntoQuery(labels) +
		UNeo4jUtilities::SerializePropertiesIntoQuery(stringProperties, intProperties, boolProperties) + ") return m";

	queryArray.Add(query);

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();
	httpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnCreateNode);


	QueryStrings(queryArray, httpRequest);
}

void UNeo4jDatabase::MergeNode(TArray<FString> labels, TMap<FString, FString> stringProperties,
	TMap<FString, int> intProperties, TMap<FString, bool> boolProperties)
{
	TArray<FString> queryArray;

	FString query = "Merge (" + UNeo4jUtilities::SerializeLabelsIntoQuery(labels) +
		UNeo4jUtilities::SerializePropertiesIntoQuery(stringProperties, intProperties, boolProperties) + ") return m";

	queryArray.Add(query);

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();
	httpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnMergeNode);

	QueryStrings(queryArray, httpRequest);
}

void UNeo4jDatabase::DeleteNodesByProperties(TArray<FString> labels, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties, TMap<FString, bool> boolProperties)
{
	TArray<FString> queryArray;

	FString matchQuery = "Match (" + UNeo4jUtilities::SerializeLabelsIntoQuery(labels)
		+ UNeo4jUtilities::SerializePropertiesIntoQuery(stringProperties, intProperties, boolProperties) + ")";

	queryArray.Add(matchQuery);
	queryArray.Add("detach delete m");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();
	QueryStrings(queryArray, httpRequest);

}

void UNeo4jDatabase::GetNodesByID(TArray<int> elementIDs)
{
	TArray<FString> queryArray;
	FString queryString = "with[";

	if (elementIDs.Num() == 0)
		return;

	else if (elementIDs.Num() == 1)
	{
		queryString.AppendInt(elementIDs[0]);
	}
	else
	{
		for (int i = 0; i < elementIDs.Num(); i++)
		{
			queryString.AppendInt(elementIDs[i]);
			if (i != elementIDs.Num() - 1)
				queryString.Append(",");
		}
	}

	queryString = queryString + "] as range";
	queryArray.Add(queryString);
	queryArray.Add("unwind range as n");
	queryArray.Add("match(m) where id(m) = n");
	queryArray.Add("return m");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNode);

	QueryStrings(queryArray, HttpRequest);



}

void UNeo4jDatabase::AddPropertiesToNodes(TArray<int> elementIDs, TMap<FString, FString> stringProperties, TMap<FString, int> intProperties, TMap<FString, bool> boolProperties)
{
	TArray<FString> queryArray;
	FString queryString = "with[";

	if (elementIDs.Num() == 0)
		return;

	else if (elementIDs.Num() == 1)
	{
		queryString.AppendInt(elementIDs[0]);
	}
	else
	{
		for (int i = 0; i < elementIDs.Num(); i++)
		{
			queryString.AppendInt(elementIDs[i]);
			if (i != elementIDs.Num() - 1)
				queryString.Append(",");
		}
	}

	queryString = queryString + "] as range";
	queryArray.Add(queryString);
	queryArray.Add("unwind range as n");
	queryArray.Add("match(m) where id(m) = n");
	FString setString = "set m += " + UNeo4jUtilities::SerializePropertiesIntoQuery(stringProperties, intProperties, boolProperties);
	queryArray.Add(setString);

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnUpdateNode);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::RemovePropertiesFromNodes(TArray<int> elementIDs, TArray<FString> propertiesToRemove)
{
	TArray<FString> queryArray;
	FString queryString = "with[";

	if (elementIDs.Num() == 0)
		return;

	else if (elementIDs.Num() == 1)
	{
		queryString.AppendInt(elementIDs[0]);
	}

	else
	{
		for (int i = 0; i < elementIDs.Num(); i++)
		{
			queryString.AppendInt(elementIDs[i]);
			if (i != elementIDs.Num() - 1)
				queryString.Append(",");
		}
	}

	queryString = queryString + "] as range";

	queryArray.Add(queryString);
	queryArray.Add("unwind range as n");
	queryArray.Add("match(m) where id(m) = n");

	//remove m.propertyName
	for (auto& prop : propertiesToRemove)
	{
		queryArray.Add("remove m." + prop);
	}


	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnUpdateNode);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::AddLabelsToNodes(TArray<int> elementIDs, TArray<FString> Labels)
{
	TArray<FString> queryArray;
	FString queryString = "with[";

	if (elementIDs.Num() == 0)
		return;

	else if (elementIDs.Num() == 1)
	{
		queryString.AppendInt(elementIDs[0]);
	}
	else
	{
		for (int i = 0; i < elementIDs.Num(); i++)
		{
			queryString.AppendInt(elementIDs[i]);
			if (i != elementIDs.Num() - 1)
				queryString.Append(",");
		}
	}

	queryString = queryString + "] as range";

	queryArray.Add(queryString);
	queryArray.Add("unwind range as n");
	queryArray.Add("match(m) where id(m) = n");

	for (auto& label : Labels)
	{
		queryArray.Add("set m:" + label);
	}

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnUpdateNode);

	QueryStrings(queryArray, HttpRequest);

}

void UNeo4jDatabase::RemoveLabelsFromNodes(TArray<int> elementIDs, TArray<FString> Labels)
{
	TArray<FString> queryArray;
	FString queryString = "with[";

	if (elementIDs.Num() == 0)
		return;

	else if (elementIDs.Num() == 1)
	{
		queryString.AppendInt(elementIDs[0]);
	}
	else
	{
		for (int i = 0; i < elementIDs.Num(); i++)
		{
			queryString.AppendInt(elementIDs[i]);
			if (i != elementIDs.Num() - 1)
				queryString.Append(",");
		}
	}

	queryString = queryString + "] as range";

	queryArray.Add(queryString);
	queryArray.Add("unwind range as n");
	queryArray.Add("match(m) where id(m) = n");

	for (auto& label : Labels)
	{
		queryArray.Add("remove m:" + label);
	}

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnUpdateNode);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::DeleteNodesByID(TArray<int> elementIDs)
{
	TArray<FString> queryArray;
	FString queryString = "with[";

	if (elementIDs.Num() == 0)
		return;

	else if (elementIDs.Num() == 1)
	{
		queryString.AppendInt(elementIDs[0]);
	}
	else
	{
		for (int i = 0; i < elementIDs.Num(); i++)
		{
			queryString.AppendInt(elementIDs[i]);
			if (i != elementIDs.Num() - 1)
				queryString.Append(",");
		}
	}

	queryString = queryString + "] as range";
	queryArray.Add(queryString);
	queryArray.Add("unwind range as n");
	queryArray.Add("match(m) where id(m) = n");
	queryArray.Add("detach delete m");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNode);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::GetNodesByLabels(TArray<FString> Labels)
{
	TArray<FString> queryArray;
	queryArray.Add("Match (" + UNeo4jUtilities::SerializeLabelsIntoQuery(Labels) + ")");
	queryArray.Add("return m");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);
}


void UNeo4jDatabase::GetNodeNeighbours(int nodeID)
{
	TArray<FString> queryArray;

	FString matchString = "Match (m) where id(m) = ";
	matchString.AppendInt(nodeID);

	queryArray.Add(matchString);
	queryArray.Add("match (m) -- (n) return n");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);

}

void UNeo4jDatabase::GetNodeNeighboursByTypes(int nodeID, TArray<FString> relationType)
{
	TArray<FString> queryArray;

	FString matchString = "Match (p) where id(p) = ";
	matchString.AppendInt(nodeID);

	queryArray.Add(matchString);
	queryArray.Add("match (p) -[" + UNeo4jUtilities::SerializeLabelsIntoQuery(relationType) + "]- (n) return n");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::GetIncomingNeighboursFromNode(int nodeID)
{
	TArray<FString> queryArray;

	FString matchString = "Match (p) where id(p) = ";
	matchString.AppendInt(nodeID);

	queryArray.Add(matchString);
	queryArray.Add("match (p) <-- (n) return n");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::GetOutgoingNeighboursFromNode(int nodeID)
{
	TArray<FString> queryArray;

	FString matchString = "Match (p) where id(p) = ";
	matchString.AppendInt(nodeID);

	queryArray.Add(matchString);
	queryArray.Add("match (p) --> (n) return n");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);
}

void UNeo4jDatabase::GetIncomingNeighboursByTypes(int nodeID, TArray<FString> relationTypes)
{
	TArray<FString> queryArray;

	FString matchString = "Match (p) where id(p) = ";
	matchString.AppendInt(nodeID);

	queryArray.Add(matchString);
	queryArray.Add("match (p) <-[" + UNeo4jUtilities::SerializeLabelsIntoQuery(relationTypes) + "]- (n) return n");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);
}


void UNeo4jDatabase::GetOutgoingNeighboursByTypes(int nodeID, TArray<FString> relationTypes)
{
	TArray<FString> queryArray;

	FString matchString = "Match (p) where id(p) = ";
	matchString.AppendInt(nodeID);

	queryArray.Add(matchString);
	queryArray.Add("match (p) -[" + UNeo4jUtilities::SerializeLabelsIntoQuery(relationTypes) + "]-> (n) return n");

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNeo4jDatabase::_OnGetNeighbour);

	QueryStrings(queryArray, HttpRequest);
}



#pragma endregion  NODE_FUNCTIONS




#pragma region RELATION_FUNCTIONS

//direction of relationship is left to right
void UNeo4jDatabase::CreateRelations(int nodeID, TMap<FString, int> relationships)
{

	TArray<FString> queryArray;

	TArray<int> nodeArray;
	relationships.GenerateValueArray(nodeArray);

	TArray<FString> stringArray;
	relationships.GenerateKeyArray(stringArray);

	FString queryString;

	//build the queries as one big string
	for (int i = 0; i < relationships.Num(); i++)
	{
		//match (n) where id(n) = *nodeID
		queryString = queryString + " Match (n) where id(n) =";
		queryString.AppendInt(nodeID);

		//match (ni) where id(ni) = *nodeID 
		queryString = queryString + " Match (n";
		queryString.AppendInt(i);
		queryString = queryString + ") where id (n";
		queryString.AppendInt(i);
		queryString = queryString + ") = ";
		queryString.AppendInt(nodeArray[i]);

		//create (n) - [: *relationships] -> (ni)
		queryString = queryString + " Create (n) - [:" + stringArray[i] + "] -> (n";
		queryString.AppendInt(i);
		queryString = queryString + ")";


		//allows us to string these queries together
		if (i != relationships.Num() - 1)
			queryString.Append(" UNION ALL");

	}

	queryArray.Add(queryString);


	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();
	//httpRequest->OnProcessRequestComplete().BindUObject(this,&UNeo4jDatabase::_OnCreateRelation);

	QueryStrings(queryArray, httpRequest);

}

void UNeo4jDatabase::MergeRelations(int relationID, TMap<FString, int> relationships)
{
	TArray<FString> queryArray;

	TArray<int> nodeArray;
	relationships.GenerateValueArray(nodeArray);

	TArray<FString> stringArray;
	relationships.GenerateKeyArray(stringArray);

	FString queryString;

	//build the queries as one big string
	for (int i = 0; i < relationships.Num(); i++)
	{
		//match (n) where id(n) = *nodeID
		queryString = queryString + " Match (n) where id(n) =";
		queryString.AppendInt(relationID);

		//match (ni) where id(ni) = *nodeID 
		queryString = queryString + " Match (n";
		queryString.AppendInt(i);
		queryString = queryString + ") where id (n";
		queryString.AppendInt(i);
		queryString = queryString + ") = ";
		queryString.AppendInt(nodeArray[i]);

		//create (n) - [: *relationships] -> (ni)
		queryString = queryString + " Merge (n) - [:" + stringArray[i] + "] -> (n";
		queryString.AppendInt(i);
		queryString = queryString + ")";


		//allows us to string these queries together
		if (i != relationships.Num() - 1)
			queryString.Append(" UNION ALL");

	}

	queryArray.Add(queryString);

	TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();
	//httpRequest->OnProcessRequestComplete().BindUObject(this,&UNeo4jDatabase::_OnMergeRelation);

	QueryStrings(queryArray, httpRequest);
}



#pragma endregion RELATION_FUNCTIONS



#pragma region HELPERS

//set up your delegate before calling this!
void UNeo4jDatabase::QueryStrings(TArray<FString> inStrings, TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest)
{

	FString queryList;

	for (auto& string : inStrings)
	{
		queryList = queryList + "\n" + string;
	}

	FString query = UNeo4jUtilities::_ConstructJSONQueryString(queryList);

	UE_LOG(LogTemp, Warning, TEXT("Query Strings input: %s"), *query);

	_SendQuery(query, httpRequest);



}



void UNeo4jDatabase::_SendQuery(FString query, TSharedRef<IHttpRequest, ESPMode::NotThreadSafe> httpRequest)
{
	httpRequest->SetURL(URL + "/commit");
	httpRequest->SetHeader("Authorization", b64Auth);
	httpRequest->SetHeader("Accept", "application/json;charset=UTF-8");
	httpRequest->SetHeader("Content-Type", "application/json");
	httpRequest->SetVerb("POST");
	httpRequest->SetContentAsString(query);
	httpRequest->ProcessRequest();
}



#pragma endregion HELPERS




#pragma region DELEGATE_FUNCTIONS

void UNeo4jDatabase::_OnStringQueryProcessed(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("String Query Result: %s"), *Response->GetContentAsString());

		stringQueryOutput = UNeo4jUtilities::DeserializeNodeQueryResult(Response->GetContentAsString());


		OnStringQueryCompleteDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response was invalid!"));
		return;
	}

}

#pragma region NODE_DELEGATE_FUNCTIONS

void UNeo4jDatabase::_OnCreateNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

	if (bWasSuccessful)
	{
		FString temp = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("OnCreateNodeResponse: %s"), *temp);

		createNodeQueryOutput = UNeo4jUtilities::DeserializeNodeQueryResult(temp);

		OnCreateNodeCompleteDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response was invalid!"));
		return;
	}



}

void UNeo4jDatabase::_OnGetNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString temp = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("OnGetNodeResponse: %s"), *temp);

		getNodeQueryOutput = UNeo4jUtilities::DeserializeNodeQueryResult(temp);

		OnGetNodeCompleteDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response was invalid!"));
		return;
	}

}

void UNeo4jDatabase::_OnMergeNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString temp = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("OnMergeNodeResponse: %s"), *temp);

		mergeNodeQueryOutput = UNeo4jUtilities::DeserializeNodeQueryResult(temp);

		OnMergeNodeCompleteDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response was invalid!"));
		return;
	}
}

void UNeo4jDatabase::_OnUpdateNode(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString temp = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("OnUpdateNodeResponse: %s"), *temp);
		updateNodeQueryOutput = UNeo4jUtilities::DeserializeNodeQueryResult(temp);
		OnUpdateNodeCompleteDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response was invalid!"));
		return;
	}
}


#pragma endregion NODE_DELEGATE_FUNCTIONS


#pragma region RELATION_DELEGATE_FUNCTIONS

void UNeo4jDatabase::_OnGetNeighbour(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString temp = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("OnGetNeighbour Response: %s"), *temp);
		getNeighboursQueryOutput = UNeo4jUtilities::DeserializeNodeQueryResult(temp);
		OnGetNeighbourCompleteDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response was invalid!"));
		return;
	}
}



#pragma endregion RELATION_DELEGATE_FUNCTIONS


#pragma endregion DELEGATE_FUNCTIONS

