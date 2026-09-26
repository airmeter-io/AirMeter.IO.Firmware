import Delay from './Delay';
import ConnectionManager from '../Protocol/RestApi'

const connection = new ConnectionManager();

export interface ITimeSettingsValues {
    ntp1 : string;
    ntp2 : string;
    enableDhcp : boolean;    
}

class TimeSettingsView {
    private _values : ITimeSettingsValues = { 
        ntp1: "1.2.3.4",
        ntp2: "ns1.pool.ntp.org",
        enableDhcp: false
    };

    public async Load() {
        var result = await connection.executeCommand("LOADSETTINGS");

        this._values = {
            ntp1: result.PrimaryNtpServer,
            ntp2: result.SecondaryNtpServer,
            enableDhcp: result.EnableDhcpNtp === true
        };
           
        return structuredClone(this._values);
    }

    public async Save(pValues : ITimeSettingsValues) {
        await connection.executeCommand("SAVESETTINGS", pCmd=>{
            pCmd.PrimaryNtpServer = pValues.ntp1;
            pCmd.SecondaryNtpServer = pValues.ntp2;
            pCmd.EnableDhcpNtp = pValues.enableDhcp.toString();
        });

        this._values = pValues;
    }
}

export default TimeSettingsView;