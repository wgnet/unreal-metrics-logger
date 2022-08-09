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


#include "InfluxDBLogger.h"
#include "Http.h"
#include "MetricsLoggerSettings.h"


FInfluxDBLogger::FInfluxDBLogger()
{
	// Create the lineprotocol metadata tag string once
	// Spaces need to be escaped within the string
	TagString = FString::Printf(
		TEXT("project_name=%s,cpu_model=%s,cpu_core_count=%s,gpu_model=%s,ram_size=%s,machine_name=%s,unreal_version=%s,extension_version=%s"),
		*ProjectName.Replace(TEXT(" "), TEXT("\\ ")),
		*CpuModel.Replace(TEXT(" "), TEXT("\\ ")),
		*CoreCount.Replace(TEXT(" "), TEXT("\\ ")),
		*GpuModel.Replace(TEXT(" "), TEXT("\\ ")),
		*RamSize.Replace(TEXT(" "), TEXT("\\ ")),
		*MachineName.Replace(TEXT(" "), TEXT("\\ ")),
		*UnrealVersion.Replace(TEXT(" "), TEXT("\\ ")),
		*ExtensionVersion.Replace(TEXT(" "), TEXT("\\ "))
		);
}

void FInfluxDBLogger::Log(const EventMetaData& data)
{
	// Get settings
	const UMetricsLoggerSettings* Settings = GetDefault<UMetricsLoggerSettings>();

	if (!Settings->EnableLogging) return;

	// Format metadata
	FString lineProtocolData = ToLineProtocol(data);

	// Log according to the version of InfluxDB is configured
	Settings->InfluxVersion == InfluxDBVersion::V1 ? LogV1(lineProtocolData) : LogV2(lineProtocolData);
}

// Log to the InfluxDB v1.8 API
void FInfluxDBLogger::LogV1(const FString& content)
{
	const UMetricsLoggerSettings* Settings = GetDefault<UMetricsLoggerSettings>();

	const FString& baseURL = Settings->InfluxURL;
	const FString& db = Settings->InfluxDatabase;
	const FString& user = Settings->InfluxUser;
	const FString& pw = Settings->InfluxPassword;

	// Check we have some form of valid data
	if (baseURL.IsEmpty() || db.IsEmpty() || user.IsEmpty() || pw.IsEmpty()) {
		UE_LOG(MetricsLog, Error, TEXT("Cannot log metrics - incomplete configuration."));
		return;
	}

	// Generate the full URL
	FString writeUrl = FString::Printf(TEXT("%s/write?db=%s&u=%s&p=%s&precision=s"), *baseURL, *db, *user, *pw);

	SendLog(writeUrl, content);
}

// Log to the InfluxDB 2.0+ API
void FInfluxDBLogger::LogV2(const FString& content)
{
	const UMetricsLoggerSettings* Settings = GetDefault<UMetricsLoggerSettings>();

	const FString& baseURL = Settings->InfluxURL;
	const FString& token = Settings->InfluxToken;
	const FString& org = Settings->InfluxOrganisation;
	const FString& bucket = Settings->InfluxBucket;

	// Check we have some form of valid data
	if (baseURL.IsEmpty() || token.IsEmpty() || org.IsEmpty() || bucket.IsEmpty()) {
		UE_LOG(MetricsLog, Error, TEXT("Cannot log metrics - incomplete configuration."));
		return;
	}

	// Generate the full URL
	FString writeUrl = FString::Printf(TEXT("%s/api/v2/write?bucket=%s&org=%s&precision=s"), *baseURL, *bucket, *org);

	SendLog(writeUrl, content, token);
}

void FInfluxDBLogger::SendLog(const FString& writeUrl, const FString& content, const FString& authorization)
{
	UE_LOG(MetricsLog, Log, TEXT("Logging %s to %s"), *content, *writeUrl);

	// Construct the request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
	request->SetURL(writeUrl);
	request->SetVerb("POST");
	request->SetHeader(TEXT("Content-Type"), TEXT("text/plain; charset=utf-8"));
	request->SetContentAsString(content);

	if (!authorization.IsEmpty()) {
		request->SetHeader(TEXT("Authorization"), *authorization);
	}

	request->OnProcessRequestComplete().BindLambda([this](const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bWasSuccessful) {
		OnLogSendComplete(Request, Response, bWasSuccessful);
		});

	request->ProcessRequest();
}

void FInfluxDBLogger::OnLogSendComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bWasSuccessful) const
{
	if (!bWasSuccessful) {
		UE_LOG(MetricsLog, Error, TEXT("Submitting log failed with return code: %s"), *FString::FromInt(Response->GetResponseCode()));
		UE_LOG(MetricsLog, Error, TEXT("Return content is: %s"),*Response->GetContentAsString());
	} else {
		UE_LOG(MetricsLog, Log, TEXT("Log submitted successfully!"));
	}
}

FString FInfluxDBLogger::ToLineProtocol(const EventMetaData& data) const
{
	// Format specified here: https://docs.influxdata.com/influxdb/v1.8/write_protocols/line_protocol_tutorial/

	const UMetricsLoggerSettings* Settings = GetDefault<UMetricsLoggerSettings>();
	const TCHAR* eventType = MetricsLoggerUtils::LogEventTypeToFString(data.type);
	const TCHAR* success = data.success ? TEXT("True") : TEXT("False");
	const TCHAR* user = Settings->LogUser ? *Username : TEXT("N/A");

	// Construct and return the final string format
	FString lineProtocol = FString::Printf(
		TEXT("%s,%s,success=%s,user=%s event_start=%d,event_finish=%d,event_duration=%.2f %lld"), 
		eventType, 
		*TagString,
		success,
		user,
		data.startTime.ToUnixTimestamp(),
		data.finishTime.ToUnixTimestamp(),
		data.duration, 
		data.startTime.ToUnixTimestamp());

	return lineProtocol;
}
