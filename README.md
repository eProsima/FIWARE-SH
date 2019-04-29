# soss-fiware plugin
System handle to connect fiware to *soss*.

## Installation
1. Create a colcon workspace
2. Clone the soss project into the colcon workspace
3. Clone the soss-fiware plugin into the colcon workspace (with `--recursive` option)
For example:
```
soss_wp
└── src
    ├── osrf
    │   └── soss
    │       └── ... (soss project subfolders)
    └── eprosima
        └── soss
            └── fiware (soss-fiware repo)
```
5. Execute colcon: `colcon build --packages-up-to soss-fiware`
6. source soss environment: `source install/local_setup.bash`

## Run soss (with rso2)
0. Source the ros2 environment and compile with `--packages-up-to soss-ros2-test`
1. Run soss (with the sample configuration): `soss sample/hello_fiware_ros2.yaml`

- For more information, you can see the [demo steps](sample/demo.md),
and the related [video](https://drive.google.com/open?id=1w90DAPkovjwj7673d5RfOINlAAc7kWb1)
- Also, you can have a look to the [internal design](doc/design.md)
