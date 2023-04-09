import Delay from './Delay';
import {i18n} from "../i18n/i18n";
import { namespaces } from "../i18n/i18n.constants";
import ConnectionManager from '../Protocol/RestApi'
import { IValueReference } from './ValueModel';
const connection = new ConnectionManager();

const t = i18n.getFixedT(null, namespaces.settings);

export interface IMQTTSettingsValues {
    enableMQTT : boolean
    frequency : number;
    serverAddress : string;
    topicPath : string;
    availableValues : IValueReference[];
    readingsTopic : IValueReference[];
    infoTopic : IValueReference[];
}

interface MQTTFrequency {
    seconds: number;
    label: string;
 }
 
const frequencies : MQTTFrequency[] = [
    {
       seconds: 60,
       label: t("frequencies.minute")
    },
    {
      seconds: 60*5,
      label: t("frequencies.fiveMinutes")
    },
    {
      seconds: 60*10,
      label: t("frequencies.tenMinutes")
    },
    {
      seconds: 60*15,
      label: t("frequencies.fifteenMinutes")
    },
    {
      seconds: 60*30,
      label: t("frequencies.thirtyMinutes")
    },
    {
      seconds: 60*60,
      label: t("frequencies.hour")
    },  
  ]


class CloudSettingsView {
    private _mqttValues : IMQTTSettingsValues = { 
        enableMQTT: false,
        frequency: 0,
        serverAddress: "",
        topicPath: "CO2Monitors/%DEVICE_NAME%",
        availableValues: [],
        readingsTopic: [],
        infoTopic: []
    };    

    private getFrequencyIndex(pSec : number) {
        var index = 0;

        for(var i = 1; i < frequencies.length;i++)
            if(Math.abs(frequencies[index].seconds-pSec) > Math.abs(frequencies[i].seconds-pSec) )
                index = i;
        return index;
    }

    public async LoadMqtt() {
        var result = await connection.executeCommand("MQTT", pCmd=> {
           pCmd.Mode = "GetValues";
        });

        this._mqttValues = {
            enableMQTT: result.EnableMqtt === "true",
            frequency: this.getFrequencyIndex(result.MqttPublishDelay),
            serverAddress: result.MqttServerAddress,
            topicPath: result.MqttTopic,
            availableValues: result.Available,
            readingsTopic: result.ReadingsTopic,
            infoTopic: result.InfoTopic
        };
      
        return structuredClone(this._mqttValues);
    }

    public async SaveMqtt(pValues : IMQTTSettingsValues) {
        await connection.executeCommand("MQTT", pCmd=>{
          pCmd.Mode = "SetValues"
            pCmd.EnableMqtt = pValues.enableMQTT.toString();
            pCmd.MqttPublishDelay = frequencies[pValues.frequency].seconds.toString();
            pCmd.MqttServerAddress = pValues.serverAddress;
            pCmd.MqttTopic = pValues.topicPath;          
            pCmd.ReadingsTopic = pValues.readingsTopic;
            pCmd.InfoTopic = pValues.infoTopic;
        });
        this._mqttValues = pValues;
    }

    public GetNumberFrequencies() {
        return frequencies.length;
    }

    public FormatFrequency(pFrequency : number) {
        return frequencies[pFrequency].label;
    }
}

export default CloudSettingsView;