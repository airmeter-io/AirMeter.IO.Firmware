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
import ValueSelect from './Components/ValueSelect';
import {IMQTTSettingsValues} from '../ViewModel/CloudSettingsView'
import { IValueReference } from '../ViewModel/ValueModel';

function Cloud() {
  const [state, setState] = React.useState<IMQTTSettingsValues>({
    enableMQTT: false,
    frequency: 0,
    serverAddress: "",
    username: "",
    password: "",
    readingsTopicPath: "",
    infoTopicPath: "",
    availableValues: [],
    readingsTopic: [],
    infoTopic: []
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

  const handleTextChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      [event.target.name]: event.target.value,
    });
  };

  const handleReadingsTopicChange = (values : IValueReference[]) => {
    setState({
      ...state,
      readingsTopic: values,
    });
  };

  const handleInfoTopicChange = (values : IValueReference[]) => {
    setState({
      ...state,
      infoTopic: values,
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
      <Container  sx={{ width: 'auto', m: '1rem', mb: '4rem' }}>
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
                    name="serverAddress"
                    onChange={handleTextChange}
                    label={t("cloud.mqtt.serverAddress.title")}
                    type="url"
                    fullWidth
                    variant="standard"
                    value={state.serverAddress} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.serverAddress.helper")}</FormHelperText>
          </FormGroup>
          <FormGroup>
            <TextField
                    autoFocus={ true}
                    disabled={ !state.enableMQTT}                    
                    margin="dense"
                    name="username"
                    onChange={handleTextChange}
                    label={t("cloud.mqtt.username.title")}
                    type="url"
                    fullWidth
                    variant="standard"
                    value={state.username} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.username.helper")}</FormHelperText>
          </FormGroup>
          <FormGroup>
            <TextField
                    autoFocus={ true}
                    disabled={ !state.enableMQTT}                    
                    margin="dense"
                    name="password"
                    onChange={handleTextChange}
                    label={t("cloud.mqtt.password.title")}
                    type="url"
                    fullWidth
                    variant="standard"
                    value={state.password} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.password.helper")}</FormHelperText>
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
          <FormGroup sx={{mt: "1em"}}>
              <FormLabel id="non-linear-slider"  disabled={ !state.enableMQTT}>
              {t("cloud.mqtt.readingsTopic.title")}              
              </FormLabel>
              <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.readingsTopic.helper")}</FormHelperText>
            <Box sx={{ width: "100%" }}>            
              <ValueSelect disabled={ !state.enableMQTT} availableValues={state.availableValues} setValues={state.readingsTopic} label="Readings Topic" onChanged={handleReadingsTopicChange} />
            </Box>
            <TextField
                    disabled={ !state.enableMQTT}                    
                    margin="dense"
                    name="readingsTopicPath"
                    onChange={handleTextChange}
                    label={t("cloud.mqtt.readingsTopicPath.title")}
                    type="text"
                    fullWidth
                    variant="standard"
                    value={state.readingsTopicPath} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.readingsTopicPath.helper")}</FormHelperText>
          </FormGroup>  
          <FormGroup sx={{mt: "1em"}}>
              <FormLabel id="non-linear-slider"  disabled={ !state.enableMQTT}>
              {t("cloud.mqtt.infoTopic.title")}              
              </FormLabel>
              <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.infoTopic.helper")}</FormHelperText>
            <Box sx={{ width: "100%" }}>            
              <ValueSelect disabled={ !state.enableMQTT} availableValues={state.availableValues} setValues={state.infoTopic} label="Info Topic" onChanged={handleInfoTopicChange} />
            </Box>
            <TextField
                    disabled={ !state.enableMQTT}                    
                    margin="dense"
                    name="infoTopicPath"
                    onChange={handleTextChange}
                    label={t("cloud.mqtt.infoTopicPath.title")}
                    type="text"
                    fullWidth
                    variant="standard"
                    value={state.infoTopicPath} />     
            <FormHelperText disabled={ !state.enableMQTT} >{t("cloud.mqtt.infoTopicPath.helper")}</FormHelperText>
          </FormGroup>  
        </FormScreen>                      
      </Container>
    <Footer/>
  </Box>);
}

export default Cloud;