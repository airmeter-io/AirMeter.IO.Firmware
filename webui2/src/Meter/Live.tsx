import './Live.css';
import * as React from 'react';
import * as ReactDOM from 'react-dom';
import Box from '@mui/material/Box';
import Header from '../Header';
import Footer from '../Footer';
import MinorRadialGauge from './MinorRadialGauge';
import CO2RadialGauge from './CO2RadialGauge';
import Container from '@mui/material/Container';
import CssBaseline from '@mui/material/CssBaseline';

interface ILiveState {
  Temp : number;
  Humidity : number;
  Pressure : number;
  CO2 : number;
}

class Live extends React.Component<{},ILiveState> {
  state = {
    Temp: 10,
    Humidity: 50,
    Pressure: 1000,
    CO2: 400 
  }
  render() {
    return (
      <Box>
        <Header title="Live"/>
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
                      Value={this.state.Temp} />
              </div>               
              <div className="co2GaugeContainer" >
                  <CO2RadialGauge Value={this.state.CO2}/>
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
                      Value={this.state.Humidity} />
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
                      Value={this.state.Pressure} />
              </div>	
          </div>
      </Container>
        
        <Footer/>
      </Box>);
  }
}

export default Live;