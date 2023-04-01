import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import NetworkWifiIcon from '@mui/icons-material/NetworkWifi';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import NetworkPingIcon from '@mui/icons-material/NetworkPing';
function WirelessOptions() {
  const { t } = useTranslation(namespaces.settings);
  return (
    <Box>
      <Header title={t("wireless.options.title")}/>
      <Container sx={{ width: 'auto', m: '1rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("wireless.breadcrumb"),
            icon: <NetworkWifiIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings/wireless"
          },          
          {
            title: t("wireless.options.breadcrumb"),
            icon: <NetworkPingIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em'}} color="text.primary">
        {t("wireless.options.description")}
        </Typography>     
      </Container>
      <Footer/>
    </Box>);
}

export default WirelessOptions;