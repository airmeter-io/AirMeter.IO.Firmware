import React from 'react';
import ReactDOM from 'react-dom';

class MenuItemDesc {
    constructor(pName, pDescription, pSvg, pRenderContent) {
        this.name = pName;
        this.description = pDescription;
        this.svg = pSvg;
        this.renderContent = pRenderContent;
    }
}

class MenuItem extends React.Component {
  constructor(props) {
    super(props);
    this.buttonRef = React.createRef();
  }

  handleClick() {
    console.log("button clicked");
    if(!this.props.IsSelected) {
      this.props.onClick(this.props.Item);
    }
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
    if(this.props.IsSelected) 
      return (
        <div class="menuItemSelected" ref={this.buttonRef}>
          {this.props.Item.svg()}
          <div>{this.props.Item.name}</div>
        </div>
      
      );
    else
      return (
          <div class="menuItem" ref={this.buttonRef}>
            {this.props.Item.svg()}
            <div>{this.props.Item.name}</div>
          </div>
        
      );
  }
}

class Page extends React.Component {
    constructor(props) {
      super(props);
      this.state = {
          selected: this.props.menuItems[0]
      };
      this.buttonRef = React.createRef();
      this.popupRef = React.createRef();
      this.closeRef = React.createRef();
    
    }

    selectItem(item)  {
      this.setState({ 
        selected: item 
      });     
      this.popupRef.current.style.display = "none"; 
    }

    handleClick() {
      console.log("button clicked");
      this.popupRef.current.style.display = "block";
    }

    handleClose() {
      console.log("button clicked");
      this.popupRef.current.style.display = "none";
    }

    componentDidMount () {
      const listenerFn = this.handleClick.bind(this);
      const closeListenerFn = this.handleClose.bind(this);
      this.buttonRef.current.addEventListener("click", listenerFn);
      this.closeRef.current.addEventListener("click", closeListenerFn);
      this.removeListener = () => {
        this.buttonRef.current.removeEventListener('click', listenerFn);
        this.buttonRef.current.removeEventListener('click', closeListenerFn);
      };
    }
    componentWillUnmount () {
      this.removeListener();
    }
  
    render() {
      return [
        
        <header class="w3-container w3-teal">
          <div class="header">
            <div class="leftHeader">
              <div>
                {this.state.selected.svg()}
                <h1>{this.state.selected.name}</h1>
              </div>
            </div>
            
            <div class="burgerMenuContainer">
              <svg viewBox="0 0 100 80" class="burgerMenu" ref={this.buttonRef}>
                <rect width="100" height="20"></rect>
                <rect y="30" width="100" height="20"></rect>
                <rect y="60" width="100" height="20"></rect>
              </svg>
            </div>
          </div>
        </header>,
       <div ref={this.popupRef} class="w3-modal w3-animate-opacity">
       <div class="w3-modal-content w3-card-4" class="burgerMenuPopup">
           <header class="w3-container w3-teal"> 
             <span ref={this.closeRef} class="w3-container w3-display-topright">
               <svg version="1.1" id="Capa_1"   x="0px" y="0px"
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
             <div class="menuContainer">
            {this.props.menuItems.map(item => <MenuItem  Item={item} IsSelected={item==this.state.selected} onClick={this.selectItem.bind(this)}/>)}
            </div>      
           </header>
                
       </div>
     </div>,
        <div class="w3-container">
          {this.state.selected.renderContent()}

            <div class="w3-bar w3-black">
                
            </div>
            
            
        </div>
        
      ];
    }
  }

export default Page;
export {MenuItemDesc};