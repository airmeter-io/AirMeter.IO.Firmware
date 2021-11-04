import React from 'react';
import MinorRadialGauge  from "./minorRadialGauge.js";
import CO2RadialGauge from "./co2meter.js";
import Sidebar from '../sidebar.js';
import "./meter.css";


class MeterPageContent extends React.Component {
    constructor(props) {
      super(props);
      this.state = {
          CO2: 420,
          CO2Max: 5000,
          CO2Heating: false,
          CO2Error: false,
          CO2SerialNo: "Unknown",
          CO2SwVersion: "-",
          CO2Name: "Unknown",
          Temp: 0,
          Time: new Date(),
          Humidity: 55,
          Pressure: 1000,
          Additional: []
      };
    }

    fetchData = () => {
        const requestOptions = {

            method: 'POST',
            headers: { 'Content-Type': 'application/json;charset=UTF-8' },
            body: JSON.stringify({ COMMAND: "GETLATEST"})
        };
        fetch('command', requestOptions)
            .then(response => {return response.json();})
            .then(data => {     
                if( JSON.parse(data.HasCo2.toLowerCase())) {            
                  this.setState({ 
                      CO2: parseInt(data.Co2,10),
                      CO2Max: parseInt(data.Co2Max,10),
                      CO2Heating: JSON.parse(data.Co2Heating.toLowerCase()),
                      CO2Error: JSON.parse(data.Co2Error.toLowerCase()),
                      CO2SerialNo: data.Co2SerialNo,
                      CO2SwVersion: data.Co2SwVersion,
                      CO2Name: data.Co2Name,
                      Temp: Number(data.Temp),
                      Pressure: Number(data.Pressure),
                      Humidity: Number(data.Humidity),
                      Additional: data.Co2AdditionalValues,
                      Time:  new Date(data.Time)});
               
                } else {
                  this.setState({ 
                    CO2Name: "None Configured",
                    Temp: Number(data.Temp),
                    Pressure: Number(data.Pressure),
                    Humidity: Number(data.Humidity),
                    Additional: [],
                    Time:  new Date(data.Time)});
                }
                return true;
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
      return [
        <div class="gaugeContainer">
            <div class="tempGaugeContainer">
                <MinorRadialGauge
                    Title="Temperature" 
                    Units="C" 
                    From={-20} 
                    To={50} 
                    MajorTicks={["-20","","-10","","0","","10","","20","","30","","40","","50"]} 
                    MinorTicks={"2"} 
                    Decimals={2} 
                    Value={this.state.Temp} />
            </div>               
            <div class="co2GaugeContainer" >
                <CO2RadialGauge Value={this.state.CO2}/>
            </div>
            <div class="humidityGaugeContainer">
                <MinorRadialGauge
                    Title="Humdity" 
                    Units="%" 
                    From={0} 
                    To={100} 
                    MajorTicks={["0","","25","","50","","75","","100"]} 
                    MinorTicks={"5"} 
                    Decimals={2} 
                    Value={this.state.Humidity} />
            </div>	
            <div  class="pressureGaugeContainer">
                <MinorRadialGauge
                    Title="Pressure" 
                    Units="hPa" 
                    From={975} 
                    To={1045} 
                    MajorTicks={["","980","","990","","1000","","1010","","1020","","1030","","1040",""]} 
                    MinorTicks={"10"} 
                    Decimals={1} 
                    Value={this.state.Pressure} />
            </div>	
        </div>,
        <Sidebar>
             <div class="w3-container">
                <h5>Device</h5>
                <table class="w3-table w3-blue-grey">
                    <tbody>
                        <tr>
                          <td width="35%"> <strong>Date &amp; Time</strong></td>
                          <td width="65%"> {this.state.Time.toLocaleString()} </td>
                        </tr>
                        
                    </tbody>   
                </table>
                <h5>CO2 Sensor Information</h5>
                <table class="w3-table w3-blue-grey">
                    <tbody>
                        <tr>
                          <td width="35%"><strong>Sensor</strong></td>
                          <td width="65%"> {this.state.CO2Name} (Max <span>{this.state.CO2Max}</span>ppm)</td>
                        </tr>
                        <tr>
                          <td><strong>Serial Number</strong></td>
                          <td>{this.state.CO2SerialNo}</td>
                        </tr>
                        <tr>
                          <td><strong>Firmware Version</strong></td>
                          <td>{this.state.CO2SwVersion}</td>
                        </tr>
                        <tr>
                          <td><strong>Warming Up</strong></td>
                          <td>{this.state.CO2Heating}</td>
                        </tr>
                        <tr>
                          <td><strong>Has Error</strong></td>
                          <td>{this.state.CO2Error}</td>
                        </tr>
                    </tbody>   
                </table>       
                <h5>Model Specific Properies</h5>
                <p>Each CO2 sensor unit has its own distinct property which are displayed below.</p>
                <table class="w3-table w3-blue-grey">
                    <tbody>{this.state.Additional.map(item => 
                        <tr>
                          <td width="45%"><strong>{item.Name}</strong></td>
                          <td width="55%">{item.Value}</td>
                        </tr>)}
                    </tbody>   
                </table>

             </div>
        </Sidebar>
            
      ];
    }
  }


export default MeterPageContent;

