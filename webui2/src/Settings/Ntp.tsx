import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import TimelapseIcon from '@mui/icons-material/Timelapse';
import TextField from '@mui/material/TextField';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import FormHelperText from '@mui/material/FormHelperText';
import Switch from '@mui/material/Switch';
import * as React from 'react';
import AppBreadcrumb from '../AppBreadcrumb';
import MainView from '../ViewModel/MainView';
import FormScreen from './Components/FormScreen';

function Ntp() {
  const [state, setState] = React.useState({
    enableDhcp: false,
    ntp1: "",
    ntp2: ""
  });
  const { t } = useTranslation(namespaces.settings);
  const handleDhcpChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      [event.target.name]: event.target.checked,
    });
  };

  const handleSave = async () => {
    await MainView.Current.TimeSettings.Save(state);
  };

  const handleLoad = async () => {
    setState(await MainView.Current.TimeSettings.Load());
  }

  return (
    <Box>
      <Header title={t("ntp.title")}/>
      <Container maxWidth="sm" sx={{ width: '100%', m: '2rem' }}>
        <AppBreadcrumb breadcrumbs={[
            {
              title: t("breadcrumb"),
              icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
              to: "/settings"
            },
            {
              title: t("ntp.breadcrumb"),
              icon: <TimelapseIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
              to: null
            }]}/>        

          <Typography sx={{ display: 'flex', alignItems: 'center', mt: '1em'}} color="text.primary">
        {t("ntp.description")}
        </Typography>
        <FormScreen onLoad={handleLoad} onSave={handleSave} closePath="/settings">
          <FormGroup>
            <TextField
                    autoFocus={ true}                                 
                    margin="dense"
                    id="ns1"
                    label={t("ntp.ntp1.title")}
                    type="text"
                    fullWidth
                    variant="standard"
                    value = {state.ntp1} />     
            <FormHelperText>{t("ntp.ntp1.helper")}</FormHelperText>
          </FormGroup>
          <FormGroup>
            <TextField                              
                    margin="dense"
                    id="ns2"
                    label={t("ntp.ntp2.title")}
                    type="text"
                    fullWidth
                    variant="standard"
                    value={state.ntp2} />     
            <FormHelperText>{t("ntp.ntp2.helper")}</FormHelperText>
          </FormGroup>
          <FormGroup sx={{mt: "1em"}}>
            <FormControlLabel
              control={
                <Switch checked={state.enableDhcp} onChange={handleDhcpChange} name="enableDhcp" />
              }
              label={t("ntp.dhcp.title")}/>        
          </FormGroup>    
        </FormScreen>                
      </Container>
      <Footer/>
    </Box>);
}

export default Ntp;