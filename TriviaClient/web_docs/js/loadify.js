// A minimal library to load the alertifyjs plugin.
// (C) 2017 Ron Popov & Yotam salmon, All rights reserved
function LoadFiles()
{
	function AddCssFile(url)
	{
		var link = document.createElement('link');
		link.setAttribute('rel', 'stylesheet');
		link.setAttribute('type', 'text/css');
		link.setAttribute('href', url);
		document.getElementsByTagName('head')[0].appendChild(link);
	}

	function LoadJSFile(scripts, complete) {

		var loadScript = function( src ) {
			var xmlhttp, next;
			if (window.XMLHttpRequest)  {
				xmlhttp = new XMLHttpRequest();
			} else {
				try {
					 xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
				} catch(e) {
					return;
				}
			}
			xmlhttp.onreadystatechange = function() {
				if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
					eval(xmlhttp.responseText);
					next = scripts.shift();
					if ( next ) {
						setTimeout(function() { loadScript(next); }, 1);
					} else if ( typeof complete == 'function' ) {
						complete();
					}
				}
			};
			xmlhttp.open("GET", src , true);
			xmlhttp.send();
		};

		loadScript( scripts.shift() );
	}


	AddCssFile('http://cdn.jsdelivr.net/alertifyjs/1.10.0/css/themes/bootstrap.min.css');
	AddCssFile('http://cdn.jsdelivr.net/alertifyjs/1.10.0/css/themes/semantic.min.css');
	AddCssFile('http://cdn.jsdelivr.net/alertifyjs/1.10.0/css/themes/default.min.css');
	AddCssFile('http://cdn.jsdelivr.net/alertifyjs/1.10.0/css/alertify.min.css');

	LoadJSFile(['http://cdn.jsdelivr.net/alertifyjs/1.10.0/alertify.min.js'] , function()
			{
				alertify.notify("Welcome to BrainGames Trivia!");
			});
}
