import './App.css';
import '@fontsource/roboto/300.css';
import '@fontsource/roboto/400.css';
import '@fontsource/roboto/500.css';
import '@fontsource/roboto/700.css';

import * as React from 'react';
import * as ReactDOM from 'react-dom';

import IconButton from '@mui/material/IconButton';
import AppBar from '@mui/material/AppBar';
import Box from '@mui/material/Box';
import Toolbar from '@mui/material/Toolbar';
import CssBaseline from '@mui/material/CssBaseline';
import LoginIcon from '@mui/icons-material/Login';
import Typography from '@mui/material/Typography';
import Login from './Login';

interface IHeaderProps {
  title: string;
}


class Header extends React.Component<IHeaderProps,{}> {
  private loginRef = React.createRef<Login>();
  handleClickOpen() {
    if(this.loginRef.current!=null)
      this.loginRef.current.handleClickOpen();
  };

  render() {
    
    return (
      <div>
        <CssBaseline />
        <Box sx={{ flexGrow: 1 }}>
          <AppBar position="static">
            <Toolbar>            
              <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
                { this.props.title }
              </Typography>
              <IconButton color="inherit" onClick={this.handleClickOpen.bind(this)} ><LoginIcon /></IconButton>
            </Toolbar>
          </AppBar>
        </Box>
        <Login ref={this.loginRef}/>
    </div>);
  }
}

export default Header;