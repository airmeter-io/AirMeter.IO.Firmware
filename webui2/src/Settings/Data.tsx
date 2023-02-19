import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import PublishIcon from '@mui/icons-material/Publish';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import FormLabel from '@mui/material/FormLabel';
import FormGroup from '@mui/material/FormGroup';
import * as React from 'react';
import Slider from '@mui/material/Slider';
import MainView from '../ViewModel/MainView';
import FormScreen from './Components/FormScreen';

function Data() {
  const [state, setState] = React.useState({
    frequency: 0
  });
  const { t } = useTranslation(namespaces.settings);


  function valueLabelFormat(value: number) {    
    return MainView.Current.DataSettings.FormatFrequency(value);
  }

  function handleSliderChange(event: Event, newValue: number | number[]) {
    if (typeof newValue === 'number') {
      setState({
        ...state,
        frequency: newValue,
      });
    }
  };

  const handleSave = async () => {
    await MainView.Current.DataSettings.Save(state);
  };

  const handleLoad = async () => {
    setState(await MainView.Current.DataSettings.Load());
  }


  return (
    <Box>
      <Header title={t("data.title")}/>
      <Container maxWidth="md" sx={{ width: '100%', m: '2rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("data.breadcrumb"),
            icon: <PublishIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
      <Typography sx={{ display: 'flex', alignItems: 'center', mt: '1em', mb: '1em'}} color="text.primary">
      {t("data.description")}
      </Typography>
      <FormScreen onLoad={handleLoad} onSave={handleSave} closePath="/settings">                
        <FormGroup sx={{mt: "1em"}}>
             <FormLabel id="non-linear-slider"  >
             {t("data.frequency.label", {frequency: valueLabelFormat(state.frequency)})}              
            </FormLabel>
          <Box sx={{ width: "100%" }}>            
            <Slider
              sx={{minWidth: "600px"}}
              value={state.frequency}
              min={0}
              step={1}
              max={MainView.Current.DataSettings.GetNumberFrequencies()-1}       
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

export default Data;