# soss-fiware

System handle to connect [*SOSS*][soss] to [*FIWARE*][fiware]

## Installation
To install this package into a workspace already containing SOSS, just clone this repository into the sources directory and build it:
```
$ cd <soss workspace folder>
$ git clone git@github.com:eProsima/SOSS-FIWARE.git src/soss-fiware
$ colcon build --packages-up-to soss-fiware
```
## Use case - Connecting with ROS2

0. Prerequisites: curlpp and asio installed
1. [Create a colcon workspace](https://index.ros.org/doc/ros2/Tutorials/Colcon-Tutorial/#create-a-workspace).
    ```
    mkdir -p soss_wp/src
    cd soss_wp
    ```
2. Clone the soss project into the source subfolder.
    ```
    git clone git@github.com:osrf/soss_v2.git src/soss
    ```
3. Clone this project into the subfolder.
    ```
    git clone git@github.com:eProsima/SOSS-FIWARE.git src/soss-fiware
    ```

    The workspace layout should look like this:
    ```
        soss_wp
        └── src
            ├── soss
            |   └── ... (other soss project subfolders)
            │   └── packages
            │       └── soss-ros2 (ROS2 system handle)
            └── soss-fiware (repo)
                    ├── fiware (soss-fiware colcon pkg)
                    └── fiware-test (soss-fiware-test colcon pkg)
    ```

5. In the workspace folder, execute colcon: 
    ```
    colcon build --packages-up-to soss-fiware
    ```
6. Source the current environment:
    ```
    source install/local_setup.bash
    ```

## Usage

This system handle can be used to connect FIWARE with other systems.

### Configuration

SOSS must be configured with a YAML file, which tells the program everything it needs to know in order to establish the connection between two or more systems that the user wants. 
For example, if a simple string message wants to be exchanged between FIWARE and ROS2, the configuration file for SOSS should look as follows.

```YAML
systems:
    ros2: { type: ros2 }
    fiware: { type: fiware, host: "192.168.1.59", port: 1026}

routes:
    fiware_to_ros2: { from: fiware, to: ros2 }
    ros2_to_fiware: { from: ros2, to: fiware }

topics:
    hello_fiware: { type: "std_msgs/String", route: ros2_to_fiware }
    hello_ros2: { type: "std_msgs/String", route: fiware_to_ros2 }
```

To see how general SOSS systems, users and topics are configured, please refer to [SOSS' documentation][soss].

For the FIWARE system handle, the user must give two extra YAML key-value pairs which are the host and port in which this system handle will try to connect to an instance of FIWARE's Orion context broker.

FIWARE does not allow certain characters in its entities names. For this reason, if a type defined in the topics section of the configuration file has in its name a `/`, the FIWARE system handle will map that character into two underscores. This is something important to notice when connecting to ROS2, because in ROS2 most of the types have a `/` in their names. Also, notice that in FIWARE the type will be published as an entity with the same name but with every slash substituted with two underscores. 

With that, in the YAML file the type under the `topics` section can have a `/` (and to connect to ROS2, normally it will HAVE to), just remember that in FIWARE the entity created must have the same name but with two underscores instead of a slash.

Notice that this system handle maps soss messages dyrectly to a JSON compatible with FIWARE. As FIWARE doesn't allow nested types, neither does this system handle.

### More information

- For more information, you can see the [demo steps](fiware/doc/demo.md),
and the related [video](https://drive.google.com/open?id=1w90DAPkovjwj7673d5RfOINlAAc7kWb1)
- Also, you can have a look to the [internal design](fiware/doc/design.md)
- For a fast configuration, you can use the [dockerfile](Dockerfile). 
**NOTICE**: Both when building and running the Dockerfile, you must specify the option `--network=host`, 
because FIWARE SH tells Orion context broker its own IP in order to make subscriptions,
and for it to work the docker IP must be the same as the host IP.

### Changelog

#### v0.2.0
- Added dockerfiles
- Added integration tests
- Removed asio as local thirdparty
- Subscription host and port automatically generated.

#### v0.1.1
- Fiware communication take into account the topic type
- Added thread protection

#### v0.1.0
- Fiware communication in both directions based on topic

---

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

 [soss]: https://github.com/osrf/soss
 [fiware]: https://www.fiware.org/
