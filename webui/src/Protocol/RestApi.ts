export default class CommandManager {
    async executeCommand(pCommand : string, pPopulateCommand : ((cmd: any)=>void) | undefined = undefined, pTimeout : number | undefined = undefined) : Promise<any> {
        var cmd = { COMMAND: pCommand};
        if(pPopulateCommand!=undefined) {
            pPopulateCommand(cmd);
        }
        const requestOptions = {

            method: 'POST',
            headers: { 'Content-Type': 'application/json;charset=UTF-8' },
            body: JSON.stringify(cmd),
            signal: pTimeout===undefined? undefined :  AbortSignal.timeout(pTimeout)
        };


        try {
            var response = await fetch('/command', requestOptions);
            var json = await response.json();
            return json;
        } catch(e) {
            console.log("Error Executing Command: "+e.toString());
            throw e;
        }
    }
}
