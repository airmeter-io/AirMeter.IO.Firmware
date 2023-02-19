import GeneralSettingsView from './GeneralSettingsView';
import TimeSettingsView from './TimeSettingsView';
import CloudSettingsView from './CloudSettingsView';
import DataSettingsView from './DataSettingsView';
import CalibrationSettingsView from './CalibrationSettingsView';
import MeterView from './MeterView';

class MainView {
    public readonly GeneralSettings = new GeneralSettingsView();
    public readonly TimeSettings = new TimeSettingsView();
    public readonly CloudSettings = new CloudSettingsView();
    public readonly DataSettings = new DataSettingsView();
    public readonly CalibrationSettings = new CalibrationSettingsView();
    public readonly MeterView = new MeterView();
    public static readonly Current = new MainView();
}

export default MainView;