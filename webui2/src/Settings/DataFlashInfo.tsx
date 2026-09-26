import Box from '@mui/material/Box';
import Container from '@mui/material/Container';
import Header from '../Header';
import Footer from '../Footer';
import Typography from '@mui/material/Typography';
import SettingsIcon from '@mui/icons-material/Settings';
import PublishIcon from '@mui/icons-material/Publish';
import { namespaces } from "../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import AppBreadcrumb from '../AppBreadcrumb';
import * as React from 'react';
import MainView from '../ViewModel/MainView';
import {IDataSettingsValues, IFlashBucketInfo} from '../ViewModel/DataSettingsView'

import { useEffect } from 'react';

interface IDataFlashInfoState {
   buckets : IFlashBucketInfo[]
}

function DataFlashInfo() {
  const [state, setState] = React.useState<IDataFlashInfoState>({
    buckets: []
  });
  const { t } = useTranslation(namespaces.settings);


  


  const handleLoad = async () => {
    setState({
      ...state,
      buckets: await MainView.Current.DataSettings.GetFlashBucketInfos(),
    });
  }

  useEffect(() => {
    handleLoad();
  }, []);

  return (
    <Box>
      <Header title={t("data.flashInfo.title")}/>
      <Container sx={{  width: 'auto', m: '1rem', mb: '4rem' }}>
      <AppBreadcrumb breadcrumbs={[
          {
            title: t("breadcrumb"),
            icon: <SettingsIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings"
          },
          {
            title: t("data.breadcrumb"),
            icon: <PublishIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: "/settings/data"
          },
          {
            title: t("data.flashInfo.breadcrumb"),
            icon: <PublishIcon sx={{ mr: 0.5 }} fontSize="inherit" />,
            to: null
          }]}/>        
      <Typography sx={{ display: 'flex', alignItems: 'center', mt: '1em', mb: '1em'}} color="text.primary">
      {t("data.flashInfo.description")}
      </Typography>
      <table>
        <thead>
          <th>Index</th>          
          <th>Offset</th>
          <th>Start Time</th>
          <th>End Time Time</th>
          <th>Data Length</th>
          <th>Payload Offset</th>
          <th>Num Readings</th>
          <th>Current Time</th>
        </thead>
        <tbody>
          { state.buckets.map((bucket, i) => 
            (<tr>
              <td>0x{bucket.index===-1 ? 'FFFFFFFF' : bucket.index.toString(16)}</td>
              <td>0x{bucket.offset.toString(16)}</td>
              <td>{bucket.blockStartTime === undefined ? '' : bucket.blockStartTime.toLocaleString()}</td>
              <td>{bucket.blockEndTime === undefined ? '' : bucket.blockEndTime.toLocaleString()}</td>              
              <td>0x{bucket.dataLength.toString(16)}</td>
              <td>{bucket.payloadOffset === undefined ? '' : '0x'+bucket.payloadOffset.toString(16)}</td>
              <td>{bucket.numReadings === undefined || bucket.numReadings===-1 ? '' : '0x'+bucket.numReadings.toString(16)}</td>
              <td>{bucket.currentTime === undefined ? '' : bucket.currentTime.toLocaleString()}</td> 
             </tr>)) }       
        </tbody>
                
      </table>
    </Container>
    <Footer/>
  </Box>);
}

export default DataFlashInfo;