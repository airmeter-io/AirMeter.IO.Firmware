import Delay from './Delay';
import ConnectionManager from '../Protocol/RestApi'

export interface IMeterValue {
    name : string;
    value : string;
}

export interface IMeterValueGroup {
    name : string;
    values : IMeterValue[];
}

export interface IMeterValues {
    co2 : number;
    humidity : number;
    pressure : number;
    temp: number;
    groups : IMeterValueGroup[];
}

const connection = new ConnectionManager;

class MeterView {
    public async GetLatest() : Promise<IMeterValues> {
        var result = await connection.executeCommand("GETLATEST");

        var ret : IMeterValues = {
            co2: parseInt(result.Co2.PPM),
            humidity: parseFloat(result.Atmosphere.Humidity),
            pressure: parseFloat(result.Atmosphere.Pressure),
            temp: parseFloat(result.Atmosphere.Temp),
            groups: []
        };
        for(var group in result) {
            if(group==="Status" || group==="Time") continue;

            var newGroup : IMeterValueGroup = {
                name: group,
                values: []
            };

            for(var value in result[group]) {
                newGroup.values.push({ 
                    name: value,
                    value: result[group][value]
                });
            }
            ret.groups.push(newGroup);
        }

        return ret;        
    }

}

export default MeterView;