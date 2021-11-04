import React from 'react';

class FormSliderField extends React.Component {
    constructor(props) {
      super(props);
      this.state = {
          enabled: true,
          curVal: props.Value
      };
      this.slideRef = React.createRef();
    }

    getValue() {
      return this.slideRef.current.value;
    }

    enable() {
      this.setState({ enabled: true });
    }

    disable() {
      this.setState({ enabled: false });
    }

    handleChange() {
      
      if(this.props.OnChange!=undefined) {
         this.props.OnChange(getValue());
      }
      this.setState({curVal: this.getValue()});
    }

    componentDidMount () {
      
      const selectListenerFn = this.handleChange.bind(this);
      const selectListenerFn2 = this.handleChange.bind(this);
      this.slideRef.current.addEventListener("change", selectListenerFn);
      this.slideRef.current.addEventListener("input", selectListenerFn2);
      this.removeListener = () => {
        this.slideRef.current.removeEventListener('change', selectListenerFn);
        this.slideRef.current.removeEventListener('input', selectListenerFn2);
      };
    }
    componentWillUnmount () {
      this.removeListener();
    }

    render() {
      if(this.state.curVal == undefined) this.state.curVal = this.props.Value;
      return [
        <strong>{this.props.FormatCurrentValue(this.state.curVal)}</strong>,
        <div class="slidecontainer">
            <input ref={this.slideRef} type="range" min={this.props.Min} max={this.props.Max} value={this.state.curVal} step={this.props.Step} class="slider" />
        </div>
      ];
    }

    save() {
      this.props.OnSave(this.getValue());
    }
}

export default FormSliderField;
