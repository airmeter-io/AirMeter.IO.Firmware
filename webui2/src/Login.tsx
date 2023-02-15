import * as React from 'react';
import TextField from '@mui/material/TextField';
import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogContent from '@mui/material/DialogContent';
import DialogContentText from '@mui/material/DialogContentText';
import DialogTitle from '@mui/material/DialogTitle';
import Button from '@mui/material/Button';
import { namespaces } from "./i18n/i18n.constants";
import { withTranslation  } from "react-i18next";
import {i18n} from "./i18n/i18n";
interface ILoginState {
    open : boolean;
}

class Login extends React.Component<{},ILoginState> {
  state = { open: false };

  handleClickOpen()  {
    this.setState({
      open: true
    });
  }

  handleClose() {
    this.setState({
      open: false
    });
  }


  render() {
    const t = i18n.getFixedT(null, namespaces.login);
    return (
      <div>
      <Dialog open={this.state.open} onClose={this.handleClose}>
            <DialogTitle>{t("title")}</DialogTitle>
            <DialogContent>
              <DialogContentText>{t("description")}</DialogContentText>
              <TextField
                autoFocus
                margin="dense"
                id="name"
                label={t("username")}
                type="text"
                fullWidth
                variant="standard"
                
              />
              <TextField
                margin="dense"
                id="name"
                label={t("password")}
                type="password"
                fullWidth
                variant="standard"
              />
            </DialogContent>
            <DialogActions>
              <Button onClick={this.handleClose.bind(this)}>{t("buttons.cancel")}</Button>
              <Button variant="contained" onClick={this.handleClose.bind(this)}>{t("buttons.login")}</Button>
            </DialogActions>
          </Dialog>
          </div>);
  }
}

export default  Login;