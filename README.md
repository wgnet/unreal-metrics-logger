# Unreal Metrics Logger

## Purpose

The purpose of the MetricsLogger plugin is to capture metrics to assist in correlating machine hardware with its performance in the Unreal Engine.

It gathers metadata about the user’s computer including CPU model/core count/frequency, size of memory, and GPU model and then tracks key events including:

- Compilation initiated from the Unreal editor
- Packaging a project
- Compiling shaders

For each of these events the time the process took is then gathered and then sent, along with the machine metadata, to an external logging service.

At this point in time, the only logger implementation is for InfluxDB, however it has been written in such a way that different logging implementations can be added later.

The plugin has only been developed and tested in Unreal 4.27.


## Plugin Structure

The plugin entry point is in `MetricsLoggerModule` which implements the UnrealModule interface required for Unreal to be able to load the extension.

The `MetricsLoggerModule` then creates an instance of `MetricsEventMonitor`, which subscribes to events from various Unreal internal analytics services. This in turn is initalised with a specific implementation of the `IMetricsLogger` interface - which is a class that logs event metadata. The only implementation at the moment is for logging to InfluxDB (i.e. the class `InfluxDBLogger`).

Configuration for setting up the database connection is displayed to the user via a settings page specified in `MetricsLoggerSettings.`, which stores data in the `DefaultEditor.ini` file of the Unreal project the extension is loaded in to.


## Building

The easiest way to build is to create a new blank unreal project (or add it to an existing project) and then checkout this source code into the the `Plugins` directory of the project. i.e. the directory structure should be:

- Unreal Project Root Directory
  - Plugins
    - MetricsLogger
      - ... contents of this repo

From here you can regenerate the Unreal Project files which should pull the plugin into whatever IDE/build environment you are using. You can then build the plugin from there.

