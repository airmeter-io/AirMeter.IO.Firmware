import React from 'react';

import './sidebar.css'

class Sidebar extends React.Component {
    constructor(props) {
      super(props);
      this.buttonRef = React.createRef();
      this.closeRef = React.createRef();
      this.sidebarRef = React.createRef();
    }
  
    handleShow() {
        this.sidebarRef.current.style.display = "block";
    }
  
    handleHide() {
      this.sidebarRef.current.style.display = "none";
    }
  
    componentDidMount () {
      const showListenerFn = this.handleShow.bind(this);
      const hideListenerFn = this.handleHide.bind(this);
      this.buttonRef.current.addEventListener("click", showListenerFn);
      this.closeRef.current.addEventListener("click", hideListenerFn);
      this.removeListener = () => this.buttonRef.current.removeEventListener('click', showListenerFn)
    }
    componentWillUnmount () {
      this.removeListener();
    }
  
    render() {
      return [
        <div ref={this.sidebarRef} class="w3-sidebar w3-bar-block w3-dark-grey w3-animate-right" >
            <div class="w3-container" ref={this.closeRef}>
                <svg version="1.1" id="Capa_1"   x="0px" y="0px"  viewBox="0 0 26 26" class="closeButton">
                        <g>
                            <path  d="M21.125,0H4.875C2.182,0,0,2.182,0,4.875v16.25C0,23.818,2.182,26,4.875,26h16.25
                                C23.818,26,26,23.818,26,21.125V4.875C26,2.182,23.818,0,21.125,0z M18.78,17.394l-1.388,1.387c-0.254,0.255-0.67,0.255-0.924,0
                                L13,15.313L9.533,18.78c-0.255,0.255-0.67,0.255-0.925-0.002L7.22,17.394c-0.253-0.256-0.253-0.669,0-0.926l3.468-3.467
                                L7.221,9.534c-0.254-0.256-0.254-0.672,0-0.925l1.388-1.388c0.255-0.257,0.671-0.257,0.925,0L13,10.689l3.468-3.468
                                c0.255-0.257,0.671-0.257,0.924,0l1.388,1.386c0.254,0.255,0.254,0.671,0.001,0.927l-3.468,3.467l3.468,3.467
                                C19.033,16.725,19.033,17.138,18.78,17.394z"/>
                        </g>
                    </svg>
            </div>
            <div class="sidebarContent">
             {this.props.children}
            </div>
            
        </div>,
        <button class="infobutton" ref={this.buttonRef}>
            <svg version="1.1" id="Capa_1"  x="0px" y="0px"  viewBox="0 0 490.318 490.318" >
                <g>
                    <g>
                        <g>
                            <path d="M245.148,0C109.967,0,0.009,109.98,0.009,245.162c0,135.182,109.958,245.156,245.139,245.156
                                c135.186,0,245.162-109.978,245.162-245.156C490.31,109.98,380.333,0,245.148,0z M245.148,438.415
                                c-106.555,0-193.234-86.698-193.234-193.253c0-106.555,86.68-193.258,193.234-193.258c106.559,0,193.258,86.703,193.258,193.258
                                C438.406,351.717,351.706,438.415,245.148,438.415z"/>
                            <path d="M270.036,221.352h-49.771c-8.351,0-15.131,6.78-15.131,15.118v147.566c0,8.352,6.78,15.119,15.131,15.119h49.771
                                c8.351,0,15.131-6.77,15.131-15.119V236.471C285.167,228.133,278.387,221.352,270.036,221.352z"/>
                            <path d="M245.148,91.168c-24.48,0-44.336,19.855-44.336,44.336c0,24.484,19.855,44.34,44.336,44.34
                                c24.485,0,44.342-19.855,44.342-44.34C289.489,111.023,269.634,91.168,245.148,91.168z"/>
                        </g>
                    </g>
                </g>            
            </svg>
        </button>
      ];
    }
  }

export default Sidebar;
