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

#include "MetricsLogCategory.h"
#include "MetricsModel.h"

/**
 * Base class for defining a class that can log metric information.
 */
class IMetricsLogger
{
public:
	IMetricsLogger();
	virtual ~IMetricsLogger() {}

	virtual void Log(const EventMetaData& data) = 0;

protected:

	// Meta Data
	FString ExtensionVersion;
	FString ProjectName;
	FString UnrealVersion;
	FString CpuModel;
	FString CoreCount;
	FString RamSize;
	FString GpuModel;
	FString Username;
	FString MachineName;
};
