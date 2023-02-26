import './Live.css';
import * as React from 'react';
import Box from '@mui/material/Box';
import Header from '../Header';
import Footer from '../Footer';
import MinorRadialGauge from './MinorRadialGauge';
import CO2RadialGauge from './CO2RadialGauge';
import Container from '@mui/material/Container';
import CssBaseline from '@mui/material/CssBaseline';
import {IMeterValues, IMeterValueGroup} from '../ViewModel/MeterView';
import MainView from '../ViewModel/MainView';
import Typography from '@mui/material/Typography';
import Divider from '@mui/material/Divider';

class Live extends React.Component<{},IMeterValues> {
  state = {
    temp: 10,
    humidity: 50,
    pressure: 1000,
    co2: 400,
    groups: []
  }

  private interval : ReturnType<typeof setInterval>;

  private async fetchData() {
    var values = await MainView.Current.MeterView.GetLatest();
    this.setState(values);
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
        <Header title="Live">
        { this.state.groups.map((group : IMeterValueGroup ,i) => 
          <React.Fragment>
            <Box sx={ i === this.state.groups.length? {ml: 5, mr: 5, mt: 5, mb:5 } : {ml: 5, mr: 5, mt: 5, mb:0 }}>           
              <Typography paragraph variant="h5">{group.name}</Typography>            
                {group.values.map((value) => (
                  <Typography>
                    {value.name}: <strong>{value.value}</strong>
                  </Typography>                
                ))}
              <Divider/>
            </Box>
          </React.Fragment>) }
        </Header>
        <CssBaseline />
        <Container maxWidth={false} sx={{ height: '90vh' }} disableGutters>
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
                    Value={this.state.temp} />
            </div>               
            <div className="co2GaugeContainer" >
                <CO2RadialGauge Value={this.state.co2}/>
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
                    Value={this.state.humidity} />
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
                    Value={this.state.pressure} />
            </div>	
          </div>
        </Container>
        
        <Footer/>
      </Box>);
  }
}

export default Live;