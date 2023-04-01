import { namespaces } from "./i18n.constants";

export const en = {
  [namespaces.settings]: {
    title: "Manage Device Settings",
    breadcrumb: "Settings",
    buttons: {
       save: "Save",
       close: "Close",
       cancel: "Cancel",
       skip: "Skip",
       confirm: "Confirm",
       remove: "Remove",
       connect: "Connect"
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
        },
        add: {
          title: "Add Wireless Network",
          password: "Network Password",
          step1: {
            label: "Search for networks",
            nextLabel: "Search",
            para1: "Click the Search button to initiate a scan for wireless that your device may be able to connect to.",
            para2: "Ensure that the device is as near as possible (whilst still being accessible via the current connection) to the access point which you wish to add."
          },
          step2: {
            label: "Choose network",
            nextLabel: "Choose",
            para1: "Click the Search button to initiate a scan for wireless that your device may be able to connect to.",
            para2: "Ensure that the device is as near as possible (whilst still being accessible via the current connection) to the access point which you wish to add."
          },
          step3: {
            label: "Authenticate & Test",
            nextLabel: "Test & Add Connection",
            para1: "The {{network}} network you selected on the previous step requires password as it is secured with {{authMode}}. Please enter the network password in the box below, choose whether to switch to the new network now and whether to make it default, then press 'Test & Add Connection' to continue.",
            para2: "",
            connectAfter: {
              label: "Switch to this network."
            },
            makeDefault: {
              label: "Make default network."
            }
          }
        },
        remove: {
          title: "Remove configuration for {{ssid}}",
          para1: "This will perminantly remove this network configuration and therefore this device will no longer attempt to connect to this network.",          
        },
        card: {
          currentPriority: "Priority {{priority}}",
          moveUpPriority: "Move up to priority {{priority}}",
          moveDownPriority: "Move down to priority {{priority}}",
          setPriority: "Priority {{priority}}"
        },
        authModes: {
          WIFI_AUTH_OPEN: "Open",
          WIFI_AUTH_WEP: "WEP",
          WIFI_AUTH_WPA_PSK: "WPA Personal",
          WIFI_AUTH_WPA2_PSK: "WPA2 Personal",
          WIFI_AUTH_WPA_WPA2_PSK: "WPA/WPA2 Personal",
          WIFI_AUTH_WPA2_ENTERPRISE: "WPA2 Enterprise",
          WIFI_AUTH_WPA3_PSK: "WPA3 Personal",
          WIFI_AUTH_WPA2_WPA3_PSK: "WPA2/WPA3 Personal",
          WIFI_AUTH_WAPI_PSK: "WAPI Personal",
          WIFI_AUTH_OWE: "Enhanced Open (OWE)"
        }


    },
    calibration: {
        title: "Calibration",
        description: "Allows calibration of sensors within this device and allows configuration of calibration settings.",
        breadcrumb: "Calibration",
        manual: {
          title: "Perform Calibration",
          breadcrumb: "Perform",
          description: "Manually calibrate the CO2 sensor to background (outside) levels.",
          resetLabel: "Repeat Calibration",
          step1: {
            label: "Place device outside",
            nextLabel: "Next",
            para1: "Outside air that is away from sources of pollution has a relatively constant CO2 just above 400ppm. The CO2 sensor can use this to calibrate against as an approximate baseline value. Before calibration is performed it must be placed outside.",
            para2: "The weather conditions must be dry, not excessively hot and the sensor avoid the following:-",
            tip1: "Sources of pollution such as motor vehicles, home heating boilers/burners or any other combustion source.",
            tip2: "Sources of heat such as direct sunlight or radiant heat.",
            tip3: "People exhaling are also sources of CO2 which includes yourself.",
            tip4: "Excessively humid conditions (> 80%), precipitation such as rain or placing the device in standing water!"
          },
          step2: {
            label: 'Wait for readings to settle',
            nextLabel: "Next",
            para1: "After ensuring that the sensor is located outside as per the instructions on the previous screen. you must now wait for the sensor readings to stablise before you can complete the calibration process. This is because the reading taken during calibration will be assumed to represent the ambient CO2 concentration level of outdoor air (E.g. around 400ppm).",
            para2: "During this time leave the sensor in position in the outdoor air and ensure that it is not obstructed or interfered with.",
            para3: "Wait will be complete in: {{time}} after which you will be able to press Next to start the calibration.",
          },
          step3: {
            label: 'Perform Calibration',
            nextLabel: "Calibrate",
            para1: "As the previous steps have now been completed the device can now be calibrated.",
            para2: "Initiate the calibration by pressing the Calibrate button.",
            failed: {
              para1: "Something went wrong and the device failed to calibrate. You can try again by clicking the calibrate button again.",
              para2: "The sensor reported: "
            }
          },
          finished: {
            para1: "The CO2 sensor calibration sequence has been completed successfully. When the readings are next updated you will see the newly calibrated value.",
            para2: "To ensure the sensor is fully calibrated check it outside at different times of the day. If the readings drop below 400ppm then the sensor may benefit from another calibration.",
            para3: "Note that CO2 levels do fluctuate outdoors therefore some trial and error diagnostics is required to find the optimum time of day & location for calibration.",
          }

        },
        options: {
          title: "Configure Calibration Options",
          breadcrumb: "Configure Options",
          description: "Configure options including automatic (ABC) calibration and the baseline CO2 value.",
          frequencies: {
            hours: "{{hours}} hours",
            days: "{{days}} days",                      
            daysHours: "{{days}} days and {{hours}} hours"
          },
          enableAbc: {
            title: "Enable Automatic Baseline Calibration (ABC)",
            helper: "Automatic Baseline Calibration (ABC) is a mode in which the CO2 sensor automatically calibrates itself against the minimum reading it receives over a period. It works most effectively in locations where background (e.g. outdoor) CO2 levels are reached during each calibration period."
          }, 
          frequency: {
            label: "The calibration cycle length is {{frequency}}.",
            helper: "This sets how long the ABC calibration cycle will be. Consider how often the device will experience background (e.g. outdoor) CO2 levels."
          },
          baseline: {
            label: "The background CO2 level is assumed to be {{baseline}} ppm",
            helper: "This sets what background CO2 level is assumed when performing calibration."
          }        
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