import * as React from 'react';
import * as ReactDOM from 'react-dom/client';
import Live from './Meter/Live';
import Charts from './Charts';
import Settings from './Settings/Settings';
import General from './Settings/General';
import Wireless from './Settings/Wireless';
import "./i18n/i18n";
import {
  createHashRouter,
  RouterProvider,
} from "react-router-dom";
import Calibration from './Settings/Calibration';
import Cloud from './Settings/Cloud';
import Data from './Settings/Data';
import Ntp from './Settings/Ntp';
import Users from './Settings/Users';
import CalibrationManual from './Settings/CalibrationManual';
import WirelessNetworks from './Settings/WirelessNetworks';
import WirelessOptions from './Settings/WirelessOptions';
import CalibrationOptions from './Settings/CalibrationOptions';


const router = createHashRouter([
  {
    path: "/",
    element: <Live/>,
  },
  {
    path: "/charts",
    element: <Charts/>,
  },
  {
    path: "/settings",
    element: <Settings/>,
  },  
  {
    path: "/settings/general",
    element: <General/>,
  },
  {
    path: "/settings/wireless",
    element: <Wireless/>,
  },
  {
    path: "/settings/wireless/networks",
    element: <WirelessNetworks/>,
  },
  {
    path: "/settings/wireless/options",
    element: <WirelessOptions	/>,
  },
  {
    path: "/settings/calibration",
    element: <Calibration/>,
  },
  {
    path: "/settings/calibration/manual",
    element: <CalibrationManual/>,
  },
  {
    path: "/settings/calibration/options",
    element: <CalibrationOptions/>,
  },    
  {
    path: "/settings/cloud",
    element: <Cloud/>,
  },
  {
    path: "/settings/data",
    element: <Data/>,
  },
  {
    path: "/settings/ntp",
    element: <Ntp/>,
  },
  {
    path: "/settings/users",
    element: <Users/>,
  },
]);

const rootElement = document.getElementById('root');
if (!rootElement) throw new Error('Failed to find the root element');

ReactDOM.createRoot(rootElement).render(
  <React.StrictMode>
    <RouterProvider router={router} />
  </React.StrictMode>
);