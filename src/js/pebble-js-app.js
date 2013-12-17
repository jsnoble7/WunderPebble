var base = "https://api.wunderlist.com";
var token;
var loginData;
var lists = [];
var todayList = [];
var title;
var body;
var response;
var req;

function getTasks(list)
{
	//Pebble.showSimpleNotificationOnPebble("Update", "Getting Tasks");
	
	req = new XMLHttpRequest();
	
	req.open('GET', base + "/me/tasks", true);
	req.setRequestHeader ("Authorization", "Bearer "+token);
	
	req.onload = function(e)
	{
		if (req.readyState == 4)
		{
			if(req.status == 200) 
			{
				//Pebble.showSimpleNotificationOnPebble("Tasks Success", "Yes!");
							
				response = JSON.parse(req.responseText);
				
				body = "";
				var today = new Date();
				today.setHours(23);
				today.setMinutes(59);
				
				for (var i = 0 ; i < response.length ; i++)
				{
					if(list == "today" || !list)
					{
						if(response[i].due_date && response[i].title)
						{
							var due_date = new Date(response[i].due_date);
							
							if(due_date.getTime() <= today.getTime() && response[i].completed_at == null)
							{
								todayList.push(response[i]);
								body += "\u00BB"+response[i].title;
							}
						}
					}
					else
					{
						;
					}
				}
				title = "Today ("+todayList.length+")";
				
				Pebble.showSimpleNotificationOnPebble(title, body);
				
				Pebble.sendAppMessage({
					"title":title,
					"body":body,
					"due":todayList.length
				});
			}
			else
			{
				Pebble.showSimpleNotificationOnPebble("Tasks Error", req.status + ": " + req.statusText);
			}
		}
	}
	
	req.send();
}

// Initialize the App
Pebble.addEventListener
(
	"ready",
	function(e)
	{
		username = localStorage.username;
		password = localStorage.password;
		token = localStorage.token;
		getTasks();
	}
);

// Every time the App sends a message
Pebble.addEventListener
(
	"appmessage",
	function(e)
	{
		getTasks();
	}
);

// Show the configuration page
Pebble.addEventListener
(
	"showConfiguration",
	function(e)
	{
		Pebble.openURL("http://intrepidwebdesigns.com/WunderPebble/config/");
	}
);

// Configuration page closed
Pebble.addEventListener
(
	"webviewclosed",
	function(e)
	{		
		var data = JSON.parse(e.response);		
		
		username = data.email;
		password = data.password;
		token = data.token;
				
		localStorage.username = data.email;
		localStorage.password = data.password;
		localStorage.token = data.token;
		
		getTasks();
	}
);