import * as React from 'react';
import * as ReactDOM from 'react-dom';
import {RadialGauge, RadialGaugeOptions} from 'canvas-gauges'


class ReactRadialGauge extends React.Component<RadialGaugeOptions,{}> {
  gauge : RadialGauge;
  el : HTMLCanvasElement;
  componentDidMount () {
    var t = this;
    const options = Object.assign({}, this.props, {
      renderTo: this.el,
      // listeners:{
      //    value: function(value : number) {
      //        t.gauge.update({renderTo: t.el, units: (Number(value)).toFixed(2) + " "});
      //    } }
    });
    this.gauge = new RadialGauge(options).draw();
    window.addEventListener('resize', this.updateDimensions);   
  }

  componentWillUnmount() {
    window.removeEventListener('resize', this.updateDimensions);
  }

  componentWillReceiveProps (nextProps : RadialGaugeOptions) {
    if(nextProps.value!=null)
      this.gauge.value = nextProps.value;
    nextProps.renderTo = this.el;
    this.gauge.update(nextProps);
  }

  updateDimensions = () => {
    var node = ReactDOM.findDOMNode(this);
    if(node == null || node.parentElement==null)  return;
    var parent  = node.parentElement;
    this.gauge.update( {
      renderTo: this.el,
      width: parent.clientWidth, 
      height: parent.clientHeight });
  };

  render () {
    return (
      <canvas ref={(canvas) => {
        if(canvas!=null)
          this.el = canvas;
      }} />
    )
  }
}

export default ReactRadialGauge