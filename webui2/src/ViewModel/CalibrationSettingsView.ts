import Delay from './Delay';
import {i18n} from "../i18n/i18n";
import { namespaces } from "../i18n/i18n.constants";
import ConnectionManager from '../Protocol/RestApi'


const connection = new ConnectionManager();


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

    private _displayValues : ICalibrationDisplayValues = {
      co2 : 400,
      waitSeconds: 60
    };

    private _calibWaitTime : Date = new Date();
    



    public async Load() {
        var result = await connection.executeCommand("CALIBRATE", pCmd=>pCmd.Mode = "Info");

        this._values = {
          enableAbc: result.Co2.IsABCEnabled ==="true",
          abcFrequency: parseInt(result.Co2.ABCHoursPerCycle),
          minAbcFrequency: parseInt(result.Co2.ABCMinHoursPerCycle),
          maxAbcFrequency: parseInt(result.Co2.ABCMaxHoursPerCycle),
          stepAbcFrequency: parseInt(result.Co2.ABCStepHoursPerCycle),
          baseline: parseInt(result.BackgroundCO2),
          minBaseline: parseInt(result.Co2.MinBasePPM),
          maxBaseline: parseInt(result.Co2.MaxBasePPM),
          stepBaseline: 5
        };
                
        return structuredClone(this._values);
    }



    public async Save(pValues : ICalibrationSettingsValues) {
        var result = await connection.executeCommand("CALIBRATE", pCmd=>{
          pCmd.Mode = "SetOptions";       
          pCmd.EnableABC = pValues.enableAbc.toString();
          pCmd.ABCHoursPerCycle = pValues.abcFrequency.toString();
          pCmd.Baseline = pValues.baseline.toString();
        });

        this._values = pValues;
    }

    public async Calibrate() : Promise<ICalibrationResult> {
      var result = await connection.executeCommand("CALIBRATE", pCmd=>pCmd.Mode = "Perform");     
      return {
        success: result.Status==="true",
        message: ""
      };
    }

    public async GetDisplayValues() : Promise<ICalibrationDisplayValues> {
      var result = await connection.executeCommand("CALIBRATE", pCmd=>pCmd.Mode = "Info");
      
      this._displayValues =  {
        co2: parseInt(result.Co2.PPM),
        waitSeconds: parseInt(result.Co2.CalibWaitTime)
      };
      return structuredClone(this._displayValues);
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
      var remaining =this._displayValues.waitSeconds - elapsed;
      if(remaining < 0) remaining =  0;
      return remaining;
    }
}

export default CalibrationSettingsView;