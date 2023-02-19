import Delay from './Delay';

export interface IGeneralSettingsValues {
    deviceName : string;
    enablePowerSaving : boolean;
}

class WirelessSettingsView {
    private _values : IGeneralSettingsValues = { 
        deviceName: "AIQ2134",
        enablePowerSaving: true
    };

    public async Load() {
        await Delay(600);
        return structuredClone(this._values);
    }

    public async Save(pValues : IGeneralSettingsValues) {
        await Delay(1000);
        this._values = pValues;
    }
}

export default WirelessSettingsView;