import React from 'react';
import Wizard from '../popups/wizard';
import {WizardStep} from '../popups/wizard';
import CommandManager from '../commandManager';
import { PopupAction } from '../popups/popup';
import CO2RadialGauge from '../meter/co2meter';
import "./backgroundCalibration.css"

class BackgroundCalibrationWizard extends React.Component {
    constructor(props) {
        super(props);
        this.state = { success: false, hasError: false, error: null, saved: {}, waitText: "", CO2:400 };
        this.wizardRef = React.createRef();
        this.form = React.createRef();
        var th = this;
        this.tickCO2Func = () => {
            th.tickCO2();
        }
    }

    show() {
        this.wizardRef.current.show();    
        this.co2Interval = setInterval(this.tickCO2Func, 10000);   
    }

    onHide() {
        clearInterval(this.co2Interval);
    }

    tickCO2() {
        CommandManager.executeCommand(false, "GETLATEST", pData=> {
            this.setState({ 
                CO2: parseInt(pData.Co2,10)
            });
                                
          });   
    }

    toHourMinsSecs(pSecs) {
        var sec_num = parseInt(pSecs, 10); // don't forget the second param
        var hours   = Math.floor(sec_num / 3600);
        var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
        var seconds = sec_num - (hours * 3600) - (minutes * 60);
    
        if (hours   < 10) {hours   = "0"+hours;}
        if (minutes < 10) {minutes = "0"+minutes;}
        if (seconds < 10) {seconds = "0"+seconds;}
        return hours+':'+minutes+':'+seconds;
    }

    calcElapsed() {
        return ((new Date()) - this.calibStartTime)/1000;
    }
    
    tickCalibStep2() {
    
        
      
        if(this.props.CalibWaitTime > this.calcElapsed()) {
            this.setState({ waitText: this.toHourMinsSecs(this.props.CalibWaitTime-this.calcElapsed())});            
            var th = this;
            setTimeout(() => {th.tickCalibStep2() }, 1000);
        } else {
            this.setState({ waitText: "00:00:00"});   
        }
    }

    render() {
        return [
            <Wizard OnClose={this.onHide.bind(this)} ref={this.wizardRef} Steps={[
                new WizardStep(
                  ()=><div>
                      
                        <p>Outside air that is away from sources of pollution has a relatively constant CO2 just above 400ppm. The CO2 sensor can  
                        use this to calibrate against as an approximate baseline value. Before calibration is performed it must be placed
                        outside.
                        </p>
                        <p>
                        The weather conditions must be dry, not excesively hot and the sensor should be placed away from:-
                        </p>
                        <p>
                        <ol>
                            <li>Sources of pollution/CO2 such as motor vehicles, home heating boilers/burners, fires or any other combustion.</li>
                            <li>Sources of heat, sunlight or infrared radiation.</li>
                            <li>People who are also sources of CO2.</li>
                            <li>Excessive humidity (&gt; 80%).</li>
                        </ol>
                        </p>

                       

                        <div class="w3-panel w3-pale-red w3-border">
                            <h3>WARNING</h3>
                            <p>Once you are sure the sensor unit has been placed in an appropriate location and can remain there safely for some minutes you may press the Continue button.</p>
                        </div>                                                       
                    </div>, 
                  "Prepare background calibration", 
                  ()=>true, 
                  ()=>true, 
                  ()=> {
                     this.calibStartTime = new Date();
                     this.tickCO2();
                     this.tickCalibStep2();
                     return true;
                  },
                  ()=>[
                      new PopupAction("Cancel", ()=>true,false)
                  ]),
                  new WizardStep(
                    ()=>
                        <div>
                             <div class="calibCo2GaugeContainer" >
                            <CO2RadialGauge Value={this.state.CO2}/>
                        </div>
                            <p> After ensuring that the sensor is located outside as per the instructions on the previous screen. you must now wait for the sensor
                                readings to stablise before you can complete the calibration process. This is because the reading taken
                                during calibration will be assumed to represent the ambient CO2 concentration level of outdoor air (E.g. around 400ppm).
                            </p>
                            <p>
                                During this time leave the sensor in position in the outdoor air and ensure that it is not obstructed or interfered with.
                            </p>
                            <p>
                                Wait will be complete in <code>{this.state.waitText}</code> after which you will be able to press <strong>Next &gt;&gt;</strong> to start the calibration.
                            </p>
                        </div>, 
                    "Wait for readings to settle", 
                    ()=>true, 
                    ()=>true, 
                    ()=> {
                        var result =  (this.props.CalibWaitTime <= this.calcElapsed());
                        if(result) {
                            CommandManager.executeCommand(false,"MANUALCALIB", pData=> {
                                if(pData.Status == "true") {
                                    this.setState({ success: true });
                                } else {
                                    if(pData.Eerror!=undefined)
                                        this.setState({ hasError: true, error: pData.Error });
                                    else
                                        this.setState({ hasError: true });
                                }                                                     
                              });                              
                        }

                        return result;
                    },
                    ()=>[                        
                    ]),
                    new WizardStep(
                      ()=>
                         <div>
                            <div class="calibCo2GaugeContainer" >
                                <CO2RadialGauge Value={this.state.CO2}/>
                            </div>
                            {this.state.success ?
                            <div class="w3-panel w3-pale-green w3-border">
                                <h3>Success</h3>
                                <p>Successfully calibrated using background CO2 level.</p>
                            </div>  :
                        this.state.hasError && this.state.error!=undefined ?
                            <div>
                                <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error</h3>
                                <p>{this.state.error}</p>
                                </div>
                            </div> : 
                        this.state.hasError ? 
                            <div>
                                <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error</h3>
                                <p>An error during calibration.</p>
                                </div>
                            </div> :
                            <div>
                                <div class="w3-panel w3-pale-blue w3-border">
                                    <h3>Calibrating..</h3>
                                    <p>Please wait while the background calibration is performed.</p>
                                </div>
                            </div>}
                        </div>, 
                      "Calibrating...", 
                      ()=>true, 
                      ()=>true, 
                      ()=> {},
                      ()=>[
                          new PopupAction("Done", ()=>{                            
                              return this.state.success || this.state.hasError;
                          },true)
                      ])
            ]}/>
        ];
    }
}

export default BackgroundCalibrationWizard;