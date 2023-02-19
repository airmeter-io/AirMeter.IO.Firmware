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
import { DataGrid, GridColDef,GridRenderCellParams } from '@mui/x-data-grid';
import WirelessActionsMenu from './Components/WirelessActionsMenu';
import WifiFindIcon from '@mui/icons-material/WifiFind';
import Fab from '@mui/material/Fab';
import AddIcon from '@mui/icons-material/Add';

const columns: GridColDef[] = [
  { field: 'id', headerName: 'Priority', width: 90 },
  {
    field: 'ssid',
    headerName: 'SSID',
    width: 150,
    editable: false,
  },
  {
    field: 'status',
    headerName: 'Status',
    width: 150,
    editable: false,
  },
  {
    field: 'actions',
    headerName: 'Actions',    
    sortable: false,
    width: 160,
    renderCell: (params: GridRenderCellParams<string>) => (
      
        <WirelessActionsMenu/>
      
    ),
  },
];

const rows = [
  { id: 1, ssid: 'MoreThings', status: 'Connected', actions: {}},
  { id: 2, ssid: 'MoreThings2', status: '', actions: {}},

];

function WirelessNetworks() {
  const { t } = useTranslation(namespaces.settings);
  return (
    <Box>
      <Header title={t("wireless.networks.title")}/>
      <Container maxWidth="md" sx={{ width: '100%', m: '2rem' }}>
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
            title: t("wireless.networks.breadcrumb"),
            icon: <WifiFindIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
        <Typography sx={{ display: 'flex', alignItems: 'center' , mt: '1em', mb: '1em'}} color="text.primary">
        {t("wireless.networks.description")}
        </Typography>
        <Box sx={{ height: 400, width: '100%' }}>
      <DataGrid
        rows={rows}
        columns={columns}
        pageSize={5}
        rowsPerPageOptions={[5]}
        disableSelectionOnClick
        experimentalFeatures={{ newEditingApi: true }}
      />
     
 

    </Box>      
      </Container>
       <Fab color="primary" aria-label="add" sx={{ position: 'fixed', bottom: 70,  right: 10 }} > <AddIcon />
        </Fab>
      <Footer/>
    </Box>);
}

export default WirelessNetworks;