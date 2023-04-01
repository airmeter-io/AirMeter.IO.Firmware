import { namespaces } from "../../i18n/i18n.constants";
import { useTranslation } from "react-i18next";
import FormControl from '@mui/material/FormControl';
import FormGroup from '@mui/material/FormGroup';
import Stack from '@mui/material/Stack';
import * as React from 'react';
import LoadingButton from '@mui/lab/LoadingButton';
import Button from '@mui/material/Button';
import SaveIcon from '@mui/icons-material/Save';
import CancelIcon from '@mui/icons-material/Cancel';
import Backdrop from '@mui/material/Backdrop';
import CircularProgress from '@mui/material/CircularProgress';
import { useNavigate } from "react-router-dom";
import { ReactNode } from 'react';

type IFormScreenProps = {
  children: ReactNode[] | ReactNode;
  onSave : () => Promise<void>;
  onLoad : () => Promise<void>;
  closePath : string;
}

export default function FormScreen(props : IFormScreenProps) {
  const [state, setState] = React.useState({
    isSaving: false,
    isLoading: false,
  });
  const navigate = useNavigate();
  const { t } = useTranslation(namespaces.settings);
  React.useEffect(() => {
    (async () => {
      setState({
        ...state,
        isLoading: true,
      });
      await props.onLoad();
      setState({
        ...state,
        isLoading: false,
      });
      
    })();
  }, []);
  const handleSave = async () => {
    setState({
      ...state,
      isSaving: true,
    });
    await props.onSave();
    setState({
      ...state,
      isSaving: false,
    });
    navigate(props.closePath);
  };

  const handleCancel = async () => {
    navigate(props.closePath);
  };
   return (      
  <FormControl component="fieldset" variant="standard" >    
    <Backdrop
      sx={{ color: '#fff', zIndex: (theme) => theme.zIndex.drawer + 1 }}
      open={state.isLoading}>
      <CircularProgress color="inherit" />
    </Backdrop>
    { props.children }
    <FormGroup sx={{mt: "2em"}}>
      <Stack spacing={2} direction="row">
        <Button       
          startIcon={<CancelIcon />}
          onClick = {handleCancel}
          variant="outlined">
          {t("buttons.close")}
        </Button>
        <LoadingButton
          loading={state.isSaving}
          loadingPosition="start"
          startIcon={<SaveIcon />}
          onClick = {handleSave}
          variant="contained">
          {t("buttons.save")}
        </LoadingButton>
      </Stack>
    </FormGroup>        
  </FormControl>  );
}

