var base = "https://api.wunderlist.com/";
var token;
var loginData;
var lists;
var todayList = [];
var username = "jahdaic@yahoo.com";
var password = "blastoise";
var body;

function login(action, data) {
	title += "Starting Login\n";
	var response;
	
	var req = new XMLHttpRequest();
	req.open('POST', base + "login", true);
	req.onload = function(e)
	{
		if (req.readyState == 4)
		{
			if(req.status == 200) 
			{
				console.log(req.responseText);
				response = JSON.parse(req.responseText);
				
				if (response && response.token)
				{
					token = response.token;
					Pebble.sendAppMessage({
						"title":"Done"
					});
				}

			}
			else
			{
				console.log("Error");
				body += req.status+"\n";
				Pebble.sendAppMessage({
					"title":"Error",
					"body":body
				});
			}
		}
	}
	req.send("email="+username+"&password="+password);
}

// Initialize the App
Pebble.addEventListener(
	"ready",
	function(e) {
		console.log("connect!" + e.ready);
		login();
	}
);

// Every time the App sends a message
Pebble.addEventListener(
	"appmessage",
	function(e) {
		console.log("message!");
		var ran = Math.floor((Math.random()*5)+1);
		
		/*if(ran == 1) {title="vagina";}
		if(ran == 2) {title="boob";}
		if(ran == 3) {title="stuff";}
		if(ran == 4) {title="poop";}
		if(ran == 5) {title="nerf!";}
		
		Pebble.sendAppMessage({
			"title":title
		});*/
	}
);

// Show the configuration page
Pebble.addEventListener(
	"showConfiguration",
	function(e) {
		console.log("Configuration Window Opened");
		Pebble.openURL("http://intrepidwebdesigns.com/WunderPebble/config/");
	}
);

// Configuration page closed
Pebble.addEventListener(
	"webviewclosed",
	function(e) {
		console.log("Configuration Window Closed");
		
		var data = JSON.parse(e.response);
		
		window.localStorage.username = e.username;
		window.localStorage.password = e.password;
		
		login();
	}
);