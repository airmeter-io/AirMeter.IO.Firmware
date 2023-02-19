import Delay from './Delay';
import {i18n} from "../i18n/i18n";
import { namespaces } from "../i18n/i18n.constants";
import ConnectionManager from '../Protocol/RestApi'

const connection = new ConnectionManager();

const t = i18n.getFixedT(null, namespaces.settings);

export interface IMQTTSettingsValues {
    frequency : number;
}

interface MQTTFrequency {
    seconds: number;
    label: string;
 }
 
const frequencies : MQTTFrequency[] = [
  {
    seconds: 30,
    label: t("frequencies.thirtySeconds")
  },    
  {
     seconds: 60,
     label: t("frequencies.minute")
  },
  {
    seconds: 60*2,
    label: t("frequencies.twoMinutes")
  },
  {
    seconds: 60*3,
    label: t("frequencies.threeMinutes")
  },
  {
    seconds: 60*4,
    label: t("frequencies.fourMinutes")
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


class DataSettingsView {
    private _values : IMQTTSettingsValues = { 
        frequency: 0,
    };    

    private getFrequencyIndex(pSec : number) {
      var index = 0;

      for(var i = 1; i < frequencies.length;i++)
          if(Math.abs(frequencies[index].seconds-pSec) > Math.abs(frequencies[i].seconds-pSec) )
              index = i;
      return index;
    }

    public async Load() {
      var result = await connection.executeCommand("LOADSETTINGS");

      this._values = {
          frequency: this.getFrequencyIndex(result.SensorUpdateInterval),
      };
    
      return structuredClone(this._values);
    }

    public async Save(pValues : IMQTTSettingsValues) {
      await connection.executeCommand("SAVESETTINGS", pCmd=>{
        pCmd.SensorUpdateInterval = frequencies[pValues.frequency].seconds.toString();
      });
      this._values = pValues;
    }

    public GetNumberFrequencies() {
        return frequencies.length;
    }

    public FormatFrequency(pFrequency : number) {
        return frequencies[pFrequency].label;
    }
}

export default DataSettingsView;