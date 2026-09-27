import Box from '@mui/material/Box';
import SettingsIcon from '@mui/icons-material/Settings';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemText from '@mui/material/ListItemText';
import ListItemAvatar from '@mui/material/ListItemAvatar';
import Avatar from '@mui/material/Avatar';
import ListItemButton from '@mui/material/ListItemButton';
import Container from '@mui/material/Container';
import NetworkWifiIcon from '@mui/icons-material/NetworkWifi';
import CompassCalibrationIcon from '@mui/icons-material/CompassCalibration';
import StorageIcon from '@mui/icons-material/Storage';
import PublishIcon from '@mui/icons-material/Publish';
import TimelapseIcon from '@mui/icons-material/Timelapse';
import PeopleIcon from '@mui/icons-material/People';
import Header from '../Header';
import Footer from '../Footer';
import { Link } from 'react-router-dom';
import DisplaySettingsIcon from '@mui/icons-material/DisplaySettings';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import ISettingSection from './Components/ISettingSection';




function Settings() {
  const { t } = useTranslation(namespaces.settings);

  const settingsSections : ISettingSection[]= [
    {
       title: t("general.title"),
       description: t("general.description"),
       to: "/settings/general",
       icon: <DisplaySettingsIcon/>
    },
    {
      title: t("wireless.title"),
      description: t("wireless.description"),
      to: "/settings/wireless",
      icon: <NetworkWifiIcon />
    },
    {
      title: t("calibration.title"),
      description: t("calibration.description"),
      to: "/settings/calibration",
      icon: <CompassCalibrationIcon />
    },
    {
      title: t("data.title"),
      description: t("data.description"),
      to: "/settings/data",
      icon: <StorageIcon />
    },
    {
      title: t("cloud.title"),
      description: t("cloud.description"),
      to: "/settings/cloud",
      icon: <PublishIcon />
    },
    {
      title: t("ntp.title"),
      description: t("ntp.description"),
      to: "/settings/ntp",
      icon: <TimelapseIcon />
    },
    {
      title: t("users.title"),
      description: t("users.description"),
      to: "/settings/users",
      icon: <PeopleIcon />
    }]

  return (
    <Box>
      <Header title={t("title")}/>
      <Container sx={{ width: 'auto', m: '1rem', mb: '4rem' }}>
        <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>
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

export default Settings;