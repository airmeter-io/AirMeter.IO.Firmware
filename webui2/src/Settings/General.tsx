import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import DisplaySettingsIcon from '@mui/icons-material/DisplaySettings';
import TextField from '@mui/material/TextField';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import FormHelperText from '@mui/material/FormHelperText';
import Switch from '@mui/material/Switch';
import * as React from 'react';
import AppBreadcrumb from '../AppBreadcrumb';
import FormScreen from './Components/FormScreen';
import MainView from '../ViewModel/MainView';

function General() {
  const [state, setState] = React.useState(
    {
      deviceName: "",
      enablePowerSaving: false
    }
  );
  const { t } = useTranslation(namespaces.settings);
  const handlePowerSaveChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      enablePowerSaving: event.target.checked,
    });
  };

  const handleDeviceNameChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      deviceName: event.target.value,
    });
  };
  
  const handleSave = async () => {
    await MainView.Current.GeneralSettings.Save(state);
  };

  const handleLoad = async () => {
    setState(await MainView.Current.GeneralSettings.Load());
  }

  var dev = "";

  return (
    <Box>
      <Header title={t("general.title")}/>
      <Container sx={{ width: 'auto', m: '1rem' }}>
        <AppBreadcrumb breadcrumbs={[
            {
              title: t("breadcrumb"),
              icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
              to: "/settings"
            },
            {
              title: t("general.breadcrumb"),
              icon: <DisplaySettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
              to: null
            }]}/>        

          <Typography sx={{ display: 'flex', alignItems: 'center', mt: '1em'}} color="text.primary">
        {t("general.description")}
        </Typography>
        <FormScreen onLoad={handleLoad} onSave={handleSave} closePath="/settings">
          <FormGroup>
            <TextField
                    autoFocus={ true}                                 
                    margin="dense"
                    id="deviceName"
                    label={t("general.deviceName.title")}
                    type="text"
                    fullWidth
                    variant="standard" 
                    value={state.deviceName}
                    onChange={handleDeviceNameChange}
                   />     
            <FormHelperText>{t("general.deviceName.helper")}</FormHelperText>
          </FormGroup>
          <FormGroup sx={{mt: "1em"}}>
            <FormControlLabel
              control={
                <Switch checked={state.enablePowerSaving} onChange={handlePowerSaveChange} name="enablePowerSave" />
              }
              label={t("general.powerSave.title")}/>        
          </FormGroup>             
        </FormScreen>           
      </Container>
      <Footer/>
    </Box>);
}

export default General;