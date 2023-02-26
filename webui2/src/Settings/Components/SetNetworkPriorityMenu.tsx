import * as React from 'react';
import { styled, alpha } from '@mui/material/styles';
import Button from '@mui/material/Button';
import Menu, { MenuProps } from '@mui/material/Menu';
import MenuItem from '@mui/material/MenuItem';
import KeyboardArrowDownIcon from '@mui/icons-material/KeyboardArrowDown';
import {IAvailableNetworkPriority} from '../../ViewModel/WirelessSettingsView';
import ArrowDropUpIcon from '@mui/icons-material/ArrowDropUp';
import ArrowDropDownIcon from '@mui/icons-material/ArrowDropDown';
import { useTranslation  } from "react-i18next";
import {i18n} from "../../i18n/i18n";
import { namespaces } from "../../i18n/i18n.constants";
const StyledMenu = styled((props: MenuProps) => (
  <Menu
    elevation={0}
    anchorOrigin={{
      vertical: 'bottom',
      horizontal: 'right',
    }}
    transformOrigin={{
      vertical: 'top',
      horizontal: 'right',
    }}
    {...props}  />
))(({ theme }) => ({
  '& .MuiPaper-root': {
    borderRadius: 6,
    marginTop: theme.spacing(1),
    minWidth: 180,
    color:
      theme.palette.mode === 'light' ? 'rgb(55, 65, 81)' : theme.palette.grey[300],
    boxShadow:
      'rgb(255, 255, 255) 0px 0px 0px 0px, rgba(0, 0, 0, 0.05) 0px 0px 0px 1px, rgba(0, 0, 0, 0.1) 0px 10px 15px -3px, rgba(0, 0, 0, 0.05) 0px 4px 6px -2px',
    '& .MuiMenu-list': {
      padding: '4px 0',
    },
    '& .MuiMenuItem-root': {
      '& .MuiSvgIcon-root': {
        fontSize: 18,
        color: theme.palette.text.secondary,
        marginRight: theme.spacing(1.5),
      },
      '&:active': {
        backgroundColor: alpha(
          theme.palette.primary.main,
          theme.palette.action.selectedOpacity,
        ),
      },
    },
  },
}));


interface ISetNetworkPriorityMenuProps {
    priorities : IAvailableNetworkPriority[];
    currentPriority : number;
    onPriorityClick : (pPriority : number) => void;
    isCurrent : boolean;
}

export default function SetNetworkPriorityMenu(props : ISetNetworkPriorityMenuProps) {
  const [anchorEl, setAnchorEl] = React.useState<null | HTMLElement>(null);
  const open = Boolean(anchorEl);
  const { t } = useTranslation(namespaces.settings);
  const handleClick = (event: React.MouseEvent<HTMLElement>) => {
    setAnchorEl(event.currentTarget);
  };
  const handleClose = () => {
    setAnchorEl(null);
  };

  const handleSelect = (pPriority : number) => {
    props.onPriorityClick(pPriority);
    handleClose();
   
  };

  return (
    <div>
      <Button
        aria-controls={open ? 'demo-customized-menu' : undefined}
        aria-haspopup="true"
        aria-expanded={open ? 'true' : undefined}
        disableElevation
        onClick={handleClick}
        endIcon={<KeyboardArrowDownIcon />}>
        {t("wireless.card.moveUpPriority", {priority: props.currentPriority+1})}
      </Button>
      <StyledMenu
        id="demo-customized-menu"
        MenuListProps={{
          'aria-labelledby': 'demo-customized-button',
        }}
        anchorEl={anchorEl}
        open={open}
        onClose={handleClose}>
          {props.priorities.map(pPriority => <MenuItem onClick={()=>handleSelect(pPriority.priority)} disableRipple>
          {pPriority.up ? <ArrowDropUpIcon/> : <ArrowDropDownIcon/>}
          {pPriority.up ? t("wireless.card.moveUpPriority", {priority: pPriority.priority+1}) : 
                          t("wireless.card.moveDownPriority", {priority: pPriority.priority+1})}
        </MenuItem>)}       
      </StyledMenu>
    </div>
  );
}