import { TextField, Autocomplete, MenuItem, AutocompleteChangeReason, AutocompleteChangeDetails } from "@mui/material";
import CheckIcon from "@mui/icons-material/Check";
import { IValueReference } from "../../ViewModel/ValueModel";
import { SyntheticEvent } from "react";


interface IValueSelectProps {
    availableValues : IValueReference[];
    setValues : IValueReference[];
    label: string;
    onChanged: (value: IValueReference[]) => void
    disabled: boolean;
}

export default function ValueSelect(props: IValueSelectProps) {
  const selectionChanged = (event: SyntheticEvent<Element, Event>, value: (string | string[])[], reason: AutocompleteChangeReason, details?: AutocompleteChangeDetails<string | string[]> | undefined) => {
    if(value===undefined) return;
    var values : IValueReference[] = value.map((pItem : string) => {
        var splits = pItem.split(".");
        var group = splits[0];
        var name = splits[1];
        var newValue = props.availableValues.find(pX=>pX.Group===group && pX.Name===name);
        if(newValue===undefined) throw "Should not happen!";
        return newValue;
     });
     props.onChanged(values);
  };
  return (
    <Autocomplete
      sx={{ m: 1, width: "100%" }}
      multiple
      disabled={props.disabled}
      options={props.availableValues.map(pValue=>pValue.Group+"."+pValue.Name)}
      value={props.setValues.map(pValue=>pValue.Group+"."+pValue.Name)}
      disableCloseOnSelect
      onChange={selectionChanged}
      renderInput={(params) => (
        <TextField
          {...params}
          variant="outlined"
          label={props.label}
          placeholder={props.label}
        />
      )}
      renderOption={(props, option, { selected }) => (
        <MenuItem
          {...props}          
          value={option}         
          sx={{ justifyContent: "space-between" }}>
          {option}
          {selected ? <CheckIcon color="info" /> : null}
        </MenuItem>
      )}
    />
  );
}