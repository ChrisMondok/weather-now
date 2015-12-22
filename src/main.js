var iconMap = {
	"clear-day": 8,
	"clear-night": 9,
	"rain": 3,
	"snow": 4,
	"sleet": 7,
	"wind": undefined,
	"fog": undefined,
	"cloudy": 0,
	"partly-cloudy-day": 6,
	"partly-cloudy-night": 6
};

function getWeather() {
	navigator.geolocation.getCurrentPosition(locationSuccess, locationError, {timeout: 15000, maximumAge: 60000});
}

function locationSuccess(geolocation) {
	console.log("Got location");
	
	var url = ("https://api.forecast.io/forecast/4f95e96d2cb2a3b847ce19f232b84a9b/LAT,LNG?exclude=hourly,daily,alerts,flags")
		.replace(/LAT/, geolocation.coords.latitude)
		.replace(/LNG/, geolocation.coords.longitude);
	
	console.log("Getting "+url);
		
	var request = new XMLHttpRequest();
	request.open("GET", url);
	request.addEventListener('load', function() {
		var response = JSON.parse(this.responseText);
		gotWeather(response);
	});
	request.addEventListener('error', function(e) {
		console.log("Got error" + this.responseText);
	});
	request.send();
}

function gotWeather(weather) {
	console.log("Aww yes look at this weather");
	
	var mappedIcon = iconMap[weather.minutely.icon];
	if(mappedIcon === undefined)
		mappedIcon = 1;
	
	var dict = {
		'KEY_TEMPERATURE': Math.round(weather.currently.temperature),
		'KEY_SUMMARY': weather.minutely.summary,
		'KEY_ICON': mappedIcon
	};
	
	Pebble.sendAppMessage(dict, function(e) {
		console.log("Weather sent successfully.");
	}, function(e) {
		console.log("Error sending weather info");
	});
}

function locationError() {
	console.log("Failed to get location");
}

Pebble.addEventListener('ready', function(e) {
	console.log("PebbleKit JS ready!");
	
	getWeather();
});

Pebble.addEventListener('appmessage', function(e) {
	console.log("Got message!");
	
	getWeather();
});