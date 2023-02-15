import { namespaces } from "./i18n.constants";

export const en = {
  [namespaces.settings]: {
    title: "Manage Device Settings",
    breadcrumb: "Settings",
    buttons: {
       save: "Save"
    },
    general: {
        title: "General",
        description: "Change settings such as the device name.",
        breadcrumb: "General",
        deviceName: {
          title: "Device Name",
          helper: "This is the name of the device used to identify it on the network."
        },
        powerSave: {
          title: "Enable power saving mode"
        }

    },
    wireless: {
        title: "Wireless Networking",
        description: "Configures wireless networks that this device will connect to and other wireless connectivity settings.",
        breadcrumb: "Wifi",
        networks: {
          title: "Manage Networks",
          description: "Manage the list of wireless networks that this device will attempt to connect to.",
          breadcrumb: "Manage Networks"
        },
        options: {
          title: "Configure Options",
          description: "Configure options that control how and when this device will attempt to connect to available networks.",
          breadcrumb: "Configure Options"
        }
    },
    calibration: {
        title: "Calibration",
        description: "Allows calibration of sensors within this device and allows configuration of calibration settings.",
        breadcrumb: "Calibration",
        manual: {
          title: "Perform Manual Calibration",
          breadcrumb: "Perform Manual",
          description: "Manually calibrate the CO2 sensor to background (outside) levels."
        },
        automatic: {
          title: "Configure Automatic Calibration",
          breadcrumb: "Configure Automatic",
          description: "Enable/disable automatic (ABC) CO2 calibration and configure its settings."
        }
    }, 
    data: {
        title: "Data Logging & Frequency",
        description: "Configure which parameters are logged and how often readings are taken from the sensors.",
        breadcrumb: "Data",
        frequency: {
          label: "Readings will be taken from sensors and written to the data log {{frequency}}."
        }
    },    
    cloud: {
        title: "Cloud Services",
        description: "Configures publishing of sensor readings to cloud services using the MQTT protocol.",
        breadcrumb: "Cloud",
        mqtt: {
          enableTitle: "Enable publishing using MQTT",
          serverAddress: {
            title: "MQTT Server Address",
            helper: "The MQTT server address must be a URL starting with either mqtt:// or mqtts://"
          },
          topicPath: {
            title: "MQTT Topic Path",
            helper: "Specify where in the hierarchy of topics to publish, '%DEVICE_NAME%' is replaced by the device name. For example 'CO2Monitors/%DEVICE_NAME%'."
          },
          frequencyLabel: "Readings will be published to the MQTT server every {{frequency}}."
        }
    }, 
    ntp: {
        title: "Network Time Protocol",
        description: "Configuration of network time protocol (NTP) settings including which servers to use or whether to request them from DHCP.",
        breadcrumb: "NTP",
        dhcp: {
          title: "Use NTP servers from by DHCP if they are provided."
        },
        ntp1: {
          title: "Primary NTP Server",
          helper: "The IP or DNS of the primary NTP server"
        },
        ntp2: {
          title: "Secondary NTP Server",
          helper: "The IP or DNS of the secondary NTP server"
        },
    }, 
    users: {
        title: "Manage Users",
        description: "Manage users of this device and what level of access they have.",
        breadcrumb: "Users"
    },   
    
    frequencies: {
       thirtySeconds: "thirty seconds",
       minute: "minute",
       twoMinutes: "two minutes",
       threeMinutes: "three minutes",
       fourMinutes: "four minutes",
       fiveMinutes: "five minutes",
       tenMinutes: "ten minutes",
       fifteenMinutes: "fifteen minutes",
       thirtyMinutes: "thirty minutes",
       hour: "hour"
    }
  },
  [namespaces.login]: {
    title: "Login",
    description: "To login to this device please enter your username and password.",    username: "Username",
    password: "Password",
    buttons: {
      cancel: "Cancel",
      login: "Login"
    }    
  },
  [namespaces.pages.hello]: {
    welcome: "Welcome",
  },
};

export const gle = {
  [namespaces.settings]: {
    buttons: {
      ok: "Ok",
      cancel: "Cancel",
    },
  },
  [namespaces.login]: {
    title: "To login to this device please enter your username and password.",
    username: "Username",
    password: "Password",
    buttons: {
      cancel: "Cancel",
      login: "Login"
    }    
  },
  [namespaces.pages.hello]: {
    welcome: "Welcome",
  },
};