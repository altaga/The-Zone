import React, { Component } from 'react';

class IotReciever extends Component {

    constructor(props) {
        super(props);
        this.mqttClient = ""
    }

    componentDidMount() {
        let sub_topics = this.props.sub_topics
        let _this = this

        var AWS = require('aws-sdk');
        var AWSIoTData = require('aws-iot-device-sdk');
        var AWSConfiguration = require('./aws-configuration.js');

        AWS.config.region = AWSConfiguration.region;
        AWS.config.credentials = new AWS.CognitoIdentityCredentials({ IdentityPoolId: AWSConfiguration.poolId });
        var cognitoIdentity = new AWS.CognitoIdentity();
        AWS.config.credentials.get(function (err, data) {
            if (!err) {

                var params = { IdentityId: AWS.config.credentials.identityId };
                cognitoIdentity.getCredentialsForIdentity(params, function (err, data) {
                    if (!err) { _this.mqttClient.updateWebSocketCredentials(data.Credentials.AccessKeyId, data.Credentials.SecretKey, data.Credentials.SessionToken); }
                    else {
                        console.log('error retrieving credentials: ');
                        alert('error retrieving credentials: ');
                    }
                });
            }
            else { console.log('error retrieving identity:'); }
        });

        var messageHistory = '';
        var refresh = 0;
        var clientId = 'mqtt-explorer-' + (Math.floor((Math.random() * 100000) + 1));

        this.mqttClient = AWSIoTData.device({
            region: AWS.config.region,
            host: AWSConfiguration.host,
            clientId: clientId,
            protocol: 'wss',
            maximumReconnectTimeMs: 1000,
            debug: true,
            accessKeyId: '',
            secretKey: '',
            sessionToken: ''
        });

        window.mqttClientConnectHandler = function () {
            console.clear();
            console.log("Connected")
            console.log(sub_topics)
            for (let i = 0; i < sub_topics.length; i++) {
                console.log("Sub:" + sub_topics[i])
                _this.mqttClient.subscribe(sub_topics[i]);
            }
            messageHistory = '';
        }

        window.mqttClientReconnectHandler = function () {
            console.log('reconnect : times : ' + refresh.toString());
        };

        window.mqttClientMessageHandler = function (topic, payload) {
            for (let i = 0; i < sub_topics.length; i++) {
                if (topic === sub_topics[i]) {
                    messageHistory = payload.toString()
                    _this.props.callback([topic, messageHistory])
                }
            }
            messageHistory = "";
        }

        window.updateSubscriptionTopic = function () {

        };

        window.clearHistory = function () {
            if (1 === true) {
                messageHistory = '';
            }
        };

        window.updatePublishTopic = function () { };

        window.updatePublishData = function () {

        };

        this.mqttClient.on('connect', window.mqttClientConnectHandler);
        this.mqttClient.on('reconnect', window.mqttClientReconnectHandler);
        this.mqttClient.on('message', window.mqttClientMessageHandler);
        this.mqttClient.on('close', function () {
            console.log('close');
        });
        this.mqttClient.on('offline', function () {
            console.log('offline');
        });
        this.mqttClient.on('error', function (error) {
            //console.log('error', error);
        });
    }

    componentWillUnmount() {
        this.mqttClient.end();
    }

    render() {

        return (
            <>
            </>
        );
    }
}


export default IotReciever;