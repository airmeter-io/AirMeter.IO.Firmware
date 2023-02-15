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

function CalibrationAutomatic() {
  const { t } = useTranslation(namespaces.settings);

  return (
    <Box>
      <Header title={t("calibration.automatic.title")}/>
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
            title: t("calibration.automatic.breadcrumb"),
            icon: <RestartAltIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em' }} color="text.primary">
        {t("calibration.automatic.description")}
        </Typography>  
      </Container>
      <Footer/>
    </Box>);
}

export default CalibrationAutomatic;