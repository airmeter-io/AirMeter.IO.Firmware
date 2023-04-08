import './Live.css';
import * as React from 'react';
import Box from '@mui/material/Box';
import Grid from '@mui/material/Grid';
import Header from '../Header';
import Footer from '../Footer';
import MinorRadialGauge from './MinorRadialGauge';
import CO2RadialGauge from './CO2RadialGauge';
import Container from '@mui/material/Container';
import CssBaseline from '@mui/material/CssBaseline';
import {IMeterValues, IMeterValueGroup} from '../ViewModel/MeterView';
import MainView from '../ViewModel/MainView';
import Typography from '@mui/material/Typography';
import Tabs from '@mui/material/Tabs';
import Tab from '@mui/material/Tab';
import SwipeableViews from 'react-swipeable-views';
import ValueGroupCard from './ValueGroupCard';

interface TabPanelProps {
  children?: React.ReactNode;
  dir?: string;
  index: number;
  value: number;
}

function TabPanel(props: TabPanelProps) {
  const { children, value, index, ...other } = props;

  return (
    <div
      role="tabpanel"
      hidden={value !== index}
      id={`full-width-tabpanel-${index}`}
      aria-labelledby={`full-width-tab-${index}`}
      {...other}
    >
      {value === index && (
        <Box sx={{ p: 3 }}>
          <Typography>{children}</Typography>
        </Box>
      )}
    </div>
  );
}

function a11yProps(index: number) {
  return {
    id: `full-width-tab-${index}`,
    'aria-controls': `full-width-tabpanel-${index}`,
  };
}

interface ILiveState {
  values : IMeterValues | undefined;
  tabIndex : number;
}

class Live extends React.Component<{},ILiveState> {
  state : ILiveState = {
    values: undefined,
    tabIndex: 0
  }

  private interval : ReturnType<typeof setInterval>;

  private async fetchData() {
    var values = await MainView.Current.MeterView.GetLatest();
    this.setState({
      ...this.state,
      values: values,
    });
  }

  private handleTabChange(event: React.SyntheticEvent, newValue: number) {
    this.setState({
      ...this.state,
      tabIndex: newValue,
    });
  }

  private handleTabChangeIndex(index: number) {
    this.setState({
      ...this.state,
      tabIndex: index,
    });
  }

  componentDidMount () {   
    this.fetchData(); 
    this.interval = setInterval(() => { this.fetchData() }, 10000);
  }
  componentWillUnmount () {
      clearInterval(this.interval);
  }

  render() {
    return (
      <Box>
        <Header title="Live" titleArea={<Tabs
          value={this.state.tabIndex}
          onChange={this.handleTabChange.bind(this)}
          indicatorColor="secondary"
          textColor="inherit"
          variant="fullWidth"
          aria-label="full width tabs example"
        >
          <Tab label="Meter" {...a11yProps(0)} />
          <Tab label="Chart" {...a11yProps(1)} />
          <Tab label="Values" {...a11yProps(2)} />
        </Tabs>}/>

        <SwipeableViews axis={'x'} index={this.state.tabIndex} onChangeIndex={this.handleTabChangeIndex.bind(this)}>
          <TabPanel value={this.state.tabIndex} index={0} >
          <Container maxWidth={false} sx={{ height: '90vh' }} disableGutters>
          { this.state.values === undefined ? <div/> :
          <div className="gaugeContainer">
            <div className="tempGaugeContainer">
                <MinorRadialGauge
                    Title="Temperature" 
                    Units="C" 
                    From={-20} 
                    To={50} 
                    MajorTicks={["-20","","-10","","0","","10","","20","","30","","40","","50"]} 
                    MinorTicks={2} 
                    Decimals={2} 
                    Value={this.state.values.temp} />
            </div>               
            <div className="co2GaugeContainer" >
                <CO2RadialGauge Value={this.state.values.co2}/>
            </div>
            <div className="humidityGaugeContainer">
                <MinorRadialGauge
                    Title="Humdity" 
                    Units="%" 
                    From={0} 
                    To={100} 
                    MajorTicks={["0","","25","","50","","75","","100"]} 
                    MinorTicks={5} 
                    Decimals={2} 
                    Value={this.state.values.humidity} />
            </div>	
            <div  className="pressureGaugeContainer">
                <MinorRadialGauge
                    Title="Pressure" 
                    Units="hPa" 
                    From={975} 
                    To={1045} 
                    MajorTicks={["","980","","990","","1000","","1010","","1020","","1030","","1040",""]} 
                    MinorTicks={10} 
                    Decimals={1} 
                    Value={this.state.values.pressure} />
            </div>	
          </div> }
        </Container>
          </TabPanel>
          <TabPanel value={this.state.tabIndex} index={1} >
            Item Two
          </TabPanel>
          <TabPanel value={this.state.tabIndex} index={2} >
            <Grid container spacing={{ xs: 2, md: 3}} columns={{ xs: 4, md: 8, lg: 12}}>
              { this.state.values!==undefined && this.state.values.groups.map((group : IMeterValueGroup ,i)  => 
                <Grid item xs={4} md={4}  >
                  <ValueGroupCard  group={group}/>
                </Grid> )}         
            </Grid>           
          </TabPanel>
        </SwipeableViews>

        <CssBaseline />
        
        
        <Footer/>
      </Box>);
  }
}

export default Live;