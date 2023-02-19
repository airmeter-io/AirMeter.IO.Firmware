import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import PublishIcon from '@mui/icons-material/Publish';
import TextField from '@mui/material/TextField';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import FormLabel from '@mui/material/FormLabel';
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import FormHelperText from '@mui/material/FormHelperText';
import * as React from 'react';
import Switch from '@mui/material/Switch';
import Slider from '@mui/material/Slider';
import MainView from '../ViewModel/MainView';
import FormScreen from './Components/FormScreen';


function Cloud() {
  const [state, setState] = React.useState({
    enableMQTT: false,
    frequency: 0,
    serverAddress: "",
    topicPath: ""
  });
  const { t } = useTranslation(namespaces.settings);

  function valueLabelFormat(value: number) {    
    return MainView.Current.CloudSettings.FormatFrequency(value);
  }

  function handleSliderChange(event: Event, newValue: number | number[]) {
    if (typeof newValue === 'number') {
      setState({
        ...state,
        frequency: newValue,
      });
    }
  };

  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      [event.target.name]: event.target.checked,
    });
  };

  
  const handleSave = async () => {
    await MainView.Current.CloudSettings.SaveMqtt(state);
  };

  const handleLoad = async () => {
    setState(await MainView.Current.CloudSettings.LoadMqtt());
  }

  return (
    <Box>
      <Header title={t("cloud.title")}/>
      <Container maxWidth="sm" sx={{ width: '100%', m: '2rem' }}>
        <AppBreadcrumb breadcrumbs={[
            {
              title: t("breadcrumb"),
              icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
              to: "/settings"
            },
            {
              title: t("cloud.breadcrumb"),
              icon: <PublishIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
              to: null
            }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center', mt: '1em', mb: '1em'}} color="text.primary">
        {t("cloud.description")}
        </Typography>
        <FormScreen onLoad={handleLoad} onSave={handleSave} closePath="/settings">
          <FormGroup>
            <FormControlLabel
              control={
                <Switch checked={state.enableMQTT} onChange={handleChange} name="enableMQTT" />
              }
              label={t("cloud.mqtt.enableTitle")}/>        
          </FormGroup>
          <FormGroup>
            <TextField
                    autoFocus={ true}
                    disabled={ !state.enableMQTT}                    
                    margin="dense"
                    id="mqttServerAddress"
                    label={t("cloud.mqtt.serverAddress.title")}
                    type="url"
                    fullWidth
                    variant="standard"
                    value={state.serverAddress} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.serverAddress.helper")}</FormHelperText>
          </FormGroup>
          <FormGroup>
            <TextField
                    disabled={ !state.enableMQTT}                    
                    margin="dense"
                    id="mqttServerAddress"
                    label={t("cloud.mqtt.topicPath.title")}
                    type="text"
                    fullWidth
                    variant="standard"
                    value={state.topicPath} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.topicPath.helper")}</FormHelperText>
          </FormGroup>

          <FormGroup sx={{mt: "1em"}}>
              <FormLabel id="non-linear-slider"  disabled={ !state.enableMQTT}>
              {t("cloud.mqtt.frequencyLabel", {frequency: valueLabelFormat(state.frequency)})}              
              </FormLabel>
            <Box sx={{ width: "100%" }}>            
              <Slider
                disabled={ !state.enableMQTT}   
                value={state.frequency}
                min={0}
                step={1}
                max={ MainView.Current.CloudSettings.GetNumberFrequencies()-1}       
                getAriaValueText={valueLabelFormat}
                valueLabelFormat={valueLabelFormat}
                onChange={handleSliderChange}
                valueLabelDisplay="auto"
                aria-labelledby="non-linear-slider" />
            </Box>
          </FormGroup>  
        </FormScreen>                      
      </Container>
    <Footer/>
  </Box>);
}

export default Cloud;