import Card from '@mui/material/Card';
import CardContent from '@mui/material/CardContent';
import Typography from '@mui/material/Typography';
import {IMeterValueGroup} from '../ViewModel/MeterView';

export interface IValueGroupCardProps {
  group : IMeterValueGroup;  
}

export default function ValueGroupCard(props : IValueGroupCardProps) {
    return (
      <Card sx={{  height: "100%", display: "box", flexDirection: "column",minWidth: 200, minHeight: 150, backgroundColor:undefined}}>                 
            <CardContent>
              <Typography paragraph variant="h5">{props.group.name}</Typography>            
              {props.group.values.map((value) => (
                <Typography>
                  {value.name}: <strong>{value.value}</strong>
                </Typography>                
              ))}                        
            </CardContent>           
      </Card>);
}