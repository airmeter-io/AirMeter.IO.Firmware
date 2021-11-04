import React from 'react';
import Wizard from '../popups/wizard';
import {WizardStep} from '../popups/wizard'
import Form from '../form/form';
import FormSliderField from '../form/sliderField';
import CommandManager from '../commandManager';
import { PopupAction } from '../popups/popup';

class EnableABCWizard extends React.Component {
    constructor(props) {
        super(props);
        this.state = { success: false, hasError: false, error: null, saved: {} };
        this.wizardRef = React.createRef();
        this.form = React.createRef();
    }

    show() {
        this.wizardRef.current.show();        
    }

    render() {
        return [
            <Wizard ref={this.wizardRef} Steps={[
                new WizardStep(
                  ()=><div>
                      <p>Automatic Baseline Calibration (ABC) is a mode in which the CO2 sensor to automatically calibrates 
                        itself against the minimum reading it receives over a period defined in days. </p>
                      <p>This screen allows you to specify the number of days and also the baseline CO2 value to use if your 
                         sensor allows these values to be set. Otherwise it shows the values hard coded within the sensor. </p>
                      <Form ref={this.form}>                                                            
                        <FormSliderField  
                            Min={this.props.Info.MinABCDaysPerCycle} 
                            Max={this.props.Info.MaxABCDaysPerCycle} 
                            Step={1} 
                            FormatCurrentValue={pValue=>{
                                return ( <div>Perform ABC calibration every <strong>{pValue}</strong> days.</div>);
                            }} Value={this.props.Info.DaysPerABCCycle} OnSave={pValue=>this.state.saved.DaysPerABCCycle=pValue}/>   
                        <FormSliderField  
                            Min={this.props.Info.MinBaseline} 
                            Max={this.props.Info.MaxBaseline} 
                            Step={1} 
                            FormatCurrentValue={pValue=>{
                                return ( <div>Use <strong>{pValue}ppm</strong> as the baseline value for calibration.</div>);
                            }} Value={this.props.Info.Baseline} OnSave={pValue=>this.state.saved.Baseline=pValue}/>                                                                
                      </Form>                                      
                  </div>, 
                  "Enable ABC", 
                  ()=>true, 
                  ()=>true, 
                  ()=> {
                    if(!this.form.current.validate()) return false;
                    this.form.current.save();
                    CommandManager.executeCommand(false, "ENABLEABC", pData=> {
                        if(pData.Status=="true") {
                           this.setState({success: true});
                           if(this.props.OnChange!=undefined)
                              this.props.OnChange();
                        } else {
                            if(pData.Error!=undefined) {
                                this.setState({
                                    success: false,
                                    hasError: true,
                                    error: pData.Error
                                });
                            }
                        }
                     },
                     pCmd=>{                       
                       pCmd.Baseline =  this.state.saved.Baseline;
                       pCmd.ABCDaysPerCycle = this.state.saved.DaysPerABCCycle;
                     });
                     return true;
                  }),
                  new WizardStep(
                    ()=>
                        <div>
                            {this.state.success ?
                            <div class="w3-panel w3-pale-green w3-border">
                                <h3>Success</h3>
                                <p>Successfully enabled ABC.</p>
                            </div>  :
                         this.state.hasError && this.state.error!=undefined ?
                            <div>
                               <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error Enabling ABC</h3>
                                <p>{this.state.error}</p>
                                </div>
                            </div> : 
                         this.state.hasError ? 
                            <div>
                                <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error Enabling ABC</h3>
                                <p>An error occured enabling ABC.</p>
                                </div>
                            </div> :
                            <div>
                                <div class="w3-panel w3-pale-blue w3-border">
                                    <h3>Enabling ABC..</h3>
                                    <p>Please wait while ABC is enabled.</p>
                                </div>
                            </div>}
                        </div>, 
                    "Enabling ABC", 
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

export default EnableABCWizard;