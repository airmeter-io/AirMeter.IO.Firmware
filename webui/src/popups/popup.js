import React from 'react';

import "./popup.css";

class PopupAction {
    constructor(pName, pPerformAction, pRight) {
        this.name = pName;       
        this.performAction = pPerformAction;
        this.right = pRight;
    }

    getRight() {
      if(this.right==undefined) return false;
      return this.right;
    }
}

class PopupActionInternal extends React.Component {
  constructor(props) {
    super(props);
    this.buttonRef = React.createRef();
  }

  handleClick() {
    console.log("button clicked");
    if(this.props.Item.performAction(this.props.Item))
        this.props.onClose();
  }

  componentDidMount () {
    const listenerFn = this.handleClick.bind(this);
    this.buttonRef.current.addEventListener("click", listenerFn);
    this.removeListener = () => this.buttonRef.current.removeEventListener('click', listenerFn)
  }
  componentWillUnmount () {
    this.removeListener();
  }

  render() {
    if(this.props.Last) {
      return [
        <a href="#" class=" popupAction" ref={this.buttonRef}>
              {this.props.Item.name}             
              </a>        
      ];  
    }
    return [
      <a href="#" class=" popupAction" ref={this.buttonRef}>
            {this.props.Item.name}             
            </a>,
      <div class="popupActionSeparator"/>
    ];
  }
}


class Popup extends React.Component {
    constructor(props) {
      super(props);      
      this.closeRef = React.createRef();
      this.popupRef = React.createRef();
      this.state = {
          open: false
      }
    }
  
    show() {
        this.popupRef.current.style.display = "block";
        this.setState({open:true});
        if(this.props.OnShow!=undefined)
          this.props.OnShow();
    }
  
    hide() {
      this.popupRef.current.style.display = "none";
      this.setState({open:false});
      if(this.props.OnClose!=undefined)
        this.props.OnClose();
    }
  
    componentDidMount () {
      const hideListenerFn = this.hide.bind(this);
      
      this.closeRef.current.addEventListener("click", hideListenerFn);
      this.removeListener = () => this.closeRef.current.removeEventListener('click', hideListenerFn)
    }
    componentWillUnmount () {
      this.removeListener();
    }
  
    render() {
      var leftCount = 0;
      var rightCount = 0;
      return ( 
        <div>         
          <div ref={this.popupRef} class="w3-modal w3-animate-opacity popup">
            <div class="w3-modal-content w3-card-4">
                <header class="w3-container w3-teal"> 
                  <span ref={this.closeRef} class="w3-container w3-display-topright">
                    <svg version="1.1"   x="0px" y="0px"
                         viewBox="0 0 26 26" class="closeButton">
                        <g>
                            <path  d="M21.125,0H4.875C2.182,0,0,2.182,0,4.875v16.25C0,23.818,2.182,26,4.875,26h16.25
                                C23.818,26,26,23.818,26,21.125V4.875C26,2.182,23.818,0,21.125,0z M18.78,17.394l-1.388,1.387c-0.254,0.255-0.67,0.255-0.924,0
                                L13,15.313L9.533,18.78c-0.255,0.255-0.67,0.255-0.925-0.002L7.22,17.394c-0.253-0.256-0.253-0.669,0-0.926l3.468-3.467
                                L7.221,9.534c-0.254-0.256-0.254-0.672,0-0.925l1.388-1.388c0.255-0.257,0.671-0.257,0.925,0L13,10.689l3.468-3.468
                                c0.255-0.257,0.671-0.257,0.924,0l1.388,1.386c0.254,0.255,0.254,0.671,0.001,0.927l-3.468,3.467l3.468,3.467
                                C19.033,16.725,19.033,17.138,18.78,17.394z"/>
                        </g>
                    </svg>
                  </span>
                  <h2>{this.props.Title}</h2>
                </header>
                <div class="w3-container">
                  {this.state.open ? this.props.children : <span />}
                </div>
                <footer class="w3-container w3-teal">
                  <p class="actionsContainer w3-row popupActions">
                      <div class="w3-col s6 w3-left">
                        {this.props.Actions.filter(item=>item.getRight()==false).map(item => 
                          <PopupActionInternal 
                            Item={item} 
                            Last={leftCount+=1 >= this.props.Actions.filter(item=>item.getRight()==false).length}
                            onClose={this.hide.bind(this)} />)}
                      </div>
                      <div class="w3-col s6 w3-right">
                        <div class="rightPopupActions">
                          {this.props.Actions.filter(item=>item.getRight()==true).map(item => 
                            <PopupActionInternal 
                              Item={item} 
                              Last={leftCount+=1 >= this.props.Actions.filter(item=>item.getRight()==true).length}
                              onClose={this.hide.bind(this)} />)}  
                        </div>
                      </div>                      
                    
                  </p>
                
                  
                </footer>
            </div>
          </div>
        </div>     
        
  
        
      );
    }
  }

  export default Popup;
  export {PopupAction};