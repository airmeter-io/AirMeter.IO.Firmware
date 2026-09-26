import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import PeopleIcon from '@mui/icons-material/People';
import TextField from '@mui/material/TextField';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';


function Users() {
  const { t } = useTranslation(namespaces.settings);
  return (
    <Box>
      <Header title={t("users.title")}/>
      <Container sx={{ width: 'auto', m: '1rem', mb: '4rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("users.breadcrumb"),
            icon: <PeopleIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' }} color="text.primary">
        {t("users.description")}
        </Typography>
        <TextField
                autoFocus
                margin="dense"
                id="deviceName"
                label="Device Name"
                type="text"
                fullWidth
                variant="standard" />           
      </Container>
      <Footer/>
    </Box>);
}

export default Users;