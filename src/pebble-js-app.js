var configuration = {};

Pebble.addEventListener("ready", function(e) {
    "use strict";
  console.log("Pebble JS ready: ");
});

Pebble.addEventListener("showConfiguration",
  function(e) {

    /*if(Pebble.getActiveWatchInfo) {
      var watchinfo=Pebble.getActiveWatchInfo();
      var platform=watchinfo.platform;
    } else {
      platform="aplite";
    } */
    
    Pebble.openURL('https://www.googledrive.com/host/0B9NFRZUyn1ZpM3dtNVFHUk5VaHM');//Aplite
      
/*    if(platform!="aplite") {
      Pebble.openURL('https://www.googledrive.com/host/0B9NFRZUyn1ZpTUUtcGgzaHZ5Sjg');//Basalt
      } else {
      Pebble.openURL('https://www.googledrive.com/host/0B9NFRZUyn1ZpOGZIQnBqRm9YN1E');//Aplite
    }
    console.log("Platform: ") + platform;*/
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
    
//    localStorage.setItem('KEY_WEEK_TYPE', configuration.weekType);
    
    //Send Week Type to Pebble, persist there
    Pebble.sendAppMessage(
      {"KEY_WEEK_TYPE": configuration.week_select,
       "KEY_DATE_FORMAT": configuration.date_format,
       "KEY_DATE_DELI": configuration.date_deli,
       "KEY_SYM_BATT": configuration.batt_select,
       "KEY_SEC_COL": configuration.second_select,
       "KEY_BATT_LVL": configuration.batt_lvl_select,
       "KEY_SEC_VIS": configuration.second_vis,
       "KEY_BATT_PERC": configuration.batt_perc},
      
      function(e) {
        console.log("Sending week settings data...");
      },
      function(e) {
        console.log("Settings week feedback failed!");
      }
    );
  }
);