<a href="https://integration-service.docs.eprosima.com/"><img src="https://github.com/eProsima/Integration-Service/blob/main/docs/images/logo.png?raw=true" hspace="8" vspace="2" height="100" ></a>

# FIWARE System Handle

## Introduction

### What is a System Handle?
[![FIWARE SH CI Status](https://github.com/eProsima/FIWARE-SH/actions/workflows/ci.yml/badge.svg)](https://github.com/eProsima/FIWARE-SH/actions)

A [System Handle](<!--TODO: add link-->) is a plugin that allows a certain middleware
or communication protocol to speak the same language used by the [eProsima Integration Service](https://github.com/eProsima/Integration-Service),
that is, *Extensible and Dynamic Topic Types for DDS* (**xTypes**);
specifically, *Integration Service* bases its intercommunication abilities on eProsima's open source
implementation for the *xTypes* protocol, that is, [eProsima xTypes](https://github.com/eProsima/xtypes).

![System Handle Architecture](docs/images/system-handle-architecture.png)

### The FIWARE SystemHandle

<a href="https://www.fiware.org/"><img src="docs/images/fiware_logo.png" align="left" hspace="8" vspace="2" width="100"></a>

This repository contains the source code of *Integration Service* **System Handle**
for the [FIWARE](https://www.fiware.org/) middleware protocol, widely used in the robotics field.

The main purpose of the *FIWARE System Handle* is that of establishing a connection between a *FIWARE's Context Broker*
and an application running over a different middleware implementation.
This is the classic use-case approach for *Integration Service*.

## Configuration

*Integration Service* is configured by means of a YAML configuration file, which specifies
the middlewares, topics and/or services involved in the intercommunication process, as well as
their topic/service types and the data exchange flow. This configuration file is loaded during
runtime, so there is no need to recompile any package before switching to a whole new
intercommunication architecture.

To get a more precise idea on how these YAML files have to be filled and which fields they require
in order to succesfully configure and launch an *Integration Service* project, please refer to the
dedicated [configuration](<!-- TODO: add link -->) section of the official documentation.
An illustrative explanation is also presented in the *Readme* `Configuration` section of the
[general project repository](https://github.com/eProsima/Integration-Service).

Regarding the *FIWARE System Handle*, there are several specific parameters which must be configured
for the FIWARE middleware. These parameters are mandatory, and fall as suboptions of the main
five sections described in the *Configuration* chapter of *Integration Service* repository:

* `systems`: The system `type` must be `fiware`. In addition to the
  `type` and `types-from` fields, the *FIWARE System Handle* accepts some specific
  configuration fields:

  ```yaml
  systems:
    fiware:
      type: fiware
      host: localhost
      port: 1026
  ```

    * `port`: The specific port where the *FIWARE's Context Broker* will listen for incoming connections. This field is
      required.
    * `host`: The IP address of the *FIWARE's Context Broker`. This field is required.
## Examples

There are several *Integration Service* examples using the *FIWARE System Handle* available
in the project's [main source code repository]([https://](https://github.com/eProsima/Integration-Service/tree/main/examples)).

Some of these examples, where the *FIWARE System Handle* plays a different role in each of them, are introduced here.

### Publisher/subscriber intercommunication between FIWARE and ROS 2

In this example, *Integration Service* uses both this *FIWARE System Handle* and the *ROS 2 System Handle*
to transmit data coming from a ROS 2 publisher into the FIWARE's Context Broker, so that it can be
consumed by FIWARE entities entities named equally to the *Integration Service*'s topic name; and viceversa.

The configuration file used by *Integration Service* for this example can be found
[here](https://github.com/eProsima/Integration-Service/blob/main/examples/basic/ros2_fiware__helloworld.yaml).

For a detailed step by step guide on how to build and test this example, please refer to the
[official documentation](<!-- TODO: link to example -->).
## Compilation flags

Besides the [global compilation flags](<!-- TODO: link to IS readme section-->) available for the
whole *Integration Service* product suite, there are some specific flags which apply only to the
*FIWARE System Handle*; they are listed below:

* `BUILD_FIWARE_TESTS`: Allows to specifically compile the *FIWARE System Handle* unitary and
  integration tests; this is useful to avoid compiling each *System Handle's* test suite present
  in the `colcon` workspace, which is what would happen if using the `BUILD_TESTS` flag; and thus,
  minimizing the building time; to use it, after making sure that the *FIWARE System Handle*
  is present in the `colcon` workspace, the following command must be executed:
  ```bash
  ~/is_ws$ colcon build --cmake-args -DBUILD_FIWARE_TESTS=ON
  ```

<!-- TODO: complete when it is uploaded to read the docs
## API Reference
-->

## License

This repository is open-sourced under the *Apache-2.0* license. See the [LICENSE](LICENSE) file for more details.

## Getting help

If you need support you can reach us by mail at `support@eProsima.com` or by phone at `+34 91 804 34 48`.


<!--
    ROSIN acknowledgement from the ROSIN press kit
    @ https://github.com/rosin-project/press_kit
-->

<a href="http://rosin-project.eu">
  <img src="http://rosin-project.eu/wp-content/uploads/rosin_ack_logo_wide.png"
       alt="rosin_logo" height="60" >
</a>

Supported by ROSIN - ROS-Industrial Quality-Assured Robot Software Components.
More information: <a href="http://rosin-project.eu">rosin-project.eu</a>

<img src="http://rosin-project.eu/wp-content/uploads/rosin_eu_flag.jpg"
     alt="eu_flag" height="45" align="left" >

This project has received funding from the European Union’s Horizon 2020
research and innovation programme under grant agreement no. 732287.
