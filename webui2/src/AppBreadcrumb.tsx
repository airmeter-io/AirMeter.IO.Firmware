import Typography from '@mui/material/Typography';
import Breadcrumbs from '@mui/material/Breadcrumbs';
import UILink from '@mui/material/Link';
import { Link } from 'react-router-dom';

interface IBreadcrumbEntry {
   title: string,
   to: string | null;
   icon: React.ReactElement<any>;
}

interface IAppBreadcrumbProps {
  breadcrumbs : IBreadcrumbEntry[]
}

function AppBreadcrumb(props : IAppBreadcrumbProps) {
  return (
      <Breadcrumbs aria-label="breadcrumb">
        { props.breadcrumbs.map((crumb, i) =>
            crumb.to !=null ?
            (<UILink underline="hover" sx={{ display: 'flex', alignItems: 'center' }} color="inherit" component={Link} to={crumb.to}>
            {crumb.icon}
            {crumb.title}
          </UILink>) :
            (<Typography sx={{ display: 'flex', alignItems: 'center' }} color="text.primary">
            {crumb.icon}
            {crumb.title}
          </Typography>) ) }         
      </Breadcrumbs>);
}

export default AppBreadcrumb;