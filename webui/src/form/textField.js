import React from 'react';

class FormTextField extends React.Component {
    constructor(props) {
      super(props);
      this.state = { enabled: true};
      this.textRef = React.createRef();
    }

    getValue() {
      return this.textRef.current.value;
    }

    enable() {
      this.setState({ enabled: true });
    }

    disable() {
      this.setState({ enabled: false });
    }

    handleChange() {
      this.textRef.current.setCustomValidity("");
      if(!this.textRef.current.checkValidity()) {
        if(this.getValue() == "" && this.props.Mandatory) 
          this.textRef.current.setCustomValidity(this.props.MandatoryMessage!=undefined ? this.props.MandatoryMessage : "");
        else {
          this.textRef.current.setCustomValidity(this.props.PatternMessage!=undefined ? this.props.PatternMessage : "");
        }    
      }
      this.textRef.current.reportValidity();
      
      if(this.props.OnChange!=undefined) {
        this.props.OnChange(getValue());
      }
    }

    componentDidMount () {
      const inputListenerFn = this.handleChange.bind(this);
      const changeListenerFn = this.handleChange.bind(this);
      this.textRef.current.addEventListener("change", changeListenerFn);
      this.textRef.current.addEventListener("input", inputListenerFn);
      this.removeListener = () => {
        this.textRef.current.removeEventListener('change', changeListenerFn);
        this.textRef.current.removeEventListener('input', inputListenerFn);
      };
    }
    componentWillUnmount () {
      this.removeListener();
    }

    render() {
      
      return [
        <label><strong>{this.props.Label}</strong></label>,
        <input ref={this.textRef} class="w3-input" type="text" required={this.props.Mandatory} disabled={!this.props.Enabled} value={this.props.Value} placeholder={this.props.PlaceHolder} pattern={this.props.Pattern} />         
      ];
    }

    save() {
      this.props.OnSave(this.getValue());
    }
}

export default FormTextField;