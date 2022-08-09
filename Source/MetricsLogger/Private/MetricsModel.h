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

// Enum for specifying the type of event being handled
enum class LogEventTypeEnum {
	UKNOWN,
	BUILD,
	COOK,
	PACKAGE,
	SHADER
};

// Struct for storing metadata about an event
struct EventMetaData {
	LogEventTypeEnum type;
	FDateTime startTime;
	FDateTime finishTime;
	double duration{ 0.0 };
	bool success{ false };
};

namespace MetricsLoggerUtils {

	// Converts an event enum to a string value
	const TCHAR* LogEventTypeToFString(const LogEventTypeEnum type) {
		switch (type) 
		{
			case LogEventTypeEnum::BUILD:
				return TEXT("build_event");
				break;			
			case LogEventTypeEnum::COOK:
				return TEXT("cook_event");
				break;
			case LogEventTypeEnum::PACKAGE:
				return TEXT("package_event");
				break;
			case LogEventTypeEnum::SHADER:
				return TEXT("shader_event");
				break;
			default:
				return TEXT("uknown_event");
		}
	};
}