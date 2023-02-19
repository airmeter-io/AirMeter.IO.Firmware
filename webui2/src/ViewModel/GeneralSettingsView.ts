import Delay from './Delay';
import ConnectionManager from '../Protocol/RestApi'

export interface IGeneralSettingsValues {
    deviceName : string;
    enablePowerSaving : boolean;
}


const connection = new ConnectionManager();

class GeneralSettingsView {
    private _values : IGeneralSettingsValues = { 
        deviceName: "AIQ1234",
        enablePowerSaving: true
    };

    public async Load() {

        var result = await connection.executeCommand("LOADSETTINGS");

        this._values = {
            deviceName: result.DeviceName,
            enablePowerSaving: result.EnablePowerSave === "true"
        };
                
        return structuredClone(this._values);
    }

    public async Save(pValues : IGeneralSettingsValues) {
        var result = await connection.executeCommand("SAVESETTINGS", pCmd=>{
            pCmd.DeviceName = pValues.deviceName;
            pCmd.EnablePowerSave = pValues.enablePowerSaving.toString();

        });

        this._values.deviceName = pValues.deviceName;
    }
}

export default GeneralSettingsView;