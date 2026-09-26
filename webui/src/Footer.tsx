import BottomNavigation from '@mui/material/BottomNavigation';
import BottomNavigationAction from '@mui/material/BottomNavigationAction';
import { Link } from 'react-router-dom';

import SpeedIcon from '@mui/icons-material/Speed';
import ShowChartIcon from '@mui/icons-material/ShowChart';
import SettingsIcon from '@mui/icons-material/Settings';
import Paper from '@mui/material/Paper';
import { useLocation } from 'react-router-dom';
function Footer() {
 
  let location = useLocation();
  return (
    <Paper sx={{ position: 'fixed', bottom: 0, left: 0, right: 0 }} elevation={3}>
        <BottomNavigation
            showLabels
            value={location.pathname=="/"? "/" : "/"+location.pathname.split("/")[1] }>
          <BottomNavigationAction label="Meter" icon={<SpeedIcon />} value="/" component={Link} to="/"/>
          <BottomNavigationAction label="Charts" icon={<ShowChartIcon />} value="/charts" component={Link} to="/charts"/>
          <BottomNavigationAction label="Settings" icon={<SettingsIcon />} value="/settings" component={Link} to="/settings"/>
      </BottomNavigation>
    </Paper>    
  );
}

export default Footer;