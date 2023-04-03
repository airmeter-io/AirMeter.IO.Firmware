import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import NetworkWifiIcon from '@mui/icons-material/NetworkWifi';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import WifiFindIcon from '@mui/icons-material/WifiFind';
import Fab from '@mui/material/Fab';
import AddIcon from '@mui/icons-material/Add';
import AddWirelessNetwork from './Dialogs/AddWirelessNetwork';
import * as React from 'react';
import Divider from '@mui/material/Divider';
import Paper from '@mui/material/Paper';
import MenuList from '@mui/material/MenuList';
import MenuItem from '@mui/material/MenuItem';
import ListItemText from '@mui/material/ListItemText';
import ListItemIcon from '@mui/material/ListItemIcon';
import {IConfiguredNetwork} from '../ViewModel/WirelessSettingsView';
import Button from '@mui/material/Button';
import MainView from '../ViewModel/MainView';
import { useEffect } from 'react';
import Backdrop from '@mui/material/Backdrop';
import CircularProgress from '@mui/material/CircularProgress';
import Grid from '@mui/material/Grid';
import SetNetworkPriorityMenu from './Components/SetNetworkPriorityMenu';
import ConfiguredNetworkCard from './Cards/ConfiguredNetworkCard';
function WirelessNetworks() {
  const [networks , setNetworks] = React.useState<IConfiguredNetwork[]>([]);
  const [loading , setLoading] = React.useState(false);
  const addRef: React.RefObject<AddWirelessNetwork> = React.createRef<AddWirelessNetwork>();
  const { t } = useTranslation(namespaces.settings);
  const handleFabClick = () => {
    addRef.current?.handleClickOpen();
  };

  const handleSetPriority = async (pNetwork : IConfiguredNetwork,pPriority : number) => {
    setLoading(true);
    var newNetworks = await MainView.Current.WirelessSettings.SetNetworkPriority(pNetwork, pPriority);
    setNetworks(newNetworks);
    setLoading(false);
  }

  const handleRemoveNetwork = async (pNetwork : IConfiguredNetwork) => {
    setLoading(true);
    var newNetworks = await MainView.Current.WirelessSettings.RemoveNetwork(pNetwork);
    setNetworks(newNetworks);
    setLoading(false);
  }

  const  reload = async () => {   
    setLoading(true);
    setNetworks(await MainView.Current.WirelessSettings.GetConfiguredNetworks());
    setLoading(false);
   // reloadNetworks();
  };

  useEffect(() => {
    reload();
  }, []);
  return (
    <Box>
      <Header title={t("wireless.networks.title")}>
        <MenuItem>
          <ListItemIcon>
            <AddIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Add Network</ListItemText>
        </MenuItem>
      </Header>
      <Container sx={{ width: 'auto', m: '1rem', mb: '4rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("wireless.breadcrumb"),
            icon: <NetworkWifiIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings/wireless"
          },          
          {
            title: t("wireless.networks.breadcrumb"),
            icon: <WifiFindIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em'}} color="text.primary">
        {t("wireless.networks.description")}
        </Typography>
        <Grid container spacing={{ xs: 2, md: 3}} columns={{ xs: 4, md: 8 }}>
        { networks.map((network, i) => 
          <Grid item xs={4} md={4}  >
            <ConfiguredNetworkCard 
              network={network} 
              networks={networks} 
              onSetPriority={handleSetPriority}
              onRemoveNetwork={handleRemoveNetwork}/>
          </Grid> )}
        <Backdrop
      sx={{ color: '#fff', zIndex: (theme) => theme.zIndex.drawer + 1 }}
      open={loading}>
      <CircularProgress color="inherit" />
    </Backdrop>  
       
    </Grid>  
     
      </Container>    
        <AddWirelessNetwork onClose={reload} ref={addRef} />
      <Footer/>
    </Box>);
}

export default WirelessNetworks;