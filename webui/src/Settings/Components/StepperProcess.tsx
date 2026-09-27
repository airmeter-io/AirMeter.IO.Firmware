import Box from '@mui/material/Box';
import { namespaces } from "../../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import Stepper from '@mui/material/Stepper';
import Step from '@mui/material/Step';
import StepLabel from '@mui/material/StepLabel';
import * as React from 'react';
import Button from '@mui/material/Button';
import LoadingButton from '@mui/lab/LoadingButton';
import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import CloseIcon from '@mui/icons-material/Close';
import CancelIcon from '@mui/icons-material/Cancel';

interface IStepperProcessStep {
  label : string;
  nextLabel: string;
  isOptional : () => boolean;
  executeNext: () => Promise<boolean>;
  onShow: () => void;
  view : ()=> React.ReactElement<any>;
}

interface IStepperProcessProps {
  steps : IStepperProcessStep[];
  finishedView : React.ReactElement<any>;
  onClose: () => void;
  resetLabel : string;
}

function StepperProcess(props : IStepperProcessProps) {
  const { t } = useTranslation(namespaces.settings);
  const [activeStep, setActiveStep] = React.useState(0);
  const [skipped, setSkipped] = React.useState(new Set<number>());
  const [isNextLoading, setIsNextLoading] = React.useState(false);
  const isStepOptional = (step: number) => props.steps[step].isOptional();


  const isStepSkipped = (step: number) => {
    return skipped.has(step);
  };

  const handleNext = async () => {
    setIsNextLoading(true);
    if(!await props.steps[activeStep].executeNext()) {
      setIsNextLoading(false);
      return;
    }
    setIsNextLoading(false);
    let newSkipped = skipped;
    if (isStepSkipped(activeStep)) {
      newSkipped = new Set(newSkipped.values());
      newSkipped.delete(activeStep);
    }

    setActiveStep((prevActiveStep) => prevActiveStep + 1);
    setSkipped(newSkipped);
    props.steps[activeStep].onShow();
  };

  const handleBack = () => {
    setActiveStep((prevActiveStep) => prevActiveStep - 1);
    props.steps[activeStep].onShow();
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
    props.steps[activeStep].onShow();
  };

  const handleReset = () => {
    setActiveStep(0);
  };

  const handleClose = () => props.onClose();

  const getStepView = (step : number) => props.steps[step].view();
  
  return (
        <Box sx={{ width: '100%', minWidth: "60%", mt: "2em" }}>
          <Stepper activeStep={activeStep}>
            {props.steps.map((step, index) => {
              const stepProps: { completed?: boolean } = {};
              const labelProps: {
                optional?: React.ReactNode;
              } = {};
              
              if (isStepSkipped(index)) {
                stepProps.completed = false;
              }
              return (
                <Step key={step.label} {...stepProps}>
                  <StepLabel {...labelProps}>{step.label}</StepLabel>
                </Step>
              );
            })}
          </Stepper>
          {activeStep === props.steps.length ? (
            <React.Fragment>
              <Box sx={{ mt: 2, mb: 1 , minHeight: 200}}>
                 {props.finishedView}
              </Box>
              
              <Box sx={{ display: 'flex', flexDirection: 'row', pt: 2, mt: 5 }}>
                <Button onClick={handleReset}>{props.resetLabel}</Button>
                <Box sx={{ flex: '1 1 auto' }} />
                <Button onClick={handleClose} variant="contained">{t("buttons.close")}</Button>
              </Box>
            </React.Fragment>
          ) : (
            <React.Fragment>
              <Box sx={{ mt: 2, mb: 1, minHeight: 200 }}>
                {getStepView(activeStep)}
                
              </Box>             
            
              <Box sx={{ display: 'flex', flexDirection: 'row', pt: 2 }}>
                <Button
                  color="inherit"
                  disabled={activeStep === 0}
                  onClick={handleBack}
                  startIcon={<ArrowBackIcon />}
                  sx={{ mr: 1 }}>
                  Back
                </Button>
                <Button startIcon={<CancelIcon />} color="inherit" onClick={handleClose}>{t("buttons.cancel")}</Button>
                <Box sx={{ flex: '1 1 auto' }} />
                {isStepOptional(activeStep) && (
                  <Button color="inherit" onClick={handleSkip} sx={{ mr: 1 }}>
                    {t("buttons.skip")}
                  </Button>
                )}
                <LoadingButton onClick={handleNext} loading={isNextLoading}
                  endIcon={<ArrowForwardIcon/>}
                  loadingPosition="end" variant="contained">
                  {props.steps[activeStep].nextLabel}
                </LoadingButton>
              </Box>
            </React.Fragment>
          )}
        </Box> );
}

export default StepperProcess;