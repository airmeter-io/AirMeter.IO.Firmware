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
import ISettingSection from './Components/ISettingSection';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import TuneIcon from '@mui/icons-material/Tune';
import List from '@mui/material/List';
import ListItem from '@mui/material/ListItem';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemAvatar from '@mui/material/ListItemAvatar';
import Avatar from '@mui/material/Avatar';
import ListItemText from '@mui/material/ListItemText';
import { Link } from 'react-router-dom';

function Calibration() {
  const { t } = useTranslation(namespaces.settings);
  const settingsSections : ISettingSection[]= [
    {
       title: t("calibration.manual.title"),
       description: t("calibration.manual.description"),
       to: "/settings/calibration/manual",
       icon: <TuneIcon/>
    },
    {
      title: t("calibration.options.title"),
      description: t("calibration.options.description"),
      to: "/settings/calibration/options",
      icon: <RestartAltIcon />
    }];

  return (
    <Box>
      <Header title={t("calibration.title")}/>
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
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em' }} color="text.primary">
        {t("calibration.description")}
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

export default Calibration;