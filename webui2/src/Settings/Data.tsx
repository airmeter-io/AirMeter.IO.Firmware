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
import FormControl from '@mui/material/FormControl';
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import FormHelperText from '@mui/material/FormHelperText';
import * as React from 'react';
import Switch from '@mui/material/Switch';
import Button from '@mui/material/Button';
import Stack from '@mui/material/Stack';
import Slider from '@mui/material/Slider';

interface MQTTFrequency {
   seconds: number;
   label: string;
}

function Data() {
  const [state, setState] = React.useState({
    enableMQTT: true,
    frequencyValue: 0
  });
  const { t } = useTranslation(namespaces.settings);

  const frequencies : MQTTFrequency[] = [
    {
      seconds: 30,
      label: t("frequencies.thirtySeconds")
    },    
    {
       seconds: 60,
       label: t("frequencies.minute")
    },
    {
      seconds: 60*2,
      label: t("frequencies.twoMinutes")
    },
    {
      seconds: 60*3,
      label: t("frequencies.threeMinutes")
    },
    {
      seconds: 60*4,
      label: t("frequencies.fourMinutes")
    },            
    {
      seconds: 60*5,
      label: t("frequencies.fiveMinutes")
    },
    {
      seconds: 60*10,
      label: t("frequencies.tenMinutes")
    },
    {
      seconds: 60*15,
      label: t("frequencies.fifteenMinutes")
    },
    {
      seconds: 60*30,
      label: t("frequencies.thirtyMinutes")
    },
    {
      seconds: 60*60,
      label: t("frequencies.hour")
    },  
  ]

  function valueLabelFormat(value: number) {    
    return  frequencies[value].label;
  }

  function handleSliderChange(event: Event, newValue: number | number[]) {
    if (typeof newValue === 'number') {
      setState({
        ...state,
        frequencyValue: newValue,
      });
    }
  };

  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      [event.target.name]: event.target.checked,
    });
  };

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
      <FormControl component="fieldset" variant="standard"  sx={{minWidth: "500px"}}>                  
        <FormGroup sx={{mt: "1em"}}>
             <FormLabel id="non-linear-slider"  disabled={ !state.enableMQTT}>
             {t("data.frequency.label", {frequency: valueLabelFormat(state.frequencyValue)})}              
            </FormLabel>
          <Box sx={{ width: "100%" }}>            
            <Slider
              sx={{minWidth: "600px"}}
              disabled={ !state.enableMQTT}   
              value={state.frequencyValue}
              min={0}
              step={1}
              max={frequencies.length-1}       
              getAriaValueText={valueLabelFormat}
              valueLabelFormat={valueLabelFormat}
              onChange={handleSliderChange}
              valueLabelDisplay="auto"
              aria-labelledby="non-linear-slider" />
          </Box>
        </FormGroup>

        <FormGroup sx={{mt: "1em"}}>
          <Stack spacing={2} direction="row">
            <Button variant="contained">{t("buttons.save")}</Button>
          </Stack>
        </FormGroup>        
      </FormControl> 
    </Container>
    <Footer/>
  </Box>);
}

export default Data;