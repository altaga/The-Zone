# The-Zone

<img src="./Images/logo.png">

# Table of Contents:

- [The-Zone](#the-zone)
- [Table of Contents:](#table-of-contents)
- [Introduction:](#introduction)
- [Solution:](#solution)
- [Materials:](#materials)
- [Connection Diagram:](#connection-diagram)
- [M5 Core2 AWS Setup:](#m5-core2-aws-setup)
- [AWS Services:](#aws-services)
  - [AWS IoT:](#aws-iot)
    - [MQTT Test:](#mqtt-test)
  - [AWS DynamoDB:](#aws-dynamodb)
  - [AWS IoT Rule:](#aws-iot-rule)
  - [AWS IoT Web Socket Secure:](#aws-iot-web-socket-secure)
    - [IoT Credentials:](#iot-credentials)
    - [Cognito Credentials:](#cognito-credentials)
    - [ReactJS IoT Component:](#reactjs-iot-component)
  - [AWS S3:](#aws-s3)
  - [AWS CloudFront:](#aws-cloudfront)
- [Final Product:](#final-product)
- [WebPage:](#webpage)
- [Field Test:](#field-test)
- [EPIC DEMO:](#epic-demo)

# Introduction:

According to Statista, more than 85% of adults in the United States own a cell phone, thanks to that I develop a system based on the analysis of BT devices within establishments in order to carry out a census of the people who are in the establishment.

https://www.statista.com/statistics/219865/percentage-of-us-adults-who-own-a-smartphone/

Differences with existing technologies:

https://www.rs-online.com/designspark/people-counter-project-for-covid-19

https://www.footfallcam.com/Industries/Covid-19-Automated-Occupancy-Control-System

Current systems use sensors, video cameras and high infrastructure costs to install this type of device to count people.

Our solution offers a plug and play solution that will easily allow the counting of people without large infrastructures with a simple deployment in a web app and notifications.

WebPage: https://d3vv2w0ip0ot9g.cloudfront.net/

# Solution:

We will build an IoT device to Help Maintain establishments with the maximum number of people recommended by law.

# Materials:

Hardware:

1. M5Stack Core2 ESP32 IoT Development Kit for AWS IoT EduKit - [Product Link](https://shop.m5stack.com/collections/stack-series/products/m5stack-core2-esp32-iot-development-kit-for-aws-iot-edukit)

Software:

1. Arduino IDE - [Program Link](https://www.arduino.cc/en/software)
2. Arduino M5Core2 Library - [Library Link](https://github.com/m5stack/M5Core2)

Cloud Services:

1. IoT Core - [Service Link](https://aws.amazon.com/iot-core/)
2. DynamoDb - [Service Link](https://aws.amazon.com/dynamodb/?nc2=type_a)
3. Cognito - [Service Link](https://aws.amazon.com/cognito/)
4. S3 - [Service Link](https://aws.amazon.com/s3/?nc2=type_a)
5. CloudFront - [Service Link](https://aws.amazon.com/cloudfront/?nc2=type_a)

# Connection Diagram:

<img src="./Images/diagram.png">

# M5 Core2 AWS Setup:

First of all you should know that to develop in the M5Core2 we have 4 exceptional options for it, [FreeRTOS](https://aws.amazon.com/freertos/), [MicroPython](http://micropython.org/), [UIFlow](https://flow.m5stack.com/), [Arduino](https://www.arduino.cc/) development frameworks.

* Arduino Library for M5Core2 [Library Link](https://github.com/m5stack/M5Core2)
* Arduino ESP32 Support [Repository Link](https://github.com/espressif/arduino-esp32)

For this development the Arduino IDE was preferred due to the experience of using the ESP32 before in IoT projects, since all the codes used for the ESP32 work correctly on this board. In the Arduino Test folder, we will leave a simple code so that you can familiarize yourself with the programming of the M5Core2 in the Arduino IDE.

[Test Code](https://github.com/altaga/The-Zone/tree/main/Arduino%20Test/M5StackTestRTCwWiFiSyncNTP)

If you can see the following on the screen, we are ready to move on to programming the main code.

[![Test](./Images/logo.png)](https://youtu.be/tO4BJops1-s)

The [Main Code](https://github.com/altaga/The-Zone/tree/main/Arduino%20Code/TheZone) has the complete code as it will be tested in this guide, however it has the following important parts.

* All BT detections are stopped by this filter to avoid repeating results, for this we use ArduinoJSON.

      class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
          void onResult(BLEAdvertisedDevice advertisedDevice) {
            doc[getAddress(advertisedDevice.toString().c_str())][0] = advertisedDevice.getRSSI(); //Filter Multiple Detections
          }
      };

* To obtain the Address data of each device, the following function was used, to eliminate the data that we are not interested in.

      String getAddress(String AD) {
        int place = AD.indexOf("Address");
        return AD.substring(place + 9, place + 26);
      }

* For connectivity with AWS, an MQTTS client with certificates was configured, all this information will be placed in the file [certs.h](https://github.com/altaga/The-Zone/blob/main/Arduino%20Code/TheZone/certs.h), to see how to obtain the certificates go to [AWS IoT Certs](#aws-iot).
   
  * Edit the certs.h file with your credentials but DO NOT CHANGE THE AWS_CERT_CA [].
  
        static const char* ssid     = "SSID";

        static const char* password = "PASSWORD";

        static const char* AWS_IOT_ENDPOINT = YOURENDPOINT.iot.us-east-2.amazonaws.com";

        // Device Certificate
        static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
        -----BEGIN CERTIFICATE-----

        -----END CERTIFICATE-----
        )KEY";

        // Private Device Certificate
        static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
        -----BEGIN RSA PRIVATE KEY-----

        -----END RSA PRIVATE KEY-----
        )KEY";

* To send a data String to AWS IoT we must convert it into a char array, therefore we must convert it into one prior to sending it with this function.

      char* string2char(String command) {
        if (command.length() != 0) {
          char *p = const_cast<char*>(command.c_str());
          return p;
        }
      }

# AWS Services:

If you want to replicate this project correctly you must already have an AWS account.

[Create Free Tier Account](https://aws.amazon.com/free)

As mentioned in the introduction, the following 5 AWS services were used for the complete operation of the application.

1. IoT Core - [Service Link](https://aws.amazon.com/iot-core/)
2. DynamoDb - [Service Link](https://aws.amazon.com/dynamodb/?nc2=type_a)
3. Cognito - [Service Link](https://aws.amazon.com/cognito/)
4. S3 - [Service Link](https://aws.amazon.com/s3/?nc2=type_a)
5. CloudFront - [Service Link](https://aws.amazon.com/cloudfront/?nc2=type_a)

The first step before starting to create your services is to configure AWS in the region that is closest to you or the place where the application will be consumed, in order to have the best performance.

<img src="./Images/region.png">

With this configured, we can go to AWS IoT to start the setup.

## AWS IoT:

The first thing we must do is create the access credentials of our device to AWS, then we will go to IoT Core and create our thing.

<img src="./Images/create1.png">

Select "Create Single Thing".

<img src="./Images/create2.png">

For this basic configuration, we will put an identifiable name to the Thing and leave this configuration.

<img src="./Images/create3.png">

We will let AWS configure and create all the certificates.

<img src="./Images/create4.png">

If it is the first time that you configure an AWS Thing you will need to configure a Policy, which defines the permissions that the thing will have to publish in a topic, to avoid conflicts during the tutorial, we will give you full access to the thing with the Full policy- IoT.

<img src="./Images/create5.png">

The content of this policy is.

<img src="./Images/create6.png">

AWS will require you to download all the certificates in order to continue, however the only ones we will use for this thing will be the Device certificate and the Private key file.

<img src="./Images/create7.png">

The other data that we will occupy to be able to use AWS IoT correctly is going to be the AWS IoT Endpoint, this is in the Settings tab.

<img src="./Images/create8.png">

NOTE: already with this information we can configure all the data from [certs.h](https://github.com/altaga/The-Zone/blob/main/Arduino%20Code/TheZone/certs.h).

### MQTT Test:

To check that your device is working correctly, we will have to open the MQTT explorer and subscribe to the code topic.

[![Check](./Images/logo.png)](https://youtu.be/OVFnoyLM7j4)

## AWS DynamoDB:

Now to store the data of the detections and thus be able to carry out analytics of customer visits, etc. A DynamoDB has to be created.

<img src="./Images/db.png">

In this case, the partition key is generally used to store the data that best classifies the information in the database, in our case it will be the topic and as a sort key we will use a timestamp, so that no data is ever overwritten.

<img src="./Images/db1.png">

Once the database is created and the data arriving from IoT, it will be time to send data from IoT to the DB.

<img src="./Images/db2.png">

## AWS IoT Rule:

To start filling that DB with data without the need for code, we are going to create an IoT Rule, which are special "functions" that are activated every time a topic receives information from a device.Select a name for your rule and the Rule query statement set it equal to the one shown in the image.

<img src="./Images/db3.png">

* Select a name for your rule and the Rule query statement set it equal to the one shown in the image.

<img src="./Images/db4.png">

    SELECT * FROM '/TheZone/M5Core'

* In the lower section we press the Add action button.

<img src="./Images/db5.png">

* For this example we will select the Insert a message into a DynamoDB Table action and press Configure action.

<img src="./Images/db6.png">

* The configuration part is very important since this will redirect our data to the DB, my configuration to make this action work is the following.

<img src="./Images/db7.png">

Partition key value:

    ${topic()}

Sort key value:

    ${parse_time("yyyy.MM.dd G 'at' HH:mm:ss z", timestamp() )}

NOTA: the function in the sortkey, which will give us a unique time stamp for each data that enters the DB, this in order to never overwrite any data and also order the DB by date.

* For our action to be able to access our DB correctly, it must be executed with a role, which will be a permission that AWS will give the action to perform actions on the DB. IOnce this is configured, we complete the rule creation process by pressing the Create Rule button.f you do not have your role configured yet, create one in the Create Role button , AWS will create a uniquely configured role with the ability to insert data into dynamoDB and go to the next point.

<img src="./Images/db8.png">

* Once this is configured, we complete the rule creation process by pressing the Create Rule button.

<img src="./Images/db9.png">

* Finally we have to make sure that the data reaches the DB correctly, therefore we are going to carry out a test as shown in the video.

[![Check](./Images/logo.png)](https://youtu.be/JQBolpWgvrE)

## AWS IoT Web Socket Secure:

Since we have our entire IoT ecosystem working correctly, we have to have a way to remotely monitor the results in real time, we will do this through a WebApp which directly consumes the IoT services, in a similar way to the MQTT monitor that we use on [MQTT Test](#mqtt-test).

WebPage: https://d3vv2w0ip0ot9g.cloudfront.net/

The website is already functional and is hosted in AWS, more details in [Hosting](#aws-s3), however to make it work we require some data from AWS IoT and Cognito.

We will put these credentials in the file [aws-configuration.js](https://github.com/altaga/The-Zone/blob/main/WebPage/src/components/aws-configuration.js)

### IoT Credentials:

Here we will only need the AWS Endpoint and the Topic to which the messages from the devices will arrive.

EndPoint:
<img src="./Images/create8.png">
Topic:
/TheZone/M5Core

### Cognito Credentials:

Cognito credentials is a safe way to consume AWS resources, in this case we will create a cognito credential which we can consume from our app.

<img src="./Images/cognito.png">

### ReactJS IoT Component:

If the configuration from the [aws-configuration.js](https://github.com/altaga/The-Zone/blob/main/WebPage/src/components/aws-configuration.js)  file was done correctly, we should see the following message within our WebApp.

<img src="./Images/webapp.png">

Please check the webapp code for more details.

[WebApp](https://github.com/altaga/The-Zone/tree/main/WebPage/src)

## AWS S3:

Now to be able to deploy our web page to the world we will have to host it in AWS, therefore we will use the static website hosting of an S3 bucket to obtain our web page.

<img src="./Images/s31.png">

Now configuring the Static Website Hosting option we will obtain our http link from our website, however for a production version this is not enough.

<img src="./Images/s32.png">

## AWS CloudFront:

For a production version of an application it is always necessary to use a CDN service, which serves to maintain low latency between users and the app.

<img src="./Images/cloudfront.png">

The link we get from Cloudfront will be an https link which will be the final link of the app.

WebPage: https://d3vv2w0ip0ot9g.cloudfront.net/

# Final Product:

<img src="./Images/device.jpg">

# WebPage:

The design website as a web app for desktop and mobile.

Desktop:

<img src="./Images/web1.png">

Mobile:

<img src="./Images/web2.png" height="300">

# Field Test:

[![DEMO](./Images/logo.png)](https://youtu.be/tTnYknAwxg0)

# EPIC DEMO:

[![DEMO](./Images/logo.png)](https://youtu.be/75XoH2JduJY)
