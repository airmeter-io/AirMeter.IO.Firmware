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
import ISettingSection from './ISettingSection';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemAvatar from '@mui/material/ListItemAvatar';
import Avatar from '@mui/material/Avatar';
import ListItemText from '@mui/material/ListItemText';
import { Link } from 'react-router-dom';
import WifiFindIcon from '@mui/icons-material/WifiFind';
import NetworkPingIcon from '@mui/icons-material/NetworkPing';
function Wireless() {
  const { t } = useTranslation(namespaces.settings);
  const settingsSections : ISettingSection[]= [
    {
       title: t("wireless.networks.title"),
       description: t("wireless.networks.description"),
       to: "/settings/wireless/networks",
       icon: <WifiFindIcon/>
    },
    {
      title: t("wireless.options.title"),
      description: t("wireless.options.description"),
      to: "/settings/wireless/options",
      icon: <NetworkPingIcon />
    }];

  return (
    <Box>
      <Header title={t("wireless.title")}/>
      <Container maxWidth="sm" sx={{ width: '100%', m: '2rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("wireless.breadcrumb"),
            icon: <NetworkWifiIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em' }} color="text.primary">
        {t("wireless.description")}
        </Typography>
        <List sx={{ maxWidth: '100%', minWidth: '50%', bgcolor: 'background.paper', m: 0  }}>
          { settingsSections.map((section, i) => 
            (<ListItem sx={{ pl: "0px"}}>
              <ListItemButton component={Link} to={section.to}>
                <ListItemAvatar>
                  <Avatar>
                    {section.icon}
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={section.title} secondary={section.description} />
              </ListItemButton>
            </ListItem>)) }         
        </List>
      </Container>
      <Footer/>
    </Box>);
}

export default Wireless;