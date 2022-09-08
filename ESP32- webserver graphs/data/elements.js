window.addEventListener('load', getReadings);
class Elements
{
    static GetHeadings()
    {
        return ["Nosaukums", "Vertiba", "Mervienuba"];
    }

    static GetProperties()
    {
        return ["name", "atomicnumber", "symbol"];
    }

    static GetElements()
    {
        return [
        {
            name: "Lat",
            atomicnumber: 1,
            symbol: "H"
        },
        {
            name: "Lon",
            atomicnumber: 2,
            symbol: "He"
        },
        {
            name: "Alt",
            atomicnumber: 3,
            symbol: "Li"
        }
    ]
    }
}
/*
    function plotGPS(jsonValue) {

        var keys = Object.keys(jsonValue);
        console.log(keys);
        console.log(keys.length);
      
        for (var i = 0; i < keys.length; i++){
          var x = (new Date()).getTime();
          console.log(x);
          const key = keys[i];
          var y = Number(jsonValue[key]);
          console.log(y);
      
          if(chartT.series[i].data.length > 40) {
            chartT.series[i].addPoint([x, y], true, true, true);
          } else {
            chartT.series[i].addPoint([x, y], true, false, true);
          }
        }
      }

    function getReadings(){
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            console.log(myObj);
            plotTemperature(myObj);
          }
        };
        xhr.open("GET", "/readings", true);
        xhr.send();
      }
      
      if (!!window.EventSource) {
        var source = new EventSource('/events');
      
        source.addEventListener('open', function(e) {
          console.log("Events Connected");
        }, false);
      
        source.addEventListener('error', function(e) {
          if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
          }
        }, false);
      
        source.addEventListener('message', function(e) {
          console.log("message", e.data);
        }, false);
      
        source.addEventListener('new_readings', function(e) {
          console.log("new_readings", e.data);
          var myObj = JSON.parse(e.data);
          console.log(myObj);
          plotTemperature(myObj);
        }, false);
      }
*/
