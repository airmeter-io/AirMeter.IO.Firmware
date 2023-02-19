import Delay from './Delay';
import {i18n} from "../i18n/i18n";
import { namespaces } from "../i18n/i18n.constants";

const t = i18n.getFixedT(null, namespaces.settings);

export interface ICalibrationSettingsValues {
  enableAbc : boolean;
  abcFrequency : number;
  minAbcFrequency : number;
  maxAbcFrequency : number;
  stepAbcFrequency : number;
  baseline : number;
  minBaseline : number;
  maxBaseline : number;
  stepBaseline : number;
}

export interface ICalibrationDisplayValues {
  co2 : number;
  waitSeconds: number;
}


export interface ICalibrationResult {
   success : boolean;
   message : string | undefined;
}


class CalibrationSettingsView {
    private _values : ICalibrationSettingsValues = { 
        enableAbc: true,
        abcFrequency: 24,
        minAbcFrequency: 1,
        maxAbcFrequency: 240,
        stepAbcFrequency: 1,
        baseline: 400,
        minBaseline: 400,
        maxBaseline: 500,
        stepBaseline: 5
    };    
    private _calibWaitTime : Date = new Date();

    public async Load() {
        await Delay(600);
        return structuredClone(this._values);
    }

    public async Save(pValues : ICalibrationSettingsValues) {
        await Delay(1000);
        this._values = pValues;
    }

    public async Calibrate() : Promise<ICalibrationResult> {
      await Delay(1000); 
      return {
        success: true,
        message: "shit happend"
      };
    }

    public async GetDisplayValues() : Promise<ICalibrationDisplayValues> {
      await Delay(200);
      return {
        co2: 403,
        waitSeconds: 120
      };
    }

    public FormatFrequency(pFreq : number) {
       var days = Math.floor(pFreq/24);
       var hours = pFreq - (days*24);
       if(days>0 && hours > 0) 
         return t("calibration.options.frequencies.daysHours", { days: days, hours: hours });
       if(days>0)
        return t("calibration.options.frequencies.days", { days: days });
       
      return t("calibration.options.frequencies.hours", { hours: hours });

    }

    public ResetWait() {
      this._calibWaitTime = new Date();
      
    }

    public GetRemainingSeconds() {
      var elapsed = Math.floor((new Date().getTime() - this._calibWaitTime.getTime())/1000);
      console.log("elapsed "+ elapsed + " - start: "+ this._calibWaitTime);
      var remaining =120 - elapsed;
      if(remaining < 0) remaining =  0;
      return remaining;
    }
}

export default CalibrationSettingsView;