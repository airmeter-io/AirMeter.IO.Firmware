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
import FormLabel from '@mui/material/FormLabel';
import FormControl from '@mui/material/FormControl';
import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import FormHelperText from '@mui/material/FormHelperText';
import Switch from '@mui/material/Switch';
import Button from '@mui/material/Button';
import Stack from '@mui/material/Stack';
import * as React from 'react';
import AppBreadcrumb from '../AppBreadcrumb';

function General() {
  const [state, setState] = React.useState({
    enablePowerSaving: true
  });
  const { t } = useTranslation(namespaces.settings);
  const handlePowerSaveChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setState({
      ...state,
      [event.target.name]: event.target.checked,
    });
  };
  return (
    <Box>
      <Header title={t("general.title")}/>
      <Container maxWidth="sm" sx={{ width: '100%', m: '2rem' }}>
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
      <FormControl component="fieldset" variant="standard"  sx={{minWidth: "500px"}}>    
        <FormGroup>
          <TextField
                  autoFocus={ true}                                 
                  margin="dense"
                  id="deviceName"
                  label={t("general.deviceName.title")}
                  type="text"
                  fullWidth
                  variant="standard" />     
          <FormHelperText>{t("general.deviceName.helper")}</FormHelperText>
        </FormGroup>
        <FormGroup sx={{mt: "1em"}}>
          <FormControlLabel
            control={
              <Switch checked={state.enablePowerSaving} onChange={handlePowerSaveChange} name="enablePowerSave" />
            }
            label={t("general.powerSave.title")}/>        
        </FormGroup>
        <FormGroup sx={{mt: "2em"}}>
          <Stack spacing={2} direction="row">
            <Button variant="contained">{t("buttons.save")}</Button>
          </Stack>
        </FormGroup>        
      </FormControl>           
      </Container>
      <Footer/>
    </Box>);
}

export default General;