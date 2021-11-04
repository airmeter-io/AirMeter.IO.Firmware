import React from 'react';

import CommandManager from '../commandManager.js';
import ChangeNetworkWizard from './changeNetworkWizard.js';

class NetworkInfoPanel extends React.Component {
    constructor(props) {
      super(props);
      this.state = {  info: {} };
 
      this.showChangeNetworkButton = React.createRef();
      this.showConfigureNTPButton = React.createRef();
      this.changeNetworkWizard = React.createRef();
    }

    loadData(pDone) {
        CommandManager.executeCommand(false, "GETNETWORKINFO", pData=> {
          this.setState({ 
            info: {
              WifiSSID: pData.WifiSSID,
              WifiChannel: pData.WifiChannel,
              WifiAuthMode: pData.WifiAuthMode,
              WifiIP4Address: pData.WifiIP4Address,
              WifiIP4Netmask: pData.WifiIP4Netmask,
              WifiIP4Gateway: pData.WifiIP4Gateway
            }
          });
          if(pDone!=undefined)
            pDone();                      
        });     
      }
    
    showChangeNetworkWizard() {
      this.changeNetworkWizard.current.show();
    }

    showChangeNTPConfigWizard() {

    }

    componentDidMount () {
        this.loadData();       
        const showChangeNetwork = this.showChangeNetworkWizard.bind(this);
        const showChangeNTPConfig = this.showChangeNTPConfigWizard.bind(this);
        this.showChangeNetworkButton.current.addEventListener("click", showChangeNetwork);
        this.showConfigureNTPButton.current.addEventListener("click", showChangeNTPConfig);
        this.removeListener = () => {
          this.showChangeNetworkButton.current.removeEventListener('click', showChangeNetwork);
          this.showConfigureNTPButton.current.removeEventListener('click', showChangeNTPConfig);

        };
      }
      componentWillUnmount () {
        this.removeListener();
      }

    render() {
      return (
        <div>
            <p>&nbsp;</p>
            <ChangeNetworkWizard ref={this.changeNetworkWizard}/>
            <div class="w3-container">                               
                <div class="w3-card-4">
                  <header class="w3-container w3-light-grey">
                    <h3>Network</h3>                    
                  </header>
                  <div class="w3-container">
                    <p>
                      Connected to <code>{this.state.info.WifiSSID}</code> on channel <code>{this.state.info.WifiChannel}</code> using 
                      &nbsp;<code>{this.state.info.WifiAuthMode}</code> security.
                    </p>
                    <p>
                      Assigned <code>{this.state.info.WifiIP4Address}</code> as the IPv4 address with a netmask of <code>{this.state.info.WifiIP4Netmask}</code> and
                      &nbsp;<code>{this.state.info.WifiIP4Gateway}</code> as the gateway.                      
                    </p>
                  </div>
                  <div>
                    <button ref={this.showChangeNetworkButton} class="w3-button w3-half w3-dark-grey">Change Network</button>
                    <button ref={this.showConfigureNTPButton} class="w3-button w3-half w3-dark-grey">Configure NTP</button>
                  </div>
                </div>
            </div>
        </div>            
      );
    }
  }

  export default NetworkInfoPanel;