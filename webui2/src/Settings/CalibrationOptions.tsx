import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import CompassCalibrationIcon from '@mui/icons-material/CompassCalibration';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import * as React from 'react';
import MainView from '../ViewModel/MainView';
import FormScreen from './Components/FormScreen';
import FormLabel from '@mui/material/FormLabel';
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import FormHelperText from '@mui/material/FormHelperText';
import TextField from '@mui/material/TextField';
import Switch from '@mui/material/Switch';
import Slider from '@mui/material/Slider';

function CalibrationOptions() {
  const [state, setState] = React.useState({
    enableAbc: false,
    abcFrequency: 0,
    minAbcFrequency: 1,
    maxAbcFrequency: 240,
    stepAbcFrequency: 1,
    baseline: 0,
    minBaseline: 400,
    maxBaseline: 500,
    stepBaseline: 5
  });
  const { t } = useTranslation(namespaces.settings);
  const handleCheckedChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      [event.target.name]: event.target.checked,
    });
  };

  
  function frequencyValueFormat(value: number) {    
    return MainView.Current.CalibrationSettings.FormatFrequency(value);
  }

  function baselineValueFormat(value: number) {    
    return value.toString();
  }

  function handleFrequencyChange(event: Event, newValue: number | number[]) {
    if (typeof newValue === 'number') {
      setState({
        ...state,
        abcFrequency: newValue,
      });
    }
  };

  function handleBaselineChange(event: Event, newValue: number | number[]) {
    if (typeof newValue === 'number') {
      setState({
        ...state,
        baseline: newValue,
      });
    }
  };

  const handleSave = async () => {
    await MainView.Current.CalibrationSettings.Save(state);
  };

  const handleLoad = async () => {
    setState(await MainView.Current.CalibrationSettings.Load());
  }
  return (
    <Box>
      <Header title={t("calibration.options.title")}/>
      <Container maxWidth="sm" sx={{ width: '100%', m: '2rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("calibration.breadcrumb"),
            icon: <CompassCalibrationIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings/calibration"
          },
          {
            title: t("calibration.options.breadcrumb"),
            icon: <RestartAltIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em' }} color="text.primary">
        {t("calibration.options.description")}
        </Typography>  
        <FormScreen onLoad={handleLoad} onSave={handleSave} closePath="/settings/calibration">
          <FormGroup>
            <FormControlLabel
              control={
                <Switch checked={state.enableAbc} onChange={handleCheckedChange} name="enableAbc" />
              }
              label={t("calibration.options.enableAbc.title")}/>
            <FormHelperText>{t("calibration.options.enableAbc.helper")}</FormHelperText>        
          </FormGroup>

          <FormGroup sx={{mt: "1em"}}>
              <FormLabel id="non-linear-slider"  disabled={ !state.enableAbc}>
              {t("calibration.options.frequency.label", {frequency: frequencyValueFormat(state.abcFrequency)})}              
              </FormLabel>
              
            <Box sx={{ width: "100%" }}>            
              <Slider
                disabled={ !state.enableAbc}   
                value={state.abcFrequency}
                min={state.minAbcFrequency}
                step={state.stepAbcFrequency}
                max={ state.maxAbcFrequency}       
                getAriaValueText={frequencyValueFormat}
                valueLabelFormat={frequencyValueFormat}
                onChange={handleFrequencyChange}
                valueLabelDisplay="auto"
                aria-labelledby="non-linear-slider" />
            </Box>
            <FormHelperText>{t("calibration.options.frequency.helper")}</FormHelperText> 
          </FormGroup>  
          <FormGroup sx={{mt: "1em"}}>
              <FormLabel id="non-linear-slider"  disabled={ !state.enableAbc}>
              {t("calibration.options.baseline.label", {baseline: baselineValueFormat(state.baseline)})}              
              </FormLabel>
              
            <Box sx={{ width: "100%" }}>            
              <Slider
                disabled={ !state.enableAbc}   
                value={state.baseline}
                min={state.minBaseline}
                step={state.stepBaseline}
                max={ state.maxBaseline}       
                getAriaValueText={baselineValueFormat}
                valueLabelFormat={baselineValueFormat}
                onChange={handleBaselineChange}
                valueLabelDisplay="auto"
                aria-labelledby="non-linear-slider" />
            </Box>
            <FormHelperText>{t("calibration.options.baseline.helper")}</FormHelperText> 
          </FormGroup>  
        </FormScreen>  
      </Container>
      <Footer/>
    </Box>);
}

export default CalibrationOptions;