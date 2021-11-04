import React from 'react';
import Popup from './popup';
import {PopupAction} from "./popup.js";

class WizardStep {
    constructor(pRender, pTitle, pCanNext, pCanPrevious,  pProcessNext, pAdditionalCommands) {
        this.titleStr = pTitle;
        this.renderFunc = pRender;
        this.canNextFunc = pCanNext;
        this.canPreviousFunc = pCanPrevious;
        this.processNext = pProcessNext;
        this.additionalCommands = pAdditionalCommands;
    }

    render() {
        return this.renderFunc();
    }
    canNext() {
        return this.canNextFunc();
    }

    canPrevious() {
        return this.canPreviousFunc();
    }

    next() {
        if(this.processNext!=undefined)
            return this.processNext();
        return true;
    }

    getTitle() {
        return this.titleStr;
    }



    getAdditionalCommands() {
        if(this.additionalCommands!=undefined) 
            return this.additionalCommands();
        return [];
    }
}


class Wizard extends React.Component {
    constructor(props) {
      super(props);      
      this.state = { step: 0, actions: []};
      this.popup = React.createRef();
    }

    canNext() {
        return this.state.step< this.props.Steps.length - 1 && this.props.Steps[this.state.step].canNext();
    }

    canPrevious() {
        return this.state.step>0 && this.props.Steps[this.state.step].canPrevious();
    }

    show() {
        this.state.step = 0;
        this.updateActions();
        this.popup.current.show();
    }

    updateActions() {
        var result = [];
        if(this.canPrevious()) {
            result.push(new PopupAction("<< Previous",()=>{
                this.setState({step: this.state.step-1});
                this.updateActions();
                return false;
            }));
        }
        if(this.canNext()) {
            result.push(new PopupAction("Next >>", ()=>{
                if(this.props.Steps[this.state.step].next()) {
                    this.setState({step: this.state.step+1});
                    this.updateActions();
                }
                return false;
            },true));
        }

        result = result.concat(this.props.Steps[this.state.step].getAdditionalCommands());

        this.setState({actions: result });
    }

    onShow() {
        if(this.props.OnShow!=undefined)
            this.props.OnShow();
    }

    onHide() {
        if(this.props.OnClose!=undefined)
            this.props.OnClose();
    }

    render() {
      
      return ( 
        <Popup ref={this.popup} Actions={this.state.actions} OnShow={this.onShow.bind(this)} OnClose={this.onHide.bind(this)} Title={this.props.Steps[this.state.step].getTitle()} >
            {this.props.Steps[this.state.step].render()}
        </Popup>);
    }
  }

  export default Wizard;
  export {WizardStep};