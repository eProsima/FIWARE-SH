# soss-fiware plugin
System handle to connect fiware to *soss*.

## Installation
0. Prerequisites: curlpp and asio installed
1. Create a colcon workspace
2. Clone the soss project into the colcon workspace (tag: `eprosima_usage`)
3. Clone the soss-fiware plugin into the colcon workspace.
For example:
```
soss_wp
└── src
    ├── osrf
    │   └── soss
    │       └── ... (soss project subfolders)
    └── eprosima
        └── soss
            └── fiware (repo)
                ├── fiware (soss-fiware colcon pkg)
                └── fiware-test (soss-fiware-test colcon pkg)
```
4. Execute colcon: `colcon build --packages-up-to soss-fiware`
5. source soss environment: `source install/local_setup.bash`

## Run soss (with ros2)
0. Source the ros2 environment and compile with `--packages-up-to soss-ros2-test`
1. Run soss (with the sample configuration): `soss sample/hello_fiware_ros2.yaml`

- For more information, you can see the [demo steps](fiware/sample/demo.md),
and the related [video](https://drive.google.com/open?id=1w90DAPkovjwj7673d5RfOINlAAc7kWb1)
- Also, you can have a look to the [internal design](fiware/doc/design.md)
- For a fast configuration, you can use the [dockerfiles](fiware/dockerfiles)

## Changelog
### v0.1.2
- Added dockerfiles
- Added integration tests
- Removed asio as local thirdparty
### v0.1.1
- Fiware communication take into account the topic type
- Added thread protection
### v0.1.0
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
