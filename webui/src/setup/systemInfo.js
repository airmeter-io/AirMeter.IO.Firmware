import React from 'react';

import CommandManager from '../commandManager.js';

class SystemInfoPanel extends React.Component {
    constructor(props) {
      super(props);
      this.state = {  info: {} };
 
     
    }

    loadData(pDone) {
        setTimeout(()=>{
            CommandManager.executeCommand(false, "SYSTEMINFO", pData=> {
              this.setState({ 
                info: {
                  MCUName: pData.MCUName,
                  CoreCount: pData.CoreCount,
                  FreeHeap: pData.FreeHeap,
                  LeastFreeHeap: pData.LeastFreeHeap
                }
              });
              if(pDone!=undefined)
                pDone();                      
            });  
        },
        1000);
          
      }
    

    componentDidMount () {
        this.loadData();       
        
    }
    componentWillUnmount () {

    }
  
    formatByteSize(pSize) {
        if(pSize > 1024*1024)
          return (pSize/(1024*1024)).toFixed(2)+"Mb";
        if(pSize > 1024)
          return (pSize/(1024)).toFixed(2)+"Kb";
        return pSize+"b";
    }


    render() {
      return (
        <div>
            <p>&nbsp;</p>
            <div class="w3-container">                               
                <div class="w3-card-4">
                  <header class="w3-container w3-light-grey">
                    <h3>Hardware</h3>                    
                  </header>
                  <div class="w3-container">
                    <p>
                      This <strong>{this.state.info.MCUName}</strong> based 
                      device has <strong>{this.state.info.CoreCount} cores</strong> and 
                      has <strong>{this.formatByteSize(this.state.info.FreeHeap)}</strong> free heap space with the lowest 
                      available since boot being <strong>{this.formatByteSize(this.state.info.LeastFreeHeap)}</strong>
                    </p>
                  </div>
                </div>

            </div>
        </div>            
      );
    }
  }

  export default SystemInfoPanel;