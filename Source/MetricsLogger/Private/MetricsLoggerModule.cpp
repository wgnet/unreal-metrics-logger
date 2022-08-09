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

#include "MetricsLoggerModule.h"

#include "MetricsLoggerEventMonitor.h"
#include "IMetricsLogger.h"

// Declare UE Log Category for this module to use
#include "MetricsLogCategory.h"

// Analytics Sources
#include "EngineAnalytics.h"
#include "Misc/EngineVersion.h"
#include "EngineGlobals.h"
#include "RHI.h"

// Settings
#include "MetricsLoggerSettings.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

// Utilities
#include "Misc/DateTime.h"

#include "InfluxDBLogger.h"

#define LOCTEXT_NAMESPACE "FMetricsLoggerModule"

DEFINE_LOG_CATEGORY(MetricsLog);

void FMetricsLoggerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	RegisterSettings();
	RegisterEventMonitor();
}

void FMetricsLoggerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UnregisterSettings();

	UnRegisterEventMonitor();
}

void FMetricsLoggerModule::RegisterEventMonitor()
{
	// If needed, could instantiate different loggers based on some settings option in future
	MetricsLogger = MakeUnique<FInfluxDBLogger>();

	EventMonitor = MakeUnique<FMetricsLoggerEventMonitor>(*MetricsLogger.Get());

	// Register the event monitor with analytics events
	IAnalyticsProviderET::OnEventRecorded engineCallback = [this](const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attrs, bool bJson) {
		EventMonitor->ProcessEvent(EventName, Attrs, bJson);
	};
	if (FEngineAnalytics::IsAvailable()) {
		FEngineAnalytics::GetProvider().SetEventCallback(engineCallback);
		UE_LOG(MetricsLog, Log, TEXT("Plugin Initialised!"));
	}
}

void FMetricsLoggerModule::UnRegisterEventMonitor()
{
	MetricsLogger.Reset();
	EventMonitor.Reset();
}

void FMetricsLoggerModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Get the Editor Settings module
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Editor");

		if (SettingsContainer.IsValid())
		{
			// Create settings category
			SettingsContainer->DescribeCategory("MetricsMontior",
				LOCTEXT("EditorMetricsMonitorCategoryName", "MetricsMontior"),
				LOCTEXT("EditorMetricsMonitorDescription", "Configuration for the metrics monitor plugin"));

			// Register the settings page
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Editor", "MetricsMontior", "Config",
				LOCTEXT("EditorMetricsMonitorConfigSettingsName", "Config"),
				LOCTEXT("EditorMetricsMonitorConfigSettingsDescription", "Base configuration for our game module"),
				GetMutableDefault<UMetricsLoggerSettings>()
			);

			// Register the save handler to your settings
			if (SettingsSection.IsValid())
			{
				SettingsSection->OnModified().BindRaw(this, &FMetricsLoggerModule::SaveSettings);
			}

		}
	}
}

void FMetricsLoggerModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "MetricsMontior", "General");
	}
}

bool FMetricsLoggerModule::SaveSettings()
{
	UMetricsLoggerSettings* Settings = GetMutableDefault<UMetricsLoggerSettings>();
	Settings->SaveConfig();
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMetricsLoggerModule, MetricsLogger)