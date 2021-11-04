import React from 'react'
import ReactDOM from 'react-dom';
import {RadialGauge} from 'canvas-gauges'

class ReactRadialGauge extends React.Component {
  componentDidMount () {
    const options = Object.assign({}, this.props, {
      renderTo: this.el
    })
    this.gauge = new RadialGauge(options).draw();
    window.addEventListener('resize', this.updateDimensions);   
  }

  componentWillUnmount() {
    window.removeEventListener('resize', this.updateDimensions);
  }

  componentWillReceiveProps (nextProps) {
    this.gauge.value = nextProps.value
    this.gauge.update(nextProps)
  }

  updateDimensions = () => {
    this.gauge.update( { width: ReactDOM.findDOMNode(this).parentNode.clientWidth, height: ReactDOM.findDOMNode(this).parentNode.clientHeight });
  };

  render () {
    return (
      <canvas ref={(canvas) => {
        this.el = canvas
      }} />
    )
  }
}

export default ReactRadialGauge