import React from 'react';

class Form extends React.Component {
    constructor(props) {
      super(props);

      this.form = React.createRef();
    }

    componentDidMount () {
    }
    componentWillUnmount () {
    }
    
    save() {
        
        for(var index in this.myRefs)
            this.myRefs[index].current.save();
    }

    validate() {
        return this.form.current.reportValidity();
    }
  
    render() {
        this.myRefs = {};
        var t = this;
        function createRef(index) {
            var r = React.createRef();
            t.myRefs[index] = r;
            return r;
        }
        createRef.bind(this);
        const children = React.Children.map(this.props.children,
        (child, index) => React.cloneElement(child, {
            ref : createRef(index)
        })
        );
        
      return [
        <form ref={this.form} class="w3-container">
            {children.map(item =><p>{item}</p>)}
        </form>
      ];
    }
}

export default Form;
