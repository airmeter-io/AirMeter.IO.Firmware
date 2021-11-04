import React from 'react';
import { Line } from 'react-chartjs-2';
import 'chartjs-adapter-date-fns';

const options = {
  responsive: true,
  interaction: {
    mode: 'index',
    intersect: false,
  },
  stacked: false,
  elements: {
    point: {
      radius: 1
    }
  },
  plugins: {
    title: {
      display: true,
      text: 'Chart.js Line Chart - Multi Axis'
    }
  },
  scales: {
    x: {
      type: 'time',
      time: {
        // Luxon format string
       // tooltipFormat: 'DD T'
      },
      title: {
        display: true,
        text: 'Date'
      }
    },
    y: 
      {
        type: 'linear',
        display: true,
        position: 'left',
        min: 400,
        max: 2000,
        grid: {
          drawTicks: false,
        },
        ticks: {
          // For a category axis, the val is the index so the lookup via getLabelForValue is needed
          callback: (val, index) => val+" ppm",
          color: 'red',
        }
      },
    y1: {
        type: 'linear',
        display: true,
        position: 'right',
        min: 0,
        max: 35,
        grid: {
          drawOnChartArea: false,
        },
        ticks: {
          // For a category axis, the val is the index so the lookup via getLabelForValue is needed
          callback: (val, index) => val+" °C",
          color: 'red',
        }
    },
    y2:  {
        type: 'linear',
        display: true,
        position: 'right',
        min: 0,
        max: 100,
        grid: {
          drawOnChartArea: false,
        },
        display: false
      },
    y3: {
        type: 'linear',
        display: true,
        position: 'right',
        min: 950,
        max: 1080,
        grid: {
          drawOnChartArea: false,
        },
        display: false
      }
    
  }
};

const CHART_COLORS = {
  red: 'rgb(255, 99, 132)',
  orange: 'rgb(255, 159, 64)',
  yellow: 'rgb(255, 205, 86)',
  green: 'rgb(75, 192, 192)',
  blue: 'rgb(54, 162, 235)',
  purple: 'rgb(153, 102, 255)',
  grey: 'rgb(201, 203, 207)',
  red50: 'rgba(255, 99, 132, 0.5)',
  orange50: 'rgba(255, 159, 64, 0.5)',
  yellow50: 'rgba(255, 205, 86, 0.5)',
  green50: 'rgba(75, 192, 192, 0.5)',
  blue50: 'rgba(54, 162, 235, 0.5)',
  purple50: 'rgba(153, 102, 255, 0.5)',
  grey50: 'rgba(201, 203, 207, 0.5)'
};


class ChartsPage extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
        data:{
          datasets: [
            // {
            //   label: 'CO2 (ppm)',
            //   data: [],
            //   fill: false,
            //   backgroundColor: 'rgb(255, 99, 132)',
            //   borderColor: 'rgba(255, 99, 132, 0.2)',
            //   yAxisID: 'y'
            // }, {
            //   label: 'Temperature (C)',
            //   data: [],
            //   fill: false,
            //   backgroundColor: 'rgb(255, 99, 132)',
            //   borderColor: 'rgba(255, 132,  99, 0.2)',
            //   yAxisID: 'y1'
            // }, {
            //   label: 'Pressure',
            //   data: [],
            //   fill: false,
            //   backgroundColor: 'rgb(255, 99, 132)',
            //   borderColor: 'rgba(255, 99, 132, 0.2)',
            //   yAxisID: 'y3'
            // }, {
            //   label: 'Humidity',
            //   data: [],
            //   fill: false,
            //   backgroundColor: 'rgb(255, 99, 132)',
            //   borderColor: 'rgba(255, 99, 132, 0.2)',
            //   yAxisID: 'y2'
            // }
          ]
        }
    };
  }

  fetchData = (pFrom, pTo) => {
      var from = parseInt(pFrom.getTime() / 1000).toFixed(0);
      var to = parseInt(pTo.getTime() / 1000).toFixed(0);
      const requestOptions = {
          method: 'POST',
          headers: { 'Content-Type': 'application/json;charset=UTF-8' },
          body: JSON.stringify({ COMMAND: "GETDATA", Mode: "Read", From: from.toString(), To: to.toString()})
      };
      fetch('command', requestOptions)
          .then(response => {return response.json();})
          .then(data => {     
              console.log(data);

              var co2 = {
                label: 'CO2 (ppm)',
                data: [],
            
                borderColor: CHART_COLORS.red,
                backgroundColor: CHART_COLORS.red50,
                fill: true,
                yAxisID: 'y',
                cubicInterpolationMode: 'monotone',
                tension: 0.4  
              };
              var temp = {
                label: 'Temperature (C)',
                data: [],

                borderColor: CHART_COLORS.blue,
                backgroundColor: CHART_COLORS.blue50,
                yAxisID: 'y1',
                cubicInterpolationMode: 'monotone',
                tension: 0.4  
              };
              var pressure ={
                label: 'Pressure',
                data: [],

                borderColor: CHART_COLORS.green,
                backgroundColor: CHART_COLORS.green50,
                yAxisID: 'y3',
                cubicInterpolationMode: 'monotone',
                tension: 0.4  
              };
              var humidity = {
                label: 'Humidity',
                data: [],
  
                borderColor: CHART_COLORS.orange,
                backgroundColor:CHART_COLORS.orange50,
                yAxisID: 'y2',
                cubicInterpolationMode: 'monotone',
                tension: 0.4  
              };

              for(var i = 0; i < data.Readings.length; i++) {
                 var dateStr = new Date(data.Readings[i].TS*1000);//.toUTCString();
                 co2.data.push({x: dateStr, y: Number(data.Readings[i].C)});
                 temp.data.push({x: dateStr, y: Number(data.Readings[i].T)});
                 pressure.data.push({x: dateStr, y: Number(data.Readings[i].P)});
                 humidity.data.push({x: dateStr, y: Number(data.Readings[i].H)});
              }

              this.setState({data: {
                datasets: [
                  co2,
                  temp,
                  pressure,
                  humidity
                ]
              }});
              return true;
          });        
    }

  componentDidMount () {
    var to = new Date();
    var from = new Date();
    from.setMonth(from.getMonth()-1); // BROKEN FOR JAN!
    this.fetchData(from,to);
   // this.interval = setInterval(() => { this.fetchData() }, 10000);
  }
  componentWillUnmount () {
    //  clearInterval(this.interval);
  }

  render() {
    return [
      <div>
        <div class="w3-container">
        <Line data={this.state.data} options={options} />
        </div>
      </div>          
    ];
  }
}


export default ChartsPage;