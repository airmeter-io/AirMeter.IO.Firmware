import Delay from './Delay';

export interface IWirelessNetwork {
    ssid : string;
    apMacAddr : string
    channel : number;
    signalStrength : number;
    authMode : string;
}

export interface IConfiguredNetwork {
    ssid : string;
    authMode : string;   
    priority : number;
    connection : IConnectedWirelessNetwork |undefined;
}

export interface IConnectedWirelessNetwork {
    channel : number;
    ip4Address : string;
    ip4Netmask : string;
    ip4Gateway : string;
}

export interface IGeneralSettingsValues {
    deviceName : string;
    enablePowerSaving : boolean;
}

export interface IAvailableNetworkPriority {
    up : boolean;
    priority : number;
}


class WirelessSettingsView {
    private _values : IGeneralSettingsValues = { 
        deviceName: "AIQ2134",
        enablePowerSaving: true
    };

    private _networks : IWirelessNetwork[] = [];
    private _configuredNetworks : IConfiguredNetwork[] = [];
    

    public async Load() {
        await Delay(600);
        return structuredClone(this._values);
    }

    public async Save(pValues : IGeneralSettingsValues) {
        await Delay(1000);
        this._values = pValues;
    }

    public async ScanForNetworks() {
        await Delay(1000);
        this._networks =  [
            {
                ssid: "TestNet1",
                apMacAddr: "00-15-5D-2E-36-7E",
                channel: 5,
                signalStrength: -45,
                authMode: "WPA"
            },
            {
                ssid: "TestNet2",
                apMacAddr: "00-15-5D-2E-36-7B",
                channel: 5,
                signalStrength: -35,
                authMode: "WPA"
            },
            {
                ssid: "CheeseNetAE4",
                apMacAddr: "00-15-5D-2E-36-7E",
                channel: 5,
                signalStrength: -45,
                authMode: "WPA"
            },
            {
                ssid: "EIRCOME43",
                apMacAddr: "00-15-5D-2E-36-7B",
                channel: 5,
                signalStrength: -35,
                authMode: "WPA"
            }
        ];
        this._networks.sort((net1,net2) => 
            net1.signalStrength < net2.signalStrength ? 1 :
            net1.signalStrength > net2.signalStrength ? -1 : 0);
    }

    public GetNetworks() :  IWirelessNetwork[] {        
        return this._networks.filter(pNetwork=>this._configuredNetworks.find(pCfg=>pCfg.ssid===pNetwork.ssid)===undefined);
    }

    public async GetConfiguredNetworks() : Promise<IConfiguredNetwork[]> {
        await Delay(1000);
        this._configuredNetworks.sort((net1,net2) => 
            net1.priority > net2.priority ? 1 :
            net1.priority < net2.priority ? -1 : 0);
        return this._configuredNetworks;
    }

    public async TestNetwork(pNetwork : IWirelessNetwork, pCredential : string | undefined) {
        await Delay(1000);
        return true;
    }

    public async AddNetwork(pNetwork : IWirelessNetwork, pSwitch : boolean, pMakeDefault : boolean, pCredential : string | undefined) {
        await Delay(1000);
        var network : IConfiguredNetwork | undefined;
        if(pSwitch) {
            for(var i = 0; i < this._configuredNetworks.length;i++)
                this._configuredNetworks[i].connection = undefined;
        }
        if(pMakeDefault) {
            network = {
                ssid: pNetwork.ssid,
                authMode: pNetwork.authMode,
                priority: 0,
                connection: pSwitch ? {
                    channel: 5,
                    ip4Address: "192.168.90.43",
                    ip4Netmask: "255.255.255.0",
                    ip4Gateway: "192.168.90.1"
                } : undefined              
            };
            this._configuredNetworks.splice(0,0, network);
            for(var i = 1; i < this._configuredNetworks.length; i++)
                this._configuredNetworks[i].priority = i;
        } else {
            network = {
                ssid: pNetwork.ssid,
                authMode: pNetwork.authMode,
                priority: this._configuredNetworks.length,
                connection: pSwitch ? {
                    channel: 5,
                    ip4Address: "192.168.90.43",
                    ip4Netmask: "255.255.255.0",
                    ip4Gateway: "192.168.90.1"
                } : undefined          
            }
            this._configuredNetworks.push(network);
        }
       
    }

    public GetAvailableNetworkPriorities(pNetwork : IConfiguredNetwork) : IAvailableNetworkPriority[] {
        var priorities : IAvailableNetworkPriority[] = [];

        for(var i = 0; i< pNetwork.priority; i++) 
            priorities.push( { up: true, priority: i });

        for(var i = pNetwork.priority+1; i< this._configuredNetworks.length; i++) 
            priorities.push( { up: false, priority: i });

        return priorities;
    }

    public async SetNetworkPriority(pNetwork : IConfiguredNetwork, pNewPriority : number) {
        console.log(pNetwork.ssid+": "+ pNetwork.priority+ " -> "+pNewPriority)
        await Delay(1000);
        for(var i = 0; i < this._configuredNetworks.length; i++) 
            if(this._configuredNetworks[i].ssid!==pNetwork.ssid) {
                if(this._configuredNetworks[i].priority>pNetwork.priority)
                    this._configuredNetworks[i].priority--;
                if(this._configuredNetworks[i].priority>=pNewPriority)
                    this._configuredNetworks[i].priority++;
            }
        
        pNetwork.priority = pNewPriority;
        this._configuredNetworks.sort((net1,net2) => 
            net1.priority > net2.priority ? 1 :
            net1.priority < net2.priority ? -1 : 0); 
            
        return this._configuredNetworks;
    }

    public async RemoveNetwork(pNetwork : IConfiguredNetwork) {
        console.log("Remove: "+ pNetwork.ssid);
        await Delay(1000);

        this._configuredNetworks.splice(pNetwork.priority,1);

        for(var i = pNetwork.priority; i < this._configuredNetworks.length; i++) 
            if(this._configuredNetworks[i].ssid!==pNetwork.ssid) {
                this._configuredNetworks[i].priority--;
            }                
            
        return this._configuredNetworks;
    }
}

export default WirelessSettingsView;