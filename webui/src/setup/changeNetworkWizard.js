import React from 'react';
import Wizard from '../popups/wizard';
import {WizardStep} from '../popups/wizard';
import Form from '../form/form';
import FormTextField from '../form/textField';
import FormPasswordField from '../form/passwordField';
import CommandManager from '../commandManager';
import { PopupAction } from '../popups/popup';
import './changeNetworkWizard.css';

class NetworkDisplayItem extends React.Component {
    constructor(props) {
        super(props);
        this.buttonRef = React.createRef();
    }

    handleClick() {
        if(!this.props.IsSelected) {
          this.props.OnClick(this.props.Network);
        }
    }
    
    componentDidMount () {
        const listenerFn = this.handleClick.bind(this);
        this.buttonRef.current.addEventListener("click", listenerFn);
        this.removeListener = () => this.buttonRef.current.removeEventListener('click', listenerFn)
    }
    componentWillUnmount () {
        this.removeListener();
    }

    render() {
        return (
            <li class={this.props.IsSelected ? "" : "w3-teal"} ref={this.buttonRef}>
                <h5>{this.props.Network.WifiSSID}</h5>
                <div>{this.props.Network.WifiAuthMode} on channel {this.props.Network.WifiChannel} ({this.props.Network.WifiSignalStrength} dBm)</div>
                <div>MAC Addr: {this.props.Network.WifiApMacAddr}</div>
            </li>);
    }
}

class ChangeNetworkWizard extends React.Component {
    constructor(props) {
        super(props);
        this.state = { networks: [], success: false, testing: false, applying: false, hasError: false, error: null, saved: {}, selected: null, password: null };
        this.wizardRef = React.createRef();
        this.form = React.createRef();
        this.enableUpdates = false;
    }

    show() {
        this.wizardRef.current.show();       
        this.loadData(); 
        this.cnInterval = setInterval(this.tickUpdatesFunc.bind(this), 1000);   
    }

    onHide() {
        clearInterval(this.cnInterval);
    }

    tickUpdatesFunc() {
        if(!this.enableUpdates) return;
        CommandManager.executeCommand(false, "SELECTNETWORK", pData=> {
            this.setState({ 
                testing: pData.Testing=="true",
                success: pData.TestSuccess,
                applying: pData.Testing=="true",
            });
            if(!this.state.testing)
                enableUpdates = false;
                                
          }, 
          pCmd=> {
              pCmd.Mode = "Status";
          });   
    }

    loadData(pDone) {
        CommandManager.executeCommand(false, "GETNETWORKS", pData=> {
          this.setState({  networks: pData.Networks});
          
          if(pDone!=undefined)
            pDone();                      
        });     
      }

    
      selectItem(item)  {
        this.setState({ 
          selected: item 
        });     
      }
  

    render() {
        return [
            <Wizard ref={this.wizardRef} Steps={[
                new WizardStep(
                    ()=>
                        <div>
                            {this.state.networks.map(item =>
                                <ul class="w3-ul w3-border">
                                    <NetworkDisplayItem  Network={item} IsSelected={this.state.selected == item} OnClick={this.selectItem.bind(this)}/>
                                </ul> )}
                        </div>, 
                    "Choose Network", 
                    ()=>true, 
                    ()=>true, 
                    ()=> { 
                        return this.state.selected!=null;
                    } ),
                new WizardStep(
                    ()=>
                        <div>
                            <Form ref={this.form}>                                                            
                                <FormPasswordField 
                                    PlaceHolder="Password" 
                                    Label={"Wifi Network Password For: "+this.state.selected.WifiSSID} 
                                    Value={this.state.password}  
                                    OnSave={pValue=>this.state.password=pValue} 
                                    type="password"
                                    Mandatory={true}
                                    Enabled={true}
                                    MandatoryMessage={"Please provide the password for "+this.state.selected.WifiSSID+" to which you are connecting."}  />                                                                
                            </Form>
                        </div>, 
                        "Connect to Network", 
                    ()=>true, 
                    ()=>true, 
                    ()=> {
                        var result = this.form.current.validate();
                        if(result) {
                            this.form.current.save();
                            this.setState({testing: true});
                            CommandManager.executeCommand(false, "SELECTNETWORK", pData=> {              
                              }, 
                              pCmd=> {
                                  pCmd.Mode = "Test";
                                  pCmd.Ssid = this.state.selected.WifiSSID;
                                  pCmd.Password = this.state.password;
                                  pCmd.Id =  Math.floor(Math.random() * 1000000);
                              }); 
                            this.enableUpdates = true;
                            return true;
                        }
                        return false;
                    },
                    ()=>[]),
                new WizardStep(
                        ()=>
                        <div>
                            
                            {
                        this.state.testing ?
                            <div>
                                <div class="w3-panel w3-pale-blue w3-border">
                                    <h3>Attempting to connect to the network..</h3>
                                    <p>Please wait while the details you provided are used to attmempt to connect to the network.</p>
                                </div>
                            </div> :
                                                   
                        this.state.success ?
                            <div class="w3-panel w3-pale-green w3-border">
                                <h3>Success</h3>
                                <p>Successfully connected to network.</p>
                            </div>  :
                        this.state.hasError && this.state.error!=undefined ?
                            <div>
                            <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error connecting to network</h3>
                                <p>{this.state.error}</p>
                                </div>
                            </div> :                        
                            <div>
                                <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error connecting to network</h3>
                                <p>An error occured connecting to the network with the password you entered.</p>
                                </div>
                            </div> 
                            }
                        </div>, 
                        "Testing Network Connection", 
                        ()=>true, 
                        ()=>true, 
                        ()=> {
                            if(this.state.success) {
                                this.enableUpdates = false;
                              
                                CommandManager.executeCommand(false, "SELECTNETWORK", pData=> {              
                                    }, 
                                    pCmd=> {
                                        pCmd.Mode = "Apply";
                                        pCmd.Ssid = this.state.selected.WifiSSID;
                                        pCmd.Password = this.state.password;
                                        pCmd.Id = Math.floor(Math.random() * 1000000);
                                    }); 
                                
                                return true;
                              
                            }

                            return false;
                        },
                        ()=>[]),
                new WizardStep(
                        ()=>
                        <div>
                             <div class="w3-panel w3-pale-green w3-border">
                                <h3>The changes have now been applied</h3>
                                <p>As these changes caused the network connection to restart.</p>
                            </div> 
                        </div>, 
                        "Applying changes", 
                        ()=>true, 
                        ()=>true, 
                        ()=> {
    
                        },
                        ()=>[
                            new PopupAction("Done", ()=>{                            
                                return true;
                            },true)
                        ])
            ]}/>
        ];
    }
}

export default ChangeNetworkWizard;