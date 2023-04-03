import * as React from 'react';
import Button from '@mui/material/Button';
import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogContent from '@mui/material/DialogContent';
import DialogContentText from '@mui/material/DialogContentText';
import DialogTitle from '@mui/material/DialogTitle';
import Card from '@mui/material/Card';
import CardActions from '@mui/material/CardActions';
import CardContent from '@mui/material/CardContent';
import Grid from '@mui/material/Grid';
import Typography from '@mui/material/Typography';
import SetNetworkPriorityMenu from '../Components/SetNetworkPriorityMenu';
import MainView from '../../ViewModel/MainView';
import {IConfiguredNetwork} from '../../ViewModel/WirelessSettingsView';
import Box from '@mui/material/Box';
import { namespaces } from "../../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import Stack from '@mui/material/Stack';

export interface IConfiguredNetworkProps {
  network : IConfiguredNetwork;
  networks : IConfiguredNetwork[];
  onSetPriority :  (pNetwork : IConfiguredNetwork,pPriority : number) => void;
  onRemoveNetwork : (pNetwork : IConfiguredNetwork) => void;
}

export default function ConfiguredNetworkCard(props : IConfiguredNetworkProps) {
    const [confirmDeleteOpen, setConfirmDeleteOpen] = React.useState(false);
    const [confirmConnectOpen, setConfirmConnectOpen] = React.useState(false);
    const { t } = useTranslation(namespaces.settings);
    
    const handleDeleteClickOpen = () => {
      setConfirmDeleteOpen(true);
    };
  
    const handleDeleteClose = () => {
      setConfirmDeleteOpen(false);
    };

    const handleConfirmDelete = () => {
      setConfirmDeleteOpen(false);
      props.onRemoveNetwork(props.network);
    };
  
    return (
      <React.Fragment>
        <Card sx={{  height: "100%", display: "box", flexDirection: "column",minWidth: 200, minHeight: 150, backgroundColor:props.network.connection===undefined ? undefined :  "#f0f7f1"}}>
          {props.network.connection!==undefined ?
            <CardContent sx={{ width: "100%"}}>
              <Grid container  spacing={{ xs: 1, md: 1}} columns={{ xs: 2, md: 2 }} style={{ flexGrow: "1" , minWidth: "100%"}} >
                <Grid item xs={1} md={1}>
                  <Typography variant="h5" component="div">
                    { props.network.ssid}
                  </Typography>                
                </Grid>
                <Grid item xs={1} md={1}>
                  <Grid  container
                    sx={{ width: "100%"}}
                    direction="column"
                    alignItems="flex-end">
                      <Typography variant="caption">
                        Channel {props.network.connection.channel}
                      </Typography>  
                      <Typography variant="caption">
                        {MainView.Current.WirelessSettings.GetAuthModeFriendlyName(props.network.authMode)}
                      </Typography>                                     
                  </Grid>                                    
                </Grid>
                <Grid item xs={2} md={2}>
                  <Grid container spacing={{ xs: 2, md: 3}} columns={{ xs: 4, md: 6 }} sx={{ width: "100%"}}>                
                    <Grid item xs={2} md={2}  >
                      <Stack>
                        <Typography variant="caption">
                          IPv4 Address
                        </Typography> 
                        <Typography variant="body2">
                          {props.network.connection.ipv4Address}
                        </Typography> 
                      </Stack>        
                    </Grid> 
                    <Grid item xs={2} md={2} textAlign={{xs: "right", md: "center"}}  >
                      <Stack>
                        <Typography variant="caption">
                        IPv4 Netmask
                        </Typography> 
                        <Typography variant="body2">
                          {props.network.connection.ipv4Netmask}
                        </Typography> 
                      </Stack>        
                    </Grid>
                    <Grid item xs={2} md={2}  textAlign={{xs: "left", md: "right"}}>
                      <Stack>
                        <Typography variant="caption">
                        IPv4 Gateway
                        </Typography> 
                        <Typography variant="body2" color="secondary">
                          {props.network.connection.ipv4Gateway}
                        </Typography> 
                      </Stack>        
                    </Grid>  
                  </Grid>     
                </Grid>                           
              </Grid> 
            </CardContent> :          
            <CardContent>
              <Typography variant="h5" component="div">
              { props.network.ssid}
              </Typography>
              <Typography sx={{ mb: 1.5 }} color="text.secondary">
                {MainView.Current.WirelessSettings.GetAuthModeFriendlyName(props.network.authMode)}
              </Typography>            
            </CardContent> }
          <CardActions sx={{ mt: "auto" }}>
          { props.networks.length>1 && <SetNetworkPriorityMenu currentPriority={props.network.priority} onPriorityClick={pPriority => props.onSetPriority(props.network, pPriority)} 
                                                          priorities={MainView.Current.WirelessSettings.GetAvailableNetworkPriorities(props.network)} 
                                                          isCurrent={props.network.connection!==undefined}/> }
          <Box sx={{ flex: '1 1 auto' }} />                                                        
          {  props.network.connection===undefined && (<Button size="small" onClick={handleDeleteClickOpen}>{ t("buttons.remove") }</Button>) }
          </CardActions>
        </Card>
        <Dialog
          open={confirmDeleteOpen}
          onClose={handleDeleteClose}
          aria-labelledby="alert-dialog-title"
          aria-describedby="alert-dialog-description">
          <DialogTitle id="alert-dialog-title">
           { t("wireless.remove.title", { ssid: props.network.ssid}) }
          </DialogTitle>
          <DialogContent>
            <DialogContentText id="alert-dialog-description">
            { t("wireless.remove.para1") }
            </DialogContentText>
          </DialogContent>
          <DialogActions>
            <Button onClick={handleDeleteClose} autoFocus>{ t("buttons.cancel") }</Button>
            <Button onClick={handleConfirmDelete}>{ t("buttons.confirm") }
            </Button>
          </DialogActions>
        </Dialog>
      </React.Fragment>);
}