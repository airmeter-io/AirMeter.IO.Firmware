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
import { ReactNode } from 'react';
import MenuIcon from '@mui/icons-material/Menu';
import Drawer from '@mui/material/Drawer';
import Divider from '@mui/material/Divider';

interface IHeaderProps {
  title: string;
  children?: ReactNode[] | ReactNode;
}

interface IHeaderState {
  open : boolean;
}

const drawerWidth = 340;


class Header extends React.Component<IHeaderProps,IHeaderState> {
  state = {
    open: false
  };
  private loginRef = React.createRef<Login>();
  handleClickOpen() {
    if(this.loginRef.current!=null)
      this.loginRef.current.handleClickOpen();
  };

  handleDrawOpen() {
    this.setState( { open: true});
  }

  handleOnClose(event: object, reason: string) {
    this.setState( { open: false});
  }

  render() {
    
    return (
      <div>
        <CssBaseline />
        <Box sx={{ flexGrow: 1 }}>
          <AppBar position="static">
            <Toolbar>  
              { this.props.children!==undefined &&              
              <IconButton
                color="inherit"
                aria-label="open drawer"
                onClick={this.handleDrawOpen.bind(this)}
                edge="start"
                sx={{ mr: 2, ...(this.state.open && { display: 'none' }) }}>
                 <MenuIcon />
              </IconButton> }
              <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
                { this.props.title }
              </Typography>
              <IconButton color="inherit" onClick={this.handleClickOpen.bind(this)} ><LoginIcon /></IconButton>
            </Toolbar>
          </AppBar>
        </Box>
        { this.props.children!==undefined && <Drawer
        sx={{
          width: drawerWidth,
          flexShrink: 0,
          '& .MuiDrawer-paper': {
            width: drawerWidth,
            boxSizing: 'border-box',
          },
        }}
        anchor="left"
        onClose={this.handleOnClose.bind(this)}
        open={this.state.open}
        onClick={this.handleOnClose.bind(this)}
      >
       
        <Divider />
         { this.props.children }
      </Drawer>}
        <Login ref={this.loginRef}/>
    </div>);
  }
}

export default Header;