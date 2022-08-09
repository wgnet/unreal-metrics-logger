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
#include "CoreTypes.h"

// Source of Analytic Events
#include "IAnalyticsProviderET.h"
#include "ShaderCompiler.h"

// Data Models
#include "MetricsModel.h"

// Allow the monitor to be called every tick in the editor
#include "TickableEditorObject.h"

class IMetricsLogger;

/**
 * Class that filters and distributed events triggered by the Unreal AnalyticsProvider service to their corresponding handler
 * and logs event data through the supplied MetricsLogger.
 */
class FMetricsLoggerEventMonitor: public FTickableEditorObject 
{
public:
	FMetricsLoggerEventMonitor(IMetricsLogger& logger);

	void ProcessEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	
	// FTickableEditorObject overrides
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMetricsLoggerEventMonitor, STATGROUP_Tickables);
	}
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}
	virtual bool IsTickableInEditor() const
	{
		return true;
	}

private:

	// Map of Event Handlers
	TMap<FString, IAnalyticsProviderET::OnEventRecorded> EventHandlers;

	// Event Handlers
	void OnRecompile(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);

	// Cook events
	void OnCookStart(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	void OnCookStop(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	void OnCookFailed(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	void LogCookEvent(const TArray<FAnalyticsEventAttribute>& Attrs, bool success);

	// Package Events
	void OnPackageStart(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	void OnPackageStop(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	void OnPackageFailed(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson);
	void LogPackageEvent(const TArray<FAnalyticsEventAttribute>& Attrs, bool success);

	// Shader Compile Events
	void OnShaderStart();
	void LogShaderEvent();

	// Logger for reporting event stats
	IMetricsLogger& MetricsLogger;

	// Metadata for tracking the current cook process
	EventMetaData CurrentCookEvent;
	bool cookInProgress { false };

	// Metadata for storing current package event
	EventMetaData CurrentPackageEvent;
	bool packageInProgress{ false };

	// Flag for tracking shader compiling
	bool shaderCompileInProgress;
	EventMetaData CurrentShaderEvent;
};
