import React from 'react';
import EnableABCWizard from './enableABCWizard.js';
import DisableABCWizard from './disableABCWizard.js';
import CommandManager from '../commandManager.js';
import BackgroundCalibrationWizard from './backgroundCalibration.js';

class CalibrationPanel extends React.Component {
    constructor(props) {
      super(props);
      this.state = {  info: {} };
      this.form = React.createRef();
      this.backgroundCalibrationPopup = React.createRef();
      this.enableABCPopup = React.createRef();
      this.disableABCPopup = React.createRef();
      this.showBackgroundCalibrationButton = React.createRef();
      this.showConfigureABCButton = React.createRef();
    }

    loadData(pDone) {
        CommandManager.executeCommand(false, "GETLATESTCALIB", pData=> {
          this.setState({ 
            info: {
              ABCEnabled: pData.ABCEnabled=="true",
              MinBaseline: pData.MinBaseline,
              MaxBaseline: pData.MaxBaseline,
              Baseline: pData.Baseline,
              CalibWaitTime: pData.CalibWaitTime,
              MinABCDaysPerCycle: pData.MinABCDaysPerCycle,
              MaxABCDaysPerCycle: pData.MaxABCDaysPerCycle,
              DaysPerABCCycle: pData.DaysPerABCCycle
            }
          });
          if(pDone!=undefined)
            pDone();                      
        });     
      }
    

    componentDidMount () {
        this.loadData();       
        const showBackground = this.showBackgroundCalibrationPopup.bind(this);
        const showConfigureABC = this.showConfigureABCPopup.bind(this);
        this.showBackgroundCalibrationButton.current.addEventListener("click", showBackground);
        this.showConfigureABCButton.current.addEventListener("click", showConfigureABC);
        this.removeListener = () => {
          this.showBackgroundCalibrationButton.current.removeEventListener('click', showBackground);
          this.showConfigureABCButton.current.removeEventListener('click', showConfigureABC);

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

    showBackgroundCalibrationPopup() {
      this.loadData(()=>{
        this.backgroundCalibrationPopup.current.show();
      });
    }

    showConfigureABCPopup() {
        this.loadData(()=>{
          if(this.state.info.ABCEnabled) 
            this.disableABCPopup.current.show();
          else
            this.enableABCPopup.current.show();
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
        ];
      return (
        <div>
            
          <DisableABCWizard ref={this.disableABCPopup} Info={this.state.info} OnChange={this.loadData.bind(this)}/>
          <EnableABCWizard ref={this.enableABCPopup} Info={this.state.info} OnChange={this.loadData.bind(this)}/> 
          <BackgroundCalibrationWizard ref={this.backgroundCalibrationPopup} Info={this.state.info} OnChange={this.loadData.bind(this)} CalibWaitTime={this.state.info.CalibWaitTime}/> 
           
            <p>&nbsp;</p>
            <div class="w3-container">                               
                <div class="w3-card-4">
                  <header class="w3-container w3-light-grey">
                    <h3>Calibration</h3>                    
                  </header>

                  <div class="w3-container">
                  <p>ABC (Automatic Baseline Calibration) is <strong>{this.state.info.ABCEnabled ? "enabled" : "disabled"}</strong> the baseline CO2 ppm value is
                  &nbsp;<strong>{this.state.info.Baseline}</strong>  the calibration cycle length is  <string>{this.state.info.DaysPerABCCycle} days</string>
                  </p>
                  </div>
                  <div>
                    <button ref={this.showConfigureABCButton} class="w3-button w3-half w3-dark-grey">{this.state.info.ABCEnabled ? "Disable" : "Enable"} ABC</button>
                    <button ref={this.showBackgroundCalibrationButton} class="w3-button w3-half w3-dark-grey">Force Background Calibration</button>
                  </div>
                </div>

            </div>
        </div>            
      );
    }
  }

  export default CalibrationPanel;