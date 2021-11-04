import React from 'react';
import CalibrationPanel from './calibration.js';
import SettingsPanel from './settings.js';
import SystemInfoPanel from './systemInfo.js';
import NetworkInfoPanel from './networkInfo.js';
import DataPanel from './data.js';



  

  class SetupPageContent extends React.Component {
    constructor(props) {
      super(props);
    }

    
    render() {
      
      return [
        <SettingsPanel/>,
        <NetworkInfoPanel/>,
        <CalibrationPanel/>,
        <SystemInfoPanel/>,
        <DataPanel />
      ];
    }
  }



export default SetupPageContent;

