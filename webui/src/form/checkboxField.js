import React from 'react';

class FormCheckboxField extends React.Component {
    constructor(props) {
      super(props);
      this.state = {enabled: true, checked: null};
      this.cbxRef = React.createRef();
    }

    getValue() {
      return this.cbxRef.current.checked;
    }

    enable() {
      this.setState({ enabled: true });
    }

    disable() {
      this.setState({ enabled: false });
    }

    handleChange() {
      if(this.props.OnChange!=undefined) {
         this.props.OnChange(this.getValue());
      }
    }

    componentDidMount () {
      
      const inputListenerFn = this.handleChange.bind(this);
      const changeListenerFn = this.handleChange.bind(this);
      this.cbxRef.current.addEventListener("change", changeListenerFn);
      this.cbxRef.current.addEventListener("input", inputListenerFn);
      this.removeListener = () => {
        this.cbxRef.current.removeEventListener('change', changeListenerFn);
        this.cbxRef.current.removeEventListener('input', inputListenerFn);
      };
    }

    componentWillUnmount () {
      this.removeListener();
    }


    render() {
      if(this.state.checked == null) this.state.checked = this.props.Value;
      return [
        <label class="checkbox"><strong>{this.props.Label}</strong>
          <input disabled={!this.state.enabled} ref={this.cbxRef} type="checkbox" checked={this.props.Value}/>
          <span class="checkmark"></span>
        </label>       
      ];
    }

    save() {
      this.props.OnSave(this.getValue());
    }
}

export default FormCheckboxField;
