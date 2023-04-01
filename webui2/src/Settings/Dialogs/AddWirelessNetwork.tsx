import * as React from 'react';
import TextField from '@mui/material/TextField';
import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogContent from '@mui/material/DialogContent';
import DialogContentText from '@mui/material/DialogContentText';
import DialogTitle from '@mui/material/DialogTitle';
import Button from '@mui/material/Button';
import Grid from '@mui/material/Grid';
import StepperProcess from '../Components/StepperProcess';
import { useEffect } from 'react';
import MainView from '../../ViewModel/MainView';
import {ICalibrationResult} from '../../ViewModel/CalibrationSettingsView';
import { namespaces } from "../../i18n/i18n.constants";
import { withTranslation  } from "react-i18next";
import {i18n} from "../../i18n/i18n";
import Typography from '@mui/material/Typography';
import Box from '@mui/material/Box';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemText from '@mui/material/ListItemText';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemAvatar from '@mui/material/ListItemAvatar';
import ListItemIcon from '@mui/material/ListItemIcon';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import {IWirelessNetwork} from '../../ViewModel/WirelessSettingsView';
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import Switch from '@mui/material/Switch';

const t = i18n.getFixedT(null, namespaces.settings);
interface IAddWirelessNetworkState {
    open : boolean;
    networks: IWirelessNetwork[];
    index : number;
    makeDefault: boolean;
    password: string |undefined;
}

interface IAddWirelessNetworkProps {
   onClose : ()=>void
}

class AddWirelessNetwork extends React.Component<IAddWirelessNetworkProps,IAddWirelessNetworkState> {
  state = { open: false , networks: [], index: 0, makeDefault: true, password: undefined };

  public handleClickOpen()  {
    this.setState({
      ...this.state,
      open: true
    });
  }

  handleCheckedChange(event: React.ChangeEvent<HTMLInputElement>) {
    this.setState({
      ...this.state,
      [event.target.name]: event.target.checked,
    });
  };

  handlePasswordChange(event: React.ChangeEvent<HTMLInputElement>) {
    this.setState({
      ...this.state,
      password: event.target.value,
    });
  };
  

  private handleClose() {
    this.setState({
      ...this.state,
      open: false
    });
    this.props.onClose();
  }

  private handleSelect(
    index: number,
    event: React.MouseEvent<HTMLDivElement, MouseEvent>,

  ) {
    this.setState({
      ...this.state,
      index: index
    });
  }

  private steps = [
    {
      label: t("wireless.add.step1.label"),
      nextLabel: t("wireless.add.step1.nextLabel"),
      isOptional: () => false,
      executeNext: async ()=> {
        await MainView.Current.WirelessSettings.ScanForNetworks();
        this.setState( {
           open: true,
           networks: MainView.Current.WirelessSettings.GetNetworks()
        });
        return true;
      },
      onShow: () => {},
      view:  ()=>
    <React.Fragment>
      <Typography>
        <p>{t("wireless.add.step1.para1")}</p>
        <p>{t("wireless.add.step1.para2")}</p>
      </Typography>  
      
    </React.Fragment>
    },
    {
      label: t("wireless.add.step2.label"),
      nextLabel: t("wireless.add.step2.nextLabel"),
      isOptional: () => false,
      executeNext: async ()=> {
       
        return true;
      },
      onShow: () => {},
      view: ()=>
    <React.Fragment>
      <List>
        {this.state.networks.map((network : IWirelessNetwork, i) => 
          <ListItem>
            <ListItemButton selected={this.state.index === i} onClick={this.handleSelect.bind(this, i)}>
              <ListItemIcon><CheckCircleIcon /></ListItemIcon>
              <ListItemText primary={network.ssid} secondary={"MAC: "+network.apMacAddr+", Channel: "+network.channel+", Signal Strength: "+ network.signalStrength+ ", Auth Mode: "+MainView.Current.WirelessSettings.GetAuthModeFriendlyName(network.authMode)}/>
            </ListItemButton>

          </ListItem>)}         
          
      </List>
    </React.Fragment>
    },
    {
      label: t("wireless.add.step3.label"),
      nextLabel: t("wireless.add.step3.nextLabel"),
      isOptional: () => false,
      executeNext: async ()=> {
        if(!await MainView.Current.WirelessSettings.TestNetwork(this.state.networks[this.state.index], this.state.password)) 
          return false;
        await MainView.Current.WirelessSettings.AddNetwork(this.state.networks[this.state.index], this.state.makeDefault, this.state.password);
        return true;
      },
      onShow: () => {},
      view:  ()=>
    <React.Fragment>
      <Typography sx={{mt: 5}}>
        {t("wireless.add.step3.para1", {
          network: (this.state.networks[this.state.index] as IWirelessNetwork).ssid,
          authMode: MainView.Current.WirelessSettings.GetAuthModeFriendlyName((this.state.networks[this.state.index] as IWirelessNetwork).authMode), })}
      </Typography>     
      <TextField
              margin="dense"
              id="name"
              label={t("wireless.add.password")}
              type="password"
              fullWidth
              variant="standard"
              value={this.state.password}
              onChange={this.handlePasswordChange.bind(this)}
            />
      <Grid wrap='nowrap'  container  spacing={1} sx={{mt: "1em"}}>
        <Grid item>
        <FormGroup sx={{mt: "1em"}}>
            <FormControlLabel
               
              control={
                <Switch color="secondary" checked={this.state.makeDefault} onChange={this.handleCheckedChange.bind(this)} name="enablePowerSave" />
              }
              label={t("wireless.add.step3.makeDefault.label")}/>        
          </FormGroup>   
        </Grid>
      </Grid> 
    </React.Fragment>
    }
  ];


  private finishedView = (
    <Grid wrap='nowrap'  container  spacing={1}>
      <Grid item>
        <Typography>
            <p>{t("wireless.add.finished.para1")}</p>
            <p>{t("wireless.add.finished.para2")}</p>
            <p>{t("wireless.add.finished.para3")}</p>                         
        </Typography>
      </Grid>
      <Grid item>
             
      </Grid>
    </Grid>);



  render() {
    return (
      <div>
      <Dialog open={this.state.open} onClose={this.handleClose.bind(this)} fullWidth maxWidth={"xl"}>
            <DialogTitle>{t("wireless.add.title")}</DialogTitle>
            <DialogContent>
            <StepperProcess steps={this.steps} finishedView={this.finishedView} onClose={this.handleClose.bind(this)} resetLabel={t("calibration.manual.resetLabel")}/>
            </DialogContent>
           
          </Dialog>
          </div>);
  }
}

export default  AddWirelessNetwork;