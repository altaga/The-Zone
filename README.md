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
- [EPIC DEMO:](#epic-demo)

# Introduction:

Help Maintain establishments with the maximum number of people recommended by law.

# Solution:

According to Statista, more than 85% of adults in the United States own a cell phone, thanks to that I develop a system based on the analysis of BT devices within establishments in order to carry out a census of the people who are in the establishment.

https://www.statista.com/statistics/219865/percentage-of-us-adults-who-own-a-smartphone/

Differences with existing technologies:

https://www.rs-online.com/designspark/people-counter-project-for-covid-19

https://www.footfallcam.com/Industries/Covid-19-Automated-Occupancy-Control-System

Current systems use sensors, video cameras and high infrastructure costs to install this type of device to count people.

Our solution offers a plug and play solution that will easily allow the counting of people without large infrastructures with a simple deployment in a web app and notifications.

WebPage: https://d3vv2w0ip0ot9g.cloudfront.net/

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

Primero que nada debes saber que para desarollar en el M5Core2 tenemos 4 opciones ecepcionales para ello, [FreeRTOS](https://aws.amazon.com/freertos/), [MicroPython](http://micropython.org/), [UIFlow](https://flow.m5stack.com/), [Arduino](https://www.arduino.cc/) development frameworks.

* Arduino Library for M5Core2 [Library Link](https://github.com/m5stack/M5Core2)
* Arduino ESP32 Support [Repository Link](https://github.com/espressif/arduino-esp32)

Para este desarrollo se prefirio Arduino IDE debido a la expreiencia de utilizar el ESP32 antes en proyectos de IoT, ya que todos los codigos utilizados para el ESP32 funcionan correctamente en esta board. En la carpeta Arduino Test, dejaremos un codigo sencillo para que puedas familiarizarte con la programacion del M5Core2 en el Arduino IDE.

[Test Code](https://github.com/altaga/The-Zone/tree/main/Arduino%20Test/M5StackTestRTCwWiFiSyncNTP)

Si puedes ver lo siguiente en la pantalla, estamos listos para pasar a programar el codigo principal.

[![Test](./Images/logo.png)](https://youtu.be/tO4BJops1-s)

El [Main Code](https://github.com/altaga/The-Zone/tree/main/Arduino%20Code/TheZone) tiene el codigo completo tal cual se estara probando en esta guia, sin embargo tiene las siguientes partes importantes.

* Toda todas las detecciones de BT paran por este filtro para evitar repetir resultados, para esto utilizamos ArduinoJSON.

      class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
          void onResult(BLEAdvertisedDevice advertisedDevice) {
            doc[getAddress(advertisedDevice.toString().c_str())][0] = advertisedDevice.getRSSI(); //Filter Multiple Detections
          }
      };

* Para obtener los datos de Address de cada device se utilizo la siguiente funcion, para eliminar la data que no nos interesa.

      String getAddress(String AD) {
        int place = AD.indexOf("Address");
        return AD.substring(place + 9, place + 26);
      }

* Para la conectividad con AWS, se configuro un cliente MQTTS con certificados, toda esta informacion se colocara en el archivo [certs.h](https://github.com/altaga/The-Zone/blob/main/Arduino%20Code/TheZone/certs.h), para ver como obtener los certificados ir a [AWS IoT Certs](#aws-iot).
   
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

* Para mandar un String de datos a AWS IoT debemos convertirlo en un char array, por lo tanto debemos convertirlo en uno previo a mandarlo con esta funcion.

      char* string2char(String command) {
        if (command.length() != 0) {
          char *p = const_cast<char*>(command.c_str());
          return p;
        }
      }

# AWS Services:

Si deseas replicar este proyecto correctamente deberas ya tener una cuenta en AWS.

[Create Free Tier Account](https://aws.amazon.com/free)

Como se menciono en la introduccion se utilizaron los siguientes 5 servicios de AWS para el completo funcionamiento de la aplicacion.

1. IoT Core - [Service Link](https://aws.amazon.com/iot-core/)
2. DynamoDb - [Service Link](https://aws.amazon.com/dynamodb/?nc2=type_a)
3. Cognito - [Service Link](https://aws.amazon.com/cognito/)
4. S3 - [Service Link](https://aws.amazon.com/s3/?nc2=type_a)
5. CloudFront - [Service Link](https://aws.amazon.com/cloudfront/?nc2=type_a)

El primer paso antes de empezar a crear tus servicios es configurar AWS en la region que quede mas cerca de ti o el lugar donde se vaya a consumir la aplicacion, con el fin de tener el mejor rendimiento.

<img src="./Images/region.png">

Ya con esto configurado, ya podemos ir a AWS IoT para empezar la configuracion.

## AWS IoT:

Lo primero que debemos hacer es crear las credenciales de acceso de nuestro dispositivo a AWS, entonces iremos a IoT Core y crearemos nuestra thing.

<img src="./Images/create1.png">

Select "Create Single Thing".

<img src="./Images/create2.png">

Para esta configuracion basica, pondremos un nombre identificable a la Thing y dejaremos esta configuracion.

<img src="./Images/create3.png">

Dejaremos que AWS no configure y cree todos los certificados.

<img src="./Images/create4.png">

Si es la primera vez que configuras una Thing de AWS requeriras configurar una Policy, la cual delimite los permisos que tendra la thing para publicar en un topic, para evitar conflictos durante el tutorial, le daremos acceso completo a la thing con la policy Full-IoT.

<img src="./Images/create5.png">

El contenido de esta policy es.

<img src="./Images/create6.png">

AWS va a requerir que descarges todos los certificados para poder continuar, sin embargo los unicos que ocupamos para esta thing seran el Device certificate y el Private key file.

<img src="./Images/create7.png">

El otro dato que ocuparemos para poder utilizar AWS IoT correctamente va a ser el AWS IoT Endpoint, este se encuentra en la pestaña de Settings.

<img src="./Images/create8.png">

NOTE: ya con esta informacion podremos configurar todos los datos de [certs.h](https://github.com/altaga/The-Zone/blob/main/Arduino%20Code/TheZone/certs.h).

### MQTT Test:

Para revisar que tu device este funcionando correctamente, tendremos que abrir el MQTT explorer y subscribirse al topic del codigo.

[![Check](./Images/logo.png)](https://youtu.be/OVFnoyLM7j4)

## AWS DynamoDB:

Ahora para almacenar los datos de las detecciones y poder con eso realizar analiticas de visitas de clientes, etc. Se tiene que crear una DynamoDB.

<img src="./Images/db.png">

En mi caso el partition key lo uso por lo general para almacenar el dato que mejor clasifique la informacion de la database, en nuestro caso sera el topic y como sort key usaremos un timestamp, para que ningun dato se sobre escriba nunca.

<img src="./Images/db1.png">

Una vez creada la database y los datos llegando desde IoT sera hora de mandar datos desde IoT a la DB.

<img src="./Images/db2.png">

## AWS IoT Rule:

Para empezar a llenar esa DB con datos sin necesidad de codigo, vamos a crear una IoT Rule, las cuales son "funciones" especiales que se activan cada vez que un topic recibe informacion de un device.

<img src="./Images/db3.png">

* Selecciona un nombre para tu rule y el Rule query statement ponlo igual al mostrado en la imagen.

<img src="./Images/db4.png">

    SELECT * FROM '/TheZone/M5Core'

* En la seccion inferior presionamos el boton de Add action.

<img src="./Images/db5.png">

* Para este ejemplo seleccionaremos la accion Insert a message into a DynamoDB Table y preisonamos Configure action.

<img src="./Images/db6.png">

* La parte de la configuracion es muy importante ya que esto nos redireccionara nuestros datos a la DB, mi configuracion para hacer funcionar esta accion es la siguiente.

<img src="./Images/db7.png">

Partition key value:

    ${topic()}

Sort key value:

    ${parse_time("yyyy.MM.dd G 'at' HH:mm:ss z", timestamp() )}

NOTA: la funcion en el sortkey, lo que nos dara sera una marca de tiempo unica para cada dato que entre en la DB, esto con el fin de nunca sobre escribir ningun dato y ademas ordenar la DB por fecha.

* Para que nuestra accion pueda acceder correctamente a nuestra DB debe ejecutarse con un role, el cual sera un permiso que le dara AWS a la action para realizar acciones sobre la DB, si no tienes aun tu role configurado crea uno en el boton de Create Role, AWS creara un role configurado unicamente con la capacidad de insertar datos en dynamoDB y pasa al siguiente punto.

<img src="./Images/db8.png">

* Ya configurado esto completamos el proceso de creacion de la rule presionando el boton de Create Rule.

<img src="./Images/db9.png">

* Por ultimo tenemos que asegurarnos que los datos lleguen a la DB correctamente, por lo tanto vamos a realizar una prueba como se muestra en el video.

[![Check](./Images/logo.png)](https://youtu.be/JQBolpWgvrE)

## AWS IoT Web Socket Secure:

Ya que tenemos todo nuestro ecosistema IoT Funcionando correctamente, tenemos que tener una forma de monitorizar en tiempo real los resultados de forma remota, esto lo haremos mediante una WebApp la cual consuma directamente los sevicios de IoT, de forma similar que el monitor MQTT que utilizamos en [MQTT Test](#mqtt-test).

WebPage: https://d3vv2w0ip0ot9g.cloudfront.net/

La pagina web es ya funcional y esta hosteada en AWS, mas detalles en [Hosting](#aws-s3), sin embargo para hacerla funcionar requerimos algunos datos de AWS IoT y Cognito.

Estas credenciales las pondremos en el archivo [aws-configuration.js](https://github.com/altaga/The-Zone/blob/main/WebPage/src/components/aws-configuration.js)

### IoT Credentials:

Aqui solo necesitaremos el AWS Endpoint y el Topic al cual llegaran los mensajes de los devices.

EndPoint:
<img src="./Images/create8.png">
Topic:
/TheZone/M5Core

### Cognito Credentials:

Las credenciales de Cognito es una forma segura de consumir recursos de AWS, en este caso crearemos una credencial de cognito la cual podamos consumir desde nuestra app.

<img src="./Images/cognito.png">

### ReactJS IoT Component:

Si la configuracion del archivo [aws-configuration.js](https://github.com/altaga/The-Zone/blob/main/WebPage/src/components/aws-configuration.js) fue correcta, deberemos ver el siguiente mensaje dentro de nuestra WebApp.

<img src="./Images/webapp.png">

Porfavor revisar el codigo de la webapp para mas detalles.

[WebApp](https://github.com/altaga/The-Zone/tree/main/WebPage/src)

## AWS S3:

Ahora para poder desplegar nuestra pagina web al mundo deberemos hostearla en AWS, por lo tanto usaremos el static website hosting de un bucket S3 para obtener nuestra pagina web.

<img src="./Images/s31.png">

Ahora configurando la opcion de Static Website Hosting obtendremos nuestro link http de nuestra pagina web, sin embargo para una version de produccion esto no es suficiente.

<img src="./Images/s32.png">

## AWS CloudFront:

Para una version de produccion de una aplicacion es siempre necesario utilizar un servicio CDN, el cual sirva para mantener una baja latencia entre usuarios y la app.

<img src="./Images/cloudfront.png">

El link que obtenemos de Cloudfront sera un link https el cual sera el link final de la app.

WebPage: https://d3vv2w0ip0ot9g.cloudfront.net/

# Final Product:

<img src="./Images/device.jpg">

# WebPage:

La pagina web de diseño como una web app para desktop y para mobil.

Desktop:

<img src="./Images/web1.png">

Mobile:

<img src="./Images/web2.png" height="300">

# EPIC DEMO:

[![DEMO](./Images/logo.png)](PENDING...)