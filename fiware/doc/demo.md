# Fiware system handle demo

[This link](https://drive.google.com/open?id=1w90DAPkovjwj7673d5RfOINlAAc7kWb1) contains a demonstration on how FIWARE can exchange messages with ROS2 using SOSS through `soss-fiware` and `soss-ros2` system handles.

### 0. Terminals layout
1. [console 1] fiware client. The fiware context broker is already open at `192.168.1.59:1026`.
1. [console 2] soss (with soss environment configured)
1. [console 3] ros2 publiher (with ros2 environment configured)
1. [console 4] ros2 subscriber (with ros2 environment configured)

### 1. Create fiware entities
1. Create entity `hello_ros2` in fiware with type `std_msgs__String`.
```
curl 192.168.1.59:1026/v2/entities -s -S -H 'Content-Type: application/json' -d @- <<EOF
{
  "id": "hello_ros2",
  "type": "std_msgs__String",
  "data": {
    "value": "Hello, ros2",
    "type": "String"
  }
}
EOF
```
2. Create entity `hello_fiware` in fiware with type `std_msgs__String`.
```
curl 192.168.1.59:1026/v2/entities -s -S -H 'Content-Type: application/json' -d @- <<EOF
{
  "id": "hello_fiware",
  "type": "std_msgs__String",
  "data": {
    "value": "",
    "type": "String"
  }
}
EOF
```
3. See entities.
```
curl 192.168.1.59:1026/v2/entities?type=std_msgs__String -s -S -H 'Accept: application/json' | python -mjson.tool
```

### 2. Create Ros2 subscriber
1. Run a ros2 subscriber to topic `hello_ros2` with type `std_msgs/String`.
```
ros2 topic echo /hello_ros2 std_msgs/String
```

### 3. Run SOSS [Console 2]
1. See yaml file for configuration.
```
vi path/to/fiware_SH/fiware/sample/fiware_ros2.yaml
```
1. Run soss
```
soss path/to/fiware_SH/fiware/sample/fiware_ros2.yaml
```
1. New topic arrives to ros2 because soss has subscribed to `hello_ros2` topic.

### 4. Send topics from fiware to ros2
1. Update the `hello_ros2` entity value.
```
curl 192.168.1.59:1026/v2/entities/hello_ros2/attrs?type=std_msgs__String -s -S -H 'Content-Type: application/json' -X PUT -d @- <<EOF
{
  "data": {
    "value": "Hello, ros2 again",
    "type": "String"
  }
}
EOF
```
1. The updated topic information arrives to ros2

### 5. Send topics from ros2 to fiware
1. See current topic value for `hello_fiware`.
```
curl 192.168.1.59:1026/v2/entities/hello_fiware?type=std_msgs__String -s -S -H 'Accept: application/json' | python -mjson.tool
```
1. Run a ros2 publisher for sending `hello_fiware` topics with type `std_msgs/String`.
```
ros2 topic pub --once /hello_fiware std_msgs/String "{data: \"Hello, fiware\"}"
```
1. Current topic value for `hello_fiware` updated.
```
curl 192.168.1.59:1026/v2/entities/hello_fiware?type=std_msgs__String -s -S -H 'Accept: application/json' | python -mjson.tool
```

---

## Annex: *curl* commands to manage *fiware*
The following are the *curl* commands most used to manage the *fiware context broker* from the console.
Modify the `ip:port` from the init of the *curl* command where the *fiware context broker* is running.

### POST
- Create entity
```
curl 192.168.1.59:1026/v2/entities -s -S -H 'Content-Type: application/json' -d @- <<EOF
{
  "id": "hello_ros2",
  "type": "std_msgs__String",
  "data": {
    "value": "Hello, ros2",
    "type": "String"
  }
}
EOF
```

- Create subscription
```
curl 192.168.1.59:1026/v2/subscriptions -s -S -H 'Content-Type: application/json' -d @- <<EOF
{
  "subject": {
    "entities": [
      {
        "id": "hello_ros2"
      }
    ]
  },
  "notification": {
    "http": {
      "url": "http://192.168.1.56:1028"
    }
  }
}
EOF
```

### PUT

- Update an entity
```
curl 192.168.1.59:1026/v2/entities/hello_ros2/attrs?type=std_msgs__String -s -S -H 'Content-Type: application/json' -X PUT -d @- <<EOF
{
  "data": {
    "value": "Hello, ros2",
    "type": "String"
  }
}
EOF
```

### GET

- Show entity
```
curl 192.168.1.59:1026/v2/entities/hello_ros2 -s -S -H 'Accept: application/json' | python -mjson.tool
```

- Show entity (specifying the type)
```
curl 192.168.1.59:1026/v2/entities/hello_ros2?type=std_msgsString -s -S -H 'Accept: application/json' | python -mjson.tool
```

- List all entities
```
curl 192.168.1.59:1026/v2/entities -s -S -H 'Accept: application/json' | python -mjson.tool
```

- List all subscriptions
```
curl 192.168.1.59:1026/v2/subscriptions -s -S -H 'Accept: application/json' | python -mjson.tool
```

### DELETE

- Remove entity

```
curl 192.168.1.59:1026/v2/entities/hello_ros2 -s -S -H 'Accept: application/json' -X DELETE | python -mjson.tool
```

- Remove entity (specifying the type)
```
curl 192.168.1.59:1026/v2/entities/hello_ros2?type=std_msgsString -s -S -H 'Accept: application/json' -X DELETE | python -mjson.tool
```

- Remove subscription
```
curl 192.168.1.59:1026/v2/subscriptions/5cc3f909efccc70b574a4611 -s -S -H 'Accept: application/json' -X DELETE
```

For removing all subscriptions, please, use the script found in fiware/utils called `erase_subscriptions.sh`
