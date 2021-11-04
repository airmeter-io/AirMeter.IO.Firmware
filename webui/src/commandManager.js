import React from 'react';

class CommandManager extends React.Component {
    constructor(props) {
      super(props);
      this.state = { running: false };
    }

    static executeCommand(pBackground, pCommand, pOnDone, pPopulateParams) {
        var cmd = { COMMAND: pCommand};
        if(pPopulateParams!=undefined) {
            pPopulateParams(cmd);
        }
        const requestOptions = {

            method: 'POST',
            headers: { 'Content-Type': 'application/json;charset=UTF-8' },
            body: JSON.stringify(cmd)
        };

      
        fetch('command', requestOptions)
            .then(response => {return response.json();})
            .then(data => pOnDone(data)).catch(function() {
                console.log("Error Executing Command");
          });;      
    }

    render() {
      
      return [
        <div />     
      ];
    }

}

export default CommandManager;