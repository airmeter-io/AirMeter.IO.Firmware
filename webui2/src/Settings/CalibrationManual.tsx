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
import TuneIcon from '@mui/icons-material/Tune';
import Stepper from '@mui/material/Stepper';
import Step from '@mui/material/Step';
import StepLabel from '@mui/material/StepLabel';
import * as React from 'react';
import Button from '@mui/material/Button';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemText from '@mui/material/ListItemText';
import ListItemIcon from '@mui/material/ListItemIcon';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import CO2RadialGauge from '../Meter/CO2RadialGauge';
import Grid from '@mui/material/Grid';

const steps = ['Place device outside', 'Wait for readings to settle', 'Finalise Calibration'];
function CalibrationManual() {
  const { t } = useTranslation(namespaces.settings);
  const [activeStep, setActiveStep] = React.useState(0);
  const [skipped, setSkipped] = React.useState(new Set<number>());
  const [co2, setCo2] = React.useState(400);
  const isStepOptional = (step: number) => {
    return step === 1;
  };

  const isStepSkipped = (step: number) => {
    return skipped.has(step);
  };

  const handleNext = () => {
    let newSkipped = skipped;
    if (isStepSkipped(activeStep)) {
      newSkipped = new Set(newSkipped.values());
      newSkipped.delete(activeStep);
    }

    setActiveStep((prevActiveStep) => prevActiveStep + 1);
    setSkipped(newSkipped);
  };

  const handleBack = () => {
    setActiveStep((prevActiveStep) => prevActiveStep - 1);
  };

  const handleSkip = () => {
    if (!isStepOptional(activeStep)) {
      // You probably want to guard against something like this,
      // it should never occur unless someone's actively trying to break something.
      throw new Error("You can't skip a step that isn't optional.");
    }

    setActiveStep((prevActiveStep) => prevActiveStep + 1);
    setSkipped((prevSkipped) => {
      const newSkipped = new Set(prevSkipped.values());
      newSkipped.add(activeStep);
      return newSkipped;
    });
  };

  const handleReset = () => {
    setActiveStep(0);
  };

  const getStepView = (step : number) => {
    switch(step) {
      case 0 : 
        return (<React.Fragment>
                  <Typography>
                  <p>Outside air that is away from sources of pollution has a relatively constant CO2 just above 400ppm. The CO2 sensor can use this to calibrate against as an approximate baseline value. Before calibration is performed it must be placed outside.</p>

<p>The weather conditions must be dry, not excessively hot and the sensor should be placed away from:-</p>

                  </Typography>  
                  <List>
                    <ListItem>
                      <ListItemIcon><CheckCircleIcon /></ListItemIcon>
                      <ListItemText primary="Sources of pollution/CO2 such as motor vehicles, home heating boilers/burners, fires or any other combustion." />
                    </ListItem>
                    <ListItem>
                      <ListItemIcon><CheckCircleIcon /></ListItemIcon>
                      <ListItemText primary="Sources of heat, sunlight or infrared radiation." />
                    </ListItem>
                    <ListItem>
                      <ListItemIcon><CheckCircleIcon /></ListItemIcon>
                      <ListItemText primary="People who are also sources of CO2, this includes yourself!" />
                    </ListItem>
                    <ListItem>
                      <ListItemIcon><CheckCircleIcon /></ListItemIcon>
                      <ListItemText primary="Excessive humidity (&gt; 80%), precipitation such as rain or standing water." />
                    </ListItem>
                  </List>
                </React.Fragment>);
      case 1 :
        return (
        <React.Fragment>
          <Grid wrap='nowrap'  container  spacing={1}>
            <Grid item>
              <Typography>
                    <p>After ensuring that the sensor is located outside as per the instructions on the previous screen. you must now wait for the sensor readings to stablise before you can complete the calibration process. This is because the reading taken during calibration will be assumed to represent the ambient CO2 concentration level of outdoor air (E.g. around 400ppm).</p>
                    <p>During this time leave the sensor in position in the outdoor air and ensure that it is not obstructed or interfered with.</p>
                    <p>Wait will be complete in 00:00:00 after which you will be able to press Next to start the calibration.</p>
              </Typography>
            </Grid>

            <Grid item>
              <Box sx={{maxWidth: "200px", minHeight: "200px"}}>
                <CO2RadialGauge  Value={co2}/>                 
              </Box>        
            </Grid>
          </Grid>
        </React.Fragment>);
      default : 
        return (<React.Fragment>
                  <Typography>
                    Calibrate
                  </Typography>        
                </React.Fragment>);

    }
  }

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
        <Box sx={{ width: '100%', minWidth: "600px", mt: "2em" }}>
          <Stepper activeStep={activeStep}>
            {steps.map((label, index) => {
              const stepProps: { completed?: boolean } = {};
              const labelProps: {
                optional?: React.ReactNode;
              } = {};
              
              if (isStepSkipped(index)) {
                stepProps.completed = false;
              }
              return (
                <Step key={label} {...stepProps}>
                  <StepLabel {...labelProps}>{label}</StepLabel>
                </Step>
              );
            })}
          </Stepper>
          {activeStep === steps.length ? (
            <React.Fragment>
              <Box sx={{ mt: 2, mb: 1 }}>
                <Grid wrap='nowrap'  container  spacing={1}>
                  <Grid item>
                    <Typography>
                          <p>The CO2 sensor calibration sequence has been completed successfully. When the readings are next updated you will see the newly calibrated value.</p>
                          <p>To ensure the sensor is fully calibrated check it outside at different times of the day. If the readings drop below 400ppm then the sensor may benefit from another calibration.</p>
                          <p>Note that CO2 levels do fluctuate outdoors therefore some trial and error diagnostics is required to find the optimum time of day & location for calibration.</p>                          
                    </Typography>
                  </Grid>

                  <Grid item>
                    <Box sx={{maxWidth: "200px", minHeight: "200px"}}>
                      <CO2RadialGauge  Value={co2}/>                 
                    </Box>        
                  </Grid>
                </Grid>
              </Box>
              <Box sx={{ display: 'flex', flexDirection: 'row', pt: 2 }}>
                <Box sx={{ flex: '1 1 auto' }} />
                <Button onClick={handleReset}>Reset</Button>
              </Box>
            </React.Fragment>
          ) : (
            <React.Fragment>
              <Box sx={{ mt: 2, mb: 1 }}>
                {getStepView(activeStep)}
                
              </Box>

             
            
              <Box sx={{ display: 'flex', flexDirection: 'row', pt: 2 }}>
                <Button
                  color="inherit"
                  disabled={activeStep === 0}
                  onClick={handleBack}
                  sx={{ mr: 1 }}>
                  Back
                </Button>
                <Box sx={{ flex: '1 1 auto' }} />
                {isStepOptional(activeStep) && (
                  <Button color="inherit" onClick={handleSkip} sx={{ mr: 1 }}>
                    Skip
                  </Button>
                )}
                <Button onClick={handleNext}>
                  {activeStep === steps.length - 1 ? 'Finish' : 'Next'}
                </Button>
              </Box>
            </React.Fragment>
          )}
        </Box> 
      </Container>
      <Footer/>
    </Box>);
}

export default CalibrationManual;