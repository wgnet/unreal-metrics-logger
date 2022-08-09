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

#include "IMetricsLogger.h"

// Metadata Sources
#include "GeneralProjectSettings.h"
#include "HAL/PlatformProcess.h"
#include "Misc/EngineVersion.h"
#include "Misc/App.h"
#include "PluginDescriptor.h"
#include "Interfaces/IPluginManager.h"

/**
 * Base class for defining a class that can log metric information.
 */
IMetricsLogger::IMetricsLogger() 
{
	// Grab Static Metadata Once
	ExtensionVersion = FString("unkown");
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("MetricsLogger");
	if (Plugin.IsValid()) {
		const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
		ExtensionVersion = Descriptor.VersionName;
	}
	ProjectName = FApp::GetProjectName();
	CoreCount = FString::FromInt(FPlatformMisc::NumberOfCores());
	CpuModel = FPlatformMisc::GetCPUBrand();
	RamSize = FString::Printf(TEXT("%llu"), FPlatformMemory::GetStats().TotalPhysical); // Use FString::Printf to format into Bytes
	GpuModel = FPlatformMisc::GetPrimaryGPUBrand();
	Username = FPlatformProcess::UserName(false);
	Username = Username.Replace(TEXT(" "), TEXT("\\ "));
	MachineName = FPlatformProcess::ComputerName();
	UnrealVersion = FEngineVersion::Current().ToString();
}
