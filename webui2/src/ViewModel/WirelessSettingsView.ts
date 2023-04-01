import Delay from './Delay';
import ConnectionManager from '../Protocol/RestApi'

const connection = new ConnectionManager();

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
    ipv4Address : string;
    ipv4Netmask : string;
    ipv4Gateway : string;
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
        var result = await connection.executeCommand("GETNETWORKS");
        this._networks = result.Networks;
        for(var i = 0; i<this._networks.length; i++)
            this._networks[i].signalStrength = parseInt(result.Networks[i].signalStrength);
        this._networks.sort((net1,net2) => 
            net1.signalStrength < net2.signalStrength ? 1 :
            net1.signalStrength > net2.signalStrength ? -1 : 0);
    }

    public GetNetworks() :  IWirelessNetwork[] {        
        return this._networks.filter(pNetwork=>this._configuredNetworks.find(pCfg=>pCfg.ssid===pNetwork.ssid)===undefined);
    }

    public async GetConfiguredNetworks() : Promise<IConfiguredNetwork[]> {
        var result = await connection.executeCommand("SELECTNETWORK", pCmd=>{
            pCmd.Mode = "List";            
        });   
        for(var i = 0; i<result.Networks.length; i++)
            result.Networks[i].priority = parseInt(result.Networks[i].priority);
        result.Networks.sort((net1 : IConfiguredNetwork,net2 : IConfiguredNetwork) => 
            net1.priority > net2.priority ? 1 :
            net1.priority < net2.priority ? -1 : 0);
        this._configuredNetworks = result.Networks;
        return result.Networks;
    }

    public async TestNetwork(pNetwork : IWirelessNetwork, pCredential : string | undefined) {
        var first = true;
    
        while(true) {
            try {
                if(first) {
                    first = false;
                    connection.executeCommand("SELECTNETWORK", pCmd=>{
                        pCmd.Mode = "Test";
                        pCmd.Ssid = pNetwork.ssid;
                        pCmd.Password = pCredential;
                        pCmd.Auth = pNetwork.authMode;
                        pCmd.Id = Math.floor(Math.random() * 1000000);
                    });  
                       
                } else {
                    var result = await connection.executeCommand("SELECTNETWORK", pCmd=>{
                        pCmd.Mode = "Status";                        
                    });

                    if(result.Testing==="false") {
                        return result.TestSuccess==="true";
                    }
                }

            } catch {

            }
            await Delay(2500);
        }
        return true;
    }

    public async AddNetwork(pNetwork : IWirelessNetwork, pMakeDefault : boolean, pCredential : string | undefined) {
        var first = true;
        while(true) {
            try {
                if(first) {
                    first = false;
                    connection.executeCommand("SELECTNETWORK", pCmd=>{
                        pCmd.Mode = "Apply";
                        pCmd.Ssid = pNetwork.ssid;
                        pCmd.Password = pCredential;
                        pCmd.Auth = pNetwork.authMode;
                        pCmd.MakeDefault = pMakeDefault.toString();
                        pCmd.Id = Math.floor(Math.random() * 1000000);
                    });        
                } else {
                    var result = await connection.executeCommand("SELECTNETWORK", pCmd=>{
                        pCmd.Mode = "Status";                        
                    });
                    if(result.Applying==="false") {
                        return true;
                    }
                }

            } catch {

            }
            await Delay(2500);
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
        var removeResult = await connection.executeCommand("SELECTNETWORK", pCmd=>{
            pCmd.Mode = "Remove";            
            pCmd.Ssid= pNetwork.ssid;
        });   
        var result = await connection.executeCommand("SELECTNETWORK", pCmd=>{
            pCmd.Mode = "List";            
        });   
        result.Networks.sort((net1 : IConfiguredNetwork,net2 : IConfiguredNetwork) => 
            net1.priority > net2.priority ? 1 :
            net1.priority < net2.priority ? -1 : 0);
        this._configuredNetworks = result.Networks;
        return this._configuredNetworks;
    }
}

export default WirelessSettingsView;