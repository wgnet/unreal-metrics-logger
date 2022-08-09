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


#include "MetricsLoggerEventMonitor.h"
#include "MetricsLogCategory.h"

// Event Identifiers
const TCHAR* const RECOMPILE_EVENT = TEXT("Editor.Modules.Recompile");

const TCHAR* const COOK_START_EVENT = TEXT("Editor.Cook.Start");
const TCHAR* const COOK_STOP_EVENT = TEXT("Editor.Cook.Completed");
const TCHAR* const COOK_FAILED_EVENT = TEXT("Editor.Cook.Failed");

const TCHAR* const PACKAGE_START_EVENT = TEXT("Editor.Package.Start");
const TCHAR* const PACKAGE_STOP_EVENT = TEXT("Editor.Package.Completed");
const TCHAR* const PACKAGE_FAILED_EVENT = TEXT("Editor.Package.Failed");

FMetricsLoggerEventMonitor::FMetricsLoggerEventMonitor(IMetricsLogger& logger): MetricsLogger(logger)
{
	// Store map of functions to call for each event fired that we care about

	// Recompile event
	EventHandlers.Add(RECOMPILE_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnRecompile(EventName, Attrs, bJson);
	});

	// Cook events
	EventHandlers.Add(COOK_START_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnCookStart(EventName, Attrs, bJson);
	});
	EventHandlers.Add(COOK_STOP_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnCookStop(EventName, Attrs, bJson);
	});
	EventHandlers.Add(COOK_FAILED_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnCookFailed(EventName, Attrs, bJson);
	});

	// Package events
	EventHandlers.Add(PACKAGE_START_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnPackageStart(EventName, Attrs, bJson);
	});
	EventHandlers.Add(PACKAGE_STOP_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnPackageStop(EventName, Attrs, bJson);
	});
	EventHandlers.Add(PACKAGE_FAILED_EVENT, [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		OnPackageFailed(EventName, Attrs, bJson);
	});

	FOnGlobalShadersCompilation& shaderCompileDelegate = GetOnGlobalShaderCompilation();
	shaderCompileDelegate.AddLambda([this]() {
		OnShaderStart();
		});
}

void FMetricsLoggerEventMonitor::ProcessEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	// Log event name
	UE_LOG(MetricsLog, Verbose, TEXT("FEngineAnalytics Event fired: %s"), *EventName);

	if (IAnalyticsProviderET::OnEventRecorded* handler = EventHandlers.Find(EventName)) {
		(*handler)(EventName, Attrs, bJson);
	}
}

void FMetricsLoggerEventMonitor::Tick(float DeltaTime)
{
	// Need to do poll the ShaderManager to see when it finishes compiling (unfortunately)
	if (shaderCompileInProgress && GShaderCompilingManager && !GShaderCompilingManager->IsCompiling()) {
		LogShaderEvent();
	}

}

void FMetricsLoggerEventMonitor::OnRecompile(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	// There's no start event, so we have to trust the duration provided by the event metadata
	FString duration;
	bool success = false;
	for (const FAnalyticsEventAttribute& attr : Attrs) {
		UE_LOG(MetricsLog, Verbose, TEXT("Attr %s is %s"), *attr.GetName(), *attr.GetValue());

		if (attr.GetName() == TEXT("Duration")) {
			duration = attr.GetValue();
		}
		else if (attr.GetName() == TEXT("Result")) {
			success = attr.GetValue() == TEXT("Succeeded");
		}
	}

	// Log if we have valid information
	if (!duration.IsEmpty()) {
		EventMetaData eventData = EventMetaData();
		eventData.type = LogEventTypeEnum::BUILD;
		eventData.finishTime = FDateTime::UtcNow();
		eventData.duration = FCString::Atoi(*duration);
		eventData.startTime = eventData.finishTime - FTimespan(0, 0, eventData.duration);
		eventData.success = success;
		MetricsLogger.Log(eventData);
	}
}

void FMetricsLoggerEventMonitor::OnCookStart(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	// Don't register a new event if there's one already active - a double event will result in a failure anyway
	if (!cookInProgress) {
		CurrentCookEvent = EventMetaData();
		CurrentCookEvent.startTime = FDateTime::UtcNow();
		CurrentCookEvent.type = LogEventTypeEnum::COOK;
		cookInProgress = true;
	}
}

void FMetricsLoggerEventMonitor::OnCookStop(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	LogCookEvent(Attrs, true);
}

void FMetricsLoggerEventMonitor::OnCookFailed(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	LogCookEvent(Attrs, false);
}

void FMetricsLoggerEventMonitor::LogCookEvent(const TArray<FAnalyticsEventAttribute>& Attrs, bool success)
{
	// Ignore the second failure when someone attempts to cook twice
	if (cookInProgress) {
		CurrentCookEvent.finishTime = FDateTime::UtcNow();
		CurrentCookEvent.duration = (CurrentCookEvent.finishTime - CurrentCookEvent.startTime).GetTotalSeconds();
		CurrentCookEvent.success = success;
		MetricsLogger.Log(CurrentCookEvent);

		cookInProgress = false;
	}
}


void FMetricsLoggerEventMonitor::OnPackageStart(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	// Don't register a new event if there's one already active - a double event will result in a failure anyway
	if (!packageInProgress) {
		packageInProgress = true;

		CurrentPackageEvent = EventMetaData();
		CurrentPackageEvent.startTime = FDateTime::UtcNow();
		CurrentPackageEvent.type = LogEventTypeEnum::PACKAGE;
	}
}

void FMetricsLoggerEventMonitor::OnPackageStop(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	LogPackageEvent(Attrs, true);
}

void FMetricsLoggerEventMonitor::OnPackageFailed(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson)
{
	LogPackageEvent(Attrs, false);
}

void FMetricsLoggerEventMonitor::LogPackageEvent(const TArray<FAnalyticsEventAttribute>& Attrs, bool success)
{
	// Ignore the second failure when someone attempts to package twice
	if (packageInProgress) {
		packageInProgress = false;

		CurrentPackageEvent.finishTime = FDateTime::UtcNow();
		CurrentPackageEvent.duration = (CurrentPackageEvent.finishTime - CurrentPackageEvent.startTime).GetTotalSeconds();
		CurrentPackageEvent.success = success;
		MetricsLogger.Log(CurrentPackageEvent);

	}
}

void FMetricsLoggerEventMonitor::OnShaderStart()
{
	CurrentShaderEvent = EventMetaData();
	CurrentShaderEvent.startTime = FDateTime::UtcNow();
	CurrentShaderEvent.type = LogEventTypeEnum::SHADER;

	shaderCompileInProgress = true;
}

void FMetricsLoggerEventMonitor::LogShaderEvent()
{
	shaderCompileInProgress = false;

	CurrentShaderEvent.finishTime = FDateTime::UtcNow();
	CurrentShaderEvent.duration = (CurrentShaderEvent.finishTime - CurrentShaderEvent.startTime).GetTotalSeconds();
	CurrentShaderEvent.success = true;

	MetricsLogger.Log(CurrentShaderEvent);
}


