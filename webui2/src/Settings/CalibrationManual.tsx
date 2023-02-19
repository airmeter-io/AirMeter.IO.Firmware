import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import CompassCalibrationIcon from '@mui/icons-material/CompassCalibration';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import { useNavigate } from "react-router-dom";
import AppBreadcrumb from '../AppBreadcrumb';
import TuneIcon from '@mui/icons-material/Tune';
import * as React from 'react';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemText from '@mui/material/ListItemText';
import ListItemIcon from '@mui/material/ListItemIcon';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import CO2RadialGauge from '../Meter/CO2RadialGauge';
import Grid from '@mui/material/Grid';
import StepperProcess from './Components/StepperProcess';
import { useEffect } from 'react';
import MainView from '../ViewModel/MainView';
import {ICalibrationResult} from '../ViewModel/CalibrationSettingsView';

function CalibrationManual() {
  const { t } = useTranslation(namespaces.settings);
  const [ displayValues, setDisplayValues ] = React.useState({co2: 400, waitSeconds: 60});
  const [ timeString, setTimeString ] = React.useState("");
  const [ calibrationResult, setCalibrationResult ] = React.useState<ICalibrationResult>(
    {
      success: true,
      message: undefined
    }
  );
 
  


  const toHourMinsSecs = (pSecs : number) : string => {   
    var hours   = Math.floor(pSecs / 3600);
    var minutes = Math.floor((pSecs - (hours * 3600)) / 60);
    var seconds = Math.floor(pSecs - (hours * 3600) - (minutes * 60));

    return (hours < 10 ? "0"+hours.toString() : hours.toString())+':'+
           (minutes < 10 ? "0"+ minutes.toString() : minutes.toString())+':'+
           (seconds < 10 ? "0"+ seconds.toString() : seconds.toString());
  }

 
  
  const updateTimeString = () => setTimeString(toHourMinsSecs(MainView.Current.CalibrationSettings.GetRemainingSeconds()));

  var tickCount = 0;
  const tick = async () => {
    updateTimeString();
    if(tickCount % 10 === 0) {
      tickCount++;
      var values = await MainView.Current.CalibrationSettings.GetDisplayValues();
      setDisplayValues(values);
    } else
      tickCount++;
  }

  useEffect(() => {
    tickCount = 0;
    var interval = setInterval(tick, 1000);  
    return () => {
      clearInterval(interval);      
    }
  }, []);

  const tips = [
    t("calibration.manual.step1.tip1"),
    t("calibration.manual.step1.tip2"),
    t("calibration.manual.step1.tip3"),
    t("calibration.manual.step1.tip4")
  ]
  const steps = [
    {
      label: t("calibration.manual.step1.label"),
      nextLabel: t("calibration.manual.step1.nextLabel"),
      isOptional: () => false,
      executeNext: async ()=> {
        console.log("resetting start time");
        MainView.Current.CalibrationSettings.ResetWait();
        updateTimeString();
        return true;
      },
      onShow: () => {},
      view: 
    <React.Fragment>
      <Typography>
        <p>{t("calibration.manual.step1.para1")}</p>
        <p>{t("calibration.manual.step1.para2")}</p>
      </Typography>  
      <List dense>
        {tips.map((tip) => <ListItem><ListItemIcon><CheckCircleIcon /></ListItemIcon><ListItemText primary={tip} /></ListItem>)}        
      </List>
    </React.Fragment>
    },
    {
      label: t("calibration.manual.step2.label"),
      nextLabel: t("calibration.manual.step2.nextLabel"),
      isOptional: () => true,
      executeNext: async ()=> {
        setCalibrationResult({
          success: true,
          message: undefined
        });
        console.log("trying next"+MainView.Current.CalibrationSettings.GetRemainingSeconds());
        return MainView.Current.CalibrationSettings.GetRemainingSeconds()<=0;
      },
      onShow: () => MainView.Current.CalibrationSettings.ResetWait(),
      view: 
    <React.Fragment>
      <Grid wrap='nowrap'  container  spacing={1}>
        <Grid item>
          <Typography>
                <p>{t("calibration.manual.step2.para1")}</p>
                <p>{t("calibration.manual.step2.para2")}</p>
                <p>{t("calibration.manual.step2.para3", { time: timeString })}</p>
          </Typography>
        </Grid>

        <Grid item>
          <Box sx={{maxWidth: "200px", minHeight: "200px"}}>
            <CO2RadialGauge  Value={displayValues.co2}/>                 
          </Box>        
        </Grid>
      </Grid>
    </React.Fragment>
    },
    {
      label: t("calibration.manual.step3.label"),
      nextLabel: t("calibration.manual.step3.nextLabel"),
      isOptional: () => false,
      executeNext: async ()=> {
        var result = await MainView.Current.CalibrationSettings.Calibrate();

        setCalibrationResult(result);
        return result.success;
      },
      onShow: () => {},
      view: 
    <React.Fragment>
      <Grid wrap='nowrap'  container  spacing={1}>
        <Grid item>
          <Typography>
            {calibrationResult.success? 
              <React.Fragment>
                <p>{t("calibration.manual.step3.para1")}</p>
                <p>{t("calibration.manual.step3.para2")}</p>
              </React.Fragment>:
              <React.Fragment>
                <p>{t("calibration.manual.step3.failed.para1")}</p>
                <p>{t("calibration.manual.step3.failed.para2")} <strong>{calibrationResult.message}</strong></p>
              </React.Fragment>}
          </Typography>
        </Grid>

        <Grid item>
          <Box sx={{maxWidth: "200px", minHeight: "200px"}}>
            <CO2RadialGauge  Value={displayValues.co2}/>                 
          </Box>        
        </Grid>
      </Grid>    
    </React.Fragment>
    }
  ];

  const finishedView = (
    <Grid wrap='nowrap'  container  spacing={1}>
      <Grid item>
        <Typography>
            <p>{t("calibration.manual.finished.para1")}</p>
            <p>{t("calibration.manual.finished.para2")}</p>
            <p>{t("calibration.manual.finished.para3")}</p>                         
        </Typography>
      </Grid>

      <Grid item>
        <Box sx={{maxWidth: "200px", minHeight: "200px"}}>
          <CO2RadialGauge  Value={displayValues.co2}/>                 
        </Box>        
      </Grid>
    </Grid>);


  const navigate = useNavigate();
  const handleClose = () => navigate('/settings/calibration');

  return (
    <Box>
      <Header title={t("calibration.manual.title")}/>
      <Container maxWidth="md" sx={{ width: '100%', m: '2rem' }}>
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
            title: t("calibration.manual.breadcrumb"),
            icon: <TuneIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em' }} color="text.primary">
        {t("calibration.manual.description")}
        </Typography> 
        <StepperProcess steps={steps} finishedView={finishedView} onClose={handleClose} resetLabel={t("calibration.manual.resetLabel")}/>

      </Container>
      <Footer/>
    </Box>);
}

export default CalibrationManual;