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

#include "CoreTypes.h"

// Parent Class
#include "IMetricsLogger.h"

struct EventMetaData;
typedef TSharedPtr<class IHttpRequest, ESPMode::ThreadSafe> FHttpRequestPtr;
typedef TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe> FHttpResponsePtr;

/**
 * Specific implementation of a MetricsLogger for pushing data to InfluxDB.
 */
class FInfluxDBLogger: public IMetricsLogger
{
public:
	FInfluxDBLogger();

	void Log(const EventMetaData& data) override;

private:
	void LogV1(const FString& content);
	void LogV2(const FString& content);
	void SendLog(const FString& writeUrl, const FString& content, const FString& authorization = FString());
	void OnLogSendComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bWasSuccessful) const;

	FString ToLineProtocol(const EventMetaData& data) const;

	FString TagString;
};
