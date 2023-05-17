import Delay from './Delay';
import {i18n} from "../i18n/i18n";
import { namespaces } from "../i18n/i18n.constants";
import ConnectionManager from '../Protocol/RestApi'
import { IValueReference } from './ValueModel';
const connection = new ConnectionManager();

const t = i18n.getFixedT(null, namespaces.settings);

export interface IDataSettingsValues {
    frequency : number;
    availableValues : IValueReference[];
    values : IValueReference[];    
}

interface DataFrequency {
    seconds: number;
    label: string;
}

export interface IFlashBucketInfo {
  offset : number;
  index : number;
  blockStartTime : Date | undefined;
  blockEndTime : Date | undefined;
  dataLength : number;
  payloadOffset : number | undefined;
  numReadings : number | undefined;
  currentTime : Date | undefined;
}
 
const frequencies : DataFrequency[] = [
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
    private _values : IDataSettingsValues = { 
        frequency: 0,
        availableValues: [],
        values: []
    };    

    private getFrequencyIndex(pSec : number) {
      var index = 0;

      for(var i = 1; i < frequencies.length;i++)
          if(Math.abs(frequencies[index].seconds-pSec) > Math.abs(frequencies[i].seconds-pSec) )
              index = i;
      return index;
    }

    public async Load() {
      var result = await connection.executeCommand("DATA", pCmd=>{
        pCmd.Mode = "GetValues";
      });

      this._values = {
          frequency: this.getFrequencyIndex(result.SensorUpdateInterval),
          availableValues: result.Available,
          values: result.Values
      };
    
      return structuredClone(this._values);
    }

    public async Save(pValues : IDataSettingsValues) {
      await connection.executeCommand("DATA", pCmd=>{
        pCmd.Mode = "SetValues";
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

    public async GetFlashBucketInfos() {
      var result = await connection.executeCommand("DATA", pCmd=>{
        pCmd.Mode = "GetFlashInfo";
      });
      var infos : IFlashBucketInfo[] = [];
      for(var i = 0; i < result.Buckets.length; i++) {
        infos.push({
          offset: result.Buckets[i].Offset,
          index: result.Buckets[i].Index,
          blockStartTime: result.Buckets[i].BlockStartTime === undefined ? undefined : new Date(result.Buckets[i].BlockStartTime*1000),
          blockEndTime: result.Buckets[i].BlockEndTime === undefined ? undefined : new Date(result.Buckets[i].BlockEndTime*1000),
          dataLength: result.Buckets[i].DataLength,
          payloadOffset: result.Buckets[i].PayloadOffset === undefined ? undefined : result.Buckets[i].PayloadOffset,
          numReadings: result.Buckets[i].NumReadings === undefined ? undefined : result.Buckets[i].NumReadings,
          currentTime: result.Buckets[i].CurrentTime === undefined ? undefined : new Date(result.Buckets[i].CurrentTime*1000),
        });
      }
    
      return infos;
    }
}

export default DataSettingsView;