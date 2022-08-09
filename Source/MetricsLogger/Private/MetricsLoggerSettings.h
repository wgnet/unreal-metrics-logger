// Copyright (c) 2021-2022 Wargaming.net. All rights reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "CoreMinimal.h"

#include "MetricsLoggerSettings.generated.h"

UENUM()
enum class InfluxDBVersion {
	V1 UMETA(DisplayName = "v1.8"),
	V2 UMETA(DisplayName = "v2.0")
};

/**
 * Class for defining a settings page in the Editor preferences window.
 */
UCLASS(config = Editor, defaultconfig)
class UMetricsLoggerSettings: public UObject
{
	GENERATED_BODY()
public:
	UMetricsLoggerSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig)
	InfluxDBVersion InfluxVersion;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig)
	bool LogUser;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig)
	bool EnableLogging;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig)
	FString InfluxURL;

	// InfluxDB V1 Config - Only visible if InfluxDBVerion is equal to InfluxDBVersion::V1
	UPROPERTY(config, EditAnywhere, Category = LoggingConfig, meta = (EditCondition = "InfluxVersion == InfluxDBVersion::V1", EditConditionHides))
	FString InfluxUser;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig, meta = (PasswordField = true, EditCondition = "InfluxVersion == InfluxDBVersion::V1", EditConditionHides))
	FString InfluxPassword;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig, meta = (EditCondition = "InfluxVersion == InfluxDBVersion::V1", EditConditionHides))
	FString InfluxDatabase;

	// InfluxDB V2 Config - Only visible if InfluxDBVerion is equal to InfluxDBVersion::V1
	UPROPERTY(config, EditAnywhere, Category = LoggingConfig, meta = (PasswordField = true, EditCondition = "InfluxVersion == InfluxDBVersion::V2", EditConditionHides))
	FString InfluxToken;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig, meta = (EditCondition = "InfluxVersion == InfluxDBVersion::V2", EditConditionHides))
	FString InfluxOrganisation;

	UPROPERTY(config, EditAnywhere, Category = LoggingConfig, meta = (EditCondition = "InfluxVersion == InfluxDBVersion::V2", EditConditionHides))
	FString InfluxBucket;
};
