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
        <Card sx={{  height: "100%", display: "flex", flexDirection: "column",minWidth: 275, minHeight: 150,m: 2, backgroundColor:props.network.connection===undefined ? undefined :  "#f0f7f1"}}>
          {props.network.connection!==undefined ?
            <CardContent sx={{ width: "100%"}}>
              <Grid wrap='nowrap'  container  spacing={1} style={{ flexGrow: "1" , minWidth: "100%"}} >
                <Grid item>
                  <Typography variant="h5" component="div">
                    { props.network.ssid}
                  </Typography>
                  <Typography sx={{ mb: 1.5 }} color="text.secondary">
                    {props.network.authMode}
                  </Typography>   
                  </Grid>
  
                <Grid item >
                  <Grid  container
                    direction="column"
                    alignItems="flex-end">
                        <Typography   variant="caption">
                        Channel: {props.network.connection.channel}
                      </Typography>                                       
                      <Typography variant="caption">
                        IPv4 Address: {props.network.connection.ip4Address}
                      </Typography>                                                               
                      <Typography variant="caption">
                        IPv4 Netmask: {props.network.connection.ip4Netmask}
                      </Typography>                                                                      
                      <Typography variant="caption">
                        IPv4 Gateway: {props.network.connection.ip4Gateway}
                      </Typography>  
                  </Grid>                                    
                </Grid>
              </Grid> 
            </CardContent> :          
            <CardContent>
              <Typography variant="h5" component="div">
              { props.network.ssid}
              </Typography>
              <Typography sx={{ mb: 1.5 }} color="text.secondary">
                {props.network.authMode}
              </Typography>            
            </CardContent> }
          <CardActions sx={{ mt: "auto" }}>
          { props.networks.length>1 && <SetNetworkPriorityMenu currentPriority={props.network.priority} onPriorityClick={pPriority => props.onSetPriority(props.network, pPriority)} 
                                                          priorities={MainView.Current.WirelessSettings.GetAvailableNetworkPriorities(props.network)} 
                                                          isCurrent={props.network.connection!==undefined}/> }
          <Box sx={{ flex: '1 1 auto' }} />                                                        
          {  props.network.connection===undefined && (<Button size="small" onClick={handleDeleteClickOpen}>{ t("buttons.remove") }</Button>) }
          {  props.network.connection===undefined && (<Button size="small">{ t("buttons.connect") }</Button>) }
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