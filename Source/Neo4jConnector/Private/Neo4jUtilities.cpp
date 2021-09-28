// Fill out your copyright notice in the Description page of Project Settings.


#include "Neo4jUtilities.h"

#include <string>

#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"





//turns a string into serialized JSON format ready to send to Neo4j
FString UNeo4jUtilities::_ConstructJSONQueryString(FString stringToSerialize)
{
	TSharedPtr<FJsonObject> statementObj = MakeShareable(new FJsonObject());
	statementObj->SetStringField("statement", stringToSerialize);

	TArray<TSharedPtr<FJsonValue>> statementsArray;
	statementsArray.Add(MakeShareable(new FJsonValueObject(statementObj)));

	TSharedPtr<FJsonObject> queryJsonObj = MakeShareable(new FJsonObject());
	queryJsonObj->SetArrayField("statements", statementsArray);

	FString jsonQueryString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&jsonQueryString);
	FJsonSerializer::Serialize(queryJsonObj.ToSharedRef(), JsonWriter);

	return jsonQueryString;

}



//takes in an array of labels and serializes it into CYPHER format
FString UNeo4jUtilities::SerializeLabelsIntoQuery(TArray<FString> labels)
{
	FString outString = "m";

	if (labels.Num() == 1 && labels[0].IsEmpty())
	{
		return outString;
	}

	for (auto& label : labels)
	{
		outString = outString + ":" + label;
	}

	return outString;
}

//takes in maps of propertyname:propertyValue and serializes it into CYPHER format {...}
FString UNeo4jUtilities::SerializePropertiesIntoQuery(TMap<FString, FString> stringProps, TMap<FString, int> intProps,
	TMap<FString, bool> boolProps)
{
	FString outString = "{";

	if (!stringProps.begin().Key().IsEmpty())
	{
		for (auto& stringProp : stringProps)
		{
			outString = outString + stringProp.Key + ":\"" + stringProp.Value + "\",";

		}
	}


	if (!intProps.begin().Key().IsEmpty())
	{


		for (auto& intProp : intProps)
		{

			outString = outString + intProp.Key + ":";
			outString.AppendInt(intProp.Value);
			outString = outString + ",";
		}


	}

	if (!boolProps.begin().Key().IsEmpty())
	{


		for (auto& boolProp : boolProps)
		{
			outString = outString + boolProp.Key + ":";

			if (boolProp.Value)
				outString = outString + "true,";
			else
				outString = outString + "false,";

		}

	}

	//check if nothing was added/ all inputs are empty
	if (outString.Len() == 1)
		return "";

	//chop off end comma if necessary
	if (outString.EndsWith(","))
		outString.LeftChopInline(1, true);

	outString = outString + "}";



	return outString;
}

//FNeo4jNode -> {labelName:labelValue...propertyName:PropertyValue}
FString UNeo4jUtilities::SerializeNode(FNeo4jNode inNode)
{
	return "null";
}








//takes in raw output from query and creates an array of Neo4jNodes
TArray<FNeo4jNode> UNeo4jUtilities::DeserializeNodeQueryResult(FString resultString)
{
	TArray<FNeo4jNode> outArray;

	//make json object out of result
	TSharedPtr<FJsonObject> jsonObjectResult = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(resultString);
	if (FJsonSerializer::Deserialize(jsonReader, jsonObjectResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("Successful Node Query Deserialize"));
		TArray<TSharedPtr<FJsonValue>> resultsArray = jsonObjectResult->GetArrayField("results");

		for (auto& result : resultsArray)
		{
			TArray<TSharedPtr<FJsonValue>> dataArray = result->AsObject()->GetArrayField("data");

			//foreach row in data array
			for (auto& dataElement : dataArray)
			{
				//create neo4j node
				FNeo4jNode newNode;

				TArray<TSharedPtr<FJsonValue>> rowArray = dataElement->AsObject()->GetArrayField("row");
				TArray<TSharedPtr<FJsonValue>> metaArray = dataElement->AsObject()->GetArrayField("meta");

				//foreach object in row array
				for (auto& rowElement : rowArray)
				{
					TSharedPtr<FJsonObject> rowObj = rowElement->AsObject();
					newNode.properties = rowObj->Values;

				}

				//foreach object in meta array
				for (auto& metaElement : metaArray)
				{
					TSharedPtr<FJsonObject> metaObj = metaElement->AsObject();
					newNode.id = metaObj->GetIntegerField("id");
				}

				outArray.Add(newNode);
			}
		}

	}


	return outArray;
}

