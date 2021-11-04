import React from 'react';
import Popup from '../popups/popup.js'
import {PopupAction} from "../popups/popup.js";
import FormTextField from "../form/textField.js"
import FormCheckboxField from "../form/checkboxField.js"
import FormDropDownField from "../form/dropDownField.js"
import FormSliderField from "../form/sliderField.js"
import Form from "../form/form.js"
import CommandManager from '../commandManager.js';

class SettingsPanel extends React.Component {
    constructor(props) {
      super(props);
      this.state = {  settings: {} };
      this.form = React.createRef();
      this.editPopup = React.createRef();
      this.showButton = React.createRef();
    }

    loadData(pDone) {
        CommandManager.executeCommand(false, "LOADSETTINGS", pData=> {
          this.setState({ 
            settings: {
              DeviceName:pData.DeviceName,
              EnableMqtt:JSON.parse(pData.EnableMqtt.toLowerCase()),                    
              EnableDhcpNtp: JSON.parse(pData.EnableDhcpNtp.toLowerCase()),
              MqttServerAddress: pData.MqttServerAddress,
              MqttTopic: pData.MqttTopic,
              MqttPublishDelay: parseInt(pData.MqttPublishDelay),
              Co2SensorType: parseInt(pData.Co2SensorType,10),
              SensorUpdateInterval: parseInt(pData.SensorUpdateInterval,10)
            },
            saved: {

            },
            EnableMqttEdit:JSON.parse(pData.EnableMqtt.toLowerCase())});
          
          if(pDone!=undefined)
            pDone();                      
        });     
      }
    

    componentDidMount () {
        this.loadData();       
        const listenerFn = this.showChangeSettingsPopup.bind(this);
        this.showButton.current.addEventListener("click", listenerFn);
      
        this.removeListener = () => {
          this.showButton.current.removeEventListener('click', listenerFn);

        };
      }
      componentWillUnmount () {
        this.removeListener();
      }
  
    convertSensorType(pType)
    {
      switch(pType) {
        case 0 : return "None";
        case 1 : return "MH-Z19B/C and similar";
        case 2 : return "CM1107 &amp; CM1106";
        case 3 : return "Senseair ModBus (UART)";
        default : return "Unknown";
      }
    }

    showChangeSettingsPopup() {
      this.loadData(()=>{
        this.editPopup.current.show();
      });
    }

    formatUpdateInterval(pIntterval) {
      if(Math.floor(pIntterval/60)>0)
        return (<strong>{Math.floor(pIntterval/60)} minutes and {pIntterval-(Math.floor(pIntterval/60)*60)} seconds</strong>);
      else
      return (<strong>{pIntterval-(Math.floor(pIntterval/60)*60)} seconds.</strong>);
    }
    render() {
      const editActions = [
        new PopupAction("Save", ()=> {
          var result = this.form.current.validate();
          if(result) {
              this.form.current.save();
              CommandManager.executeCommand(false, "SAVESETTINGS", pData=> {
                 if(pData.Status=="true") {
                    this.editPopup.current.hide();  
                    this.loadData();
                 }
              },
              pCmd=>{
                pCmd.DeviceName = this.state.saved.DeviceName;
                pCmd.EnableMqtt = this.state.saved.EnableMqtt? 'true' : 'false';          
                pCmd.EnableDhcpNtp = this.state.saved.EnableDhcpNtp? 'true' : 'false';
                pCmd.MqttServerAddress = this.state.saved.MqttServerAddress;
                pCmd.MqttTopic = this.state.saved.MqttTopic;
                pCmd.MqttPublishDelay = this.state.saved.MqttPublishDelay;
                pCmd.Co2SensorType = this.state.saved.Co2SensorType;
                pCmd.SensorUpdateInterval = this.state.saved.SensorUpdateInterval;
              });                
          }
        })];
      return (
        <div>
          <Popup ref={this.editPopup}  Title="Change Settings" Actions={editActions} >
            <Form ref={this.form}>
                <FormTextField 
                  Label="Device Name" 
                  PlaceHolder="Network identity of device" 
                  Value={this.state.settings.DeviceName} 
                  Mandatory={true} 
                  Enabled={true}
                  OnSave={pValue=>this.state.saved.DeviceName=pValue} 
                  MandatoryMessage="The device must have a valid name"
                  Pattern="[\w\d]+" 
                  PatternMessage="The device name includes invalid characters"/>                      
                <FormDropDownField Label="CO2 Sensor" Value={this.state.settings.Co2SensorType} OnSave={pValue=>this.state.saved.Co2SensorType=pValue}>
                  <option value={0}>None</option>
                  <option value={1}>MH-Z19B/C and similar</option>
                  <option value={2}>CM1107 &amp; CM1106</option>
                  <option value={3}>Senseair ModBus (UART)</option>
                </FormDropDownField>        
                                  
                <FormSliderField  Min={1} Max={60} Step={1} FormatCurrentValue={pValue=>{
                    return ( <div>Read values from sensor every <strong>{pValue}</strong> seconds.</div>);
                }} Value={this.state.settings.SensorUpdateInterval} OnSave={pValue=>this.state.saved.SensorUpdateInterval=pValue}/>                      
                <FormCheckboxField 
                  Label="Enable MQTT" 
                  Value={this.state.EnableMqttEdit} 
                  OnSave={pValue=>this.state.saved.EnableMqtt=pValue}
                  OnChange={pValue=> {
                    this.setState( { EnableMqttEdit: pValue});                          
                  }}/>                     
                <FormTextField 
                  PlaceHolder="mqtt://... or mqtts://..." 
                  Label="MQTT Server Address" 
                  Value={this.state.settings.MqttServerAddress}  
                  OnSave={pValue=>this.state.saved.MqttServerAddress=pValue} 
                  Mandatory={this.state.EnableMqttEdit}
                  Enabled={this.state.EnableMqttEdit}
                  Pattern="^((mqtt:\/\/|(mqtts:\/\/))([\w-]{1,63}\.)+[\w]{1,6})(:\d+)?$"
                  PatternMessage="Must be a URI in the form mqtt[s]://serveraddress[:port]"/>  
                <FormTextField 
                  PlaceHolder="Use %DEVICE_NAME% to place name of device in topic path " 
                  Label="MQTT Topic Path"                        
                  Enabled={this.state.EnableMqttEdit}
                  PatternMessage="Must be a valid MQTT topic path"
                  Mandatory={this.state.EnableMqttEdit}
                  Value={this.state.settings.MqttTopic}  
                  OnSave={pValue=>this.state.saved.MqttTopic=pValue}/>  
                <FormSliderField  
                  Min={30} 
                  Max={3600} 
                  Step={30} 
                  FormatCurrentValue={pValue=>( 
                    <div>Publisn to MQTT server every <strong>{Math.floor(pValue/60)}</strong> minutes and <strong>{pValue-(Math.floor(pValue/60)*60)}</strong> seconds.</div>)} 
                  Value={this.state.settings.MqttPublishDelay}  
                  OnSave={pValue=>this.state.saved.MqttPublishDelay=pValue}/>      
                <FormCheckboxField 
                  Label="Configure NTP using DHCP" 
                  Value={this.state.settings.EnableDhcpNtp} 
                  OnSave={pValue=>this.state.saved.EnableDhcpNtp=pValue}  />                          
            </Form>
          </Popup>
            <p>&nbsp;</p>
            <div class="w3-container">                               
                <div class="w3-card-4">

                  <header class="w3-container w3-light-grey">
                    <h3>Settings</h3>
                    
                  </header>

                  <div class="w3-container">
                    <p>This device will appear as <strong>{this.state.settings.DeviceName}</strong> on the network and is using a <strong>{this.convertSensorType(this.state.settings.Co2SensorType)}</strong>
                    &nbsp; type sensor that is read every {this.formatUpdateInterval(this.state.settings.SensorUpdateInterval)}.</p>
                    
                    <p>{(this.state.settings.EnableMqtt==true ? [
                      <span>MQTT telemetry is <strong>enabled</strong> to publish updates to <strong>{this.state.settings.MqttServerAddress}/{this.state.settings.MqttTopic}</strong>
                      &nbsp;every {this.formatUpdateInterval(this.state.settings.MqttPublishDelay)}.</span>
                    ] : <span>MQTT telemetry is <strong>disabled</strong></span>)}</p>
                    <p>{this.state.settings.EnableDhcpNtp==true ? 
                      <span>NTP Configuration via DHCP is <strong>enabled</strong>. </span> :
                      <span>NTP Configuration via DHCP is <strong>disabled</strong>. </span>}</p>
                  </div>
                  <button ref={this.showButton} class="w3-button w3-block w3-dark-grey">Change</button>
                  

                </div>

            </div>
        </div>            
      );
    }
  }

  export default SettingsPanel;