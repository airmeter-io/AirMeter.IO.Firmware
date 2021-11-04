import React from 'react';
import Popup from '../popups/popup.js'
import {PopupAction} from "../popups/popup.js";
import FormTextField from "../form/textField.js"
import FormCheckboxField from "../form/checkboxField.js"
import FormDropDownField from "../form/dropDownField.js"
import FormSliderField from "../form/sliderField.js"
import Form from "../form/form.js"
import CommandManager from '../commandManager.js';

class DataPanel extends React.Component {
    constructor(props) {
      super(props);
      this.state = {  settings: {} };
      this.form = React.createRef();
      this.clearPopup = React.createRef();
      this.clearButton = React.createRef();
    }

    loadData(pDone) {
        // CommandManager.executeCommand(false, "LOADSETTINGS", pData=> {
        //   this.setState({ 
        //     settings: {
        //       DeviceName:pData.DeviceName,
        //       EnableMqtt:JSON.parse(pData.EnableMqtt.toLowerCase()),                    
        //       EnableDhcpNtp: JSON.parse(pData.EnableDhcpNtp.toLowerCase()),
        //       MqttServerAddress: pData.MqttServerAddress,
        //       MqttTopic: pData.MqttTopic,
        //       MqttPublishDelay: parseInt(pData.MqttPublishDelay),
        //       Co2SensorType: parseInt(pData.Co2SensorType,10),
        //       SensorUpdateInterval: parseInt(pData.SensorUpdateInterval,10)
        //     },
        //     saved: {

        //     },
        //     EnableMqttEdit:JSON.parse(pData.EnableMqtt.toLowerCase())});
          
        //   if(pDone!=undefined)
        //     pDone();                      
        // });     
      }
    

    componentDidMount () {
        this.loadData();       
        const listenerFn = this.showClearPopup.bind(this);
        this.clearButton.current.addEventListener("click", listenerFn);
      
        this.removeListener = () => {
          this.clearButton.current.removeEventListener('click', listenerFn);

        };
      }
      componentWillUnmount () {
        this.removeListener();
      }
  
    
    showClearPopup() {  
        this.clearPopup.current.show();
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
              if(this.state.UserIsSure) {
                CommandManager.executeCommand(false, "MANAGEDATA", pData=> {
                    if(pData.Status=="true") {
                        this.editPopup.current.hide();  
                        this.loadData();
                    }
                    
                },
                pCmd=>{
                    pCmd.Mode = "ClearAllData";                   
                });    
            }            
          }
        })];
      return (
        <div>
          <Popup ref={this.clearPopup}  Title="Clear All Data" Actions={editActions} >
            <Form ref={this.form}>
                                     
                <FormCheckboxField 
                  Label="Yes I am sure" 
                  Value={this.state.UserIsSure} 
                  OnSave={pValue=>this.state.UserIsSure=pValue}
                  OnChange={pValue=> {
                    this.setState( { UserIsSure: pValue});                          
                  }}/>                                      
            </Form>
          </Popup>
            <p>&nbsp;</p>
            <div class="w3-container">                               
                <div class="w3-card-4">

                  <header class="w3-container w3-light-grey">
                    <h3>Data</h3>
                    
                  </header>

                  <div class="w3-container">
                    <p>Something interesting about the data here</p>
                  </div>
                  <button ref={this.clearButton} class="w3-button w3-block w3-dark-grey">Clear All Data</button>
                  

                </div>

            </div>
        </div>            
      );
    }
  }

  export default DataPanel;