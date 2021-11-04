import React from 'react';

class FormDropDownField extends React.Component {
    constructor(props) {
      super(props);
      this.state = {enabled: true}
      this.ddlRef = React.createRef();
    }

    getValue() {
      return this.ddlRef.current.value;
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
    }

    componentDidMount () {
      const inputListenerFn = this.handleChange.bind(this);
      const changeListenerFn = this.handleChange.bind(this);
      this.ddlRef.current.addEventListener("change", changeListenerFn);
      this.ddlRef.current.addEventListener("input", inputListenerFn);
      this.removeListener = () => {
        this.ddlRef.current.removeEventListener('change', changeListenerFn);
        this.ddlRef.current.removeEventListener('input', inputListenerFn);
      };
    }

    componentWillUnmount () {
      this.removeListener();
    }


    render() {
      if(typeof(this.state.value)=='undefined') this.state.value = this.props.Value;
      console.log(this.props.Value+":renderddl");
      return [
         <label><strong>{this.props.Label}</strong></label>,
         <select ref={this.ddlRef}  class="w3-select" value={this.state.value}>
           {this.props.children}           
         </select>  
      ];
    }

    save() {
      this.props.OnSave(this.getValue());
    }
}

export default FormDropDownField;
