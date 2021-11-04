import React from 'react';
import Wizard from '../popups/wizard';
import {WizardStep} from '../popups/wizard';
import CommandManager from '../commandManager';
import { PopupAction } from '../popups/popup';

class DisableABCWizard extends React.Component {
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
                        <div class="w3-panel w3-pale-red w3-border">
                            <h3>WARNING</h3>
                            <p>
                                For most situations disabling ABC will lead to less accurate readings. Please disable only if you
                                are confident that this is the right choice for the location that this unit will be used. 
                            </p>
                        </div>                                                       
                  </div>, 
                  "Disable ABC", 
                  ()=>true, 
                  ()=>true, 
                  ()=> {
                    CommandManager.executeCommand(false, "DISABLEABC", pData=> {
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
                     });
                     return true;
                  },
                  ()=>[
                      new PopupAction("Cancel", ()=>true,false)
                  ]),
                  new WizardStep(
                    ()=>
                        <div>
                            {this.state.success ?
                            <div class="w3-panel w3-pale-green w3-border">
                                <h3>Success</h3>
                                <p>Successfully disabled ABC.</p>
                            </div>  :
                         this.state.hasError && this.state.error!=undefined ?
                            <div>
                               <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error disabling ABC</h3>
                                <p>{this.state.error}</p>
                                </div>
                            </div> : 
                         this.state.hasError ? 
                            <div>
                                <div class="w3-panel w3-pale-yellow w3-border">
                                <h3>Error disabling ABC</h3>
                                <p>An error occured enabling ABC.</p>
                                </div>
                            </div> :
                            <div>
                                <div class="w3-panel w3-pale-blue w3-border">
                                    <h3>Disabling ABC..</h3>
                                    <p>Please wait while ABC is disabled.</p>
                                </div>
                            </div>}
                        </div>, 
                    "Disabling ABC", 
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

export default DisableABCWizard;