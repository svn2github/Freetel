/*
  voiplines.js
  Ian Roberts
  May 10 2012

  voiplines screen for Mini Asterisk GUI2.
*/

var update_time = 10;
var selection = "";
var lastselection="";
var lasthost="";
var lastuser="";
var lastpasswd="";
var lastreg="";

var providerfound = 0;

// Registration line from sip.conf
var siprego_line="";

// All from sip.conf
var sipnat_user="";
var sipnat_secret="";
var sipnat_host="";
var sipnormal_user="";
var sipnormal_secret="";
var sipnormal_host="";
var sipjazmin_user="";
var sipjazmin_secret="";
var sipjazmin_host="";

// Called when we load page

function initialise() {
    mainMenu();

    initialisePage();
}


function initialisePage() {
    // 
    
	
	// Check if provider is up
	//     asterisk sip show registry
	downloadUrl("/cgi-bin/asterisk.cgi?cli=sip show registry",OnSipshowReturn);

}

function OnSipshowReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

    var tickicon = '<img src="tick.png" alt="Provider OK" />';
    var crossicon = '<img src="cross.png" alt="No provider found" />';
    var html = '';

	// If a provider is registered, 
	
    if (doc.indexOf("Registered") != -1) {
		// provider is registered
		providerfound = 1;
		// Find the provider ip add

		// Get details from sip.conf
		//document.getElementById('provider').value = sip or sipnat or jazmin
		html += tickicon;
		// get provider details from doc
		var user = "";
		var host = "";
		//var pass = "";
		// fill out form on page
		//document.getElementById('user').value = user;
		//document.getElementById('host').value = host;
		//document.getElementById('pass').value = pass;
		
	} else {
		// no provider or 
		// provider is not registered
		providerfound = 0;
		html += crossicon;
	}

    document.getElementById("voiplinestatus").innerHTML = html;
	
	// Load sip.conf
	
	 downloadUrl("/cgi-bin/getconf.cgi?file=sip.conf", loadSipConf);

}

function loadSipConf(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    if (line.indexOf("mini-asterisk") != -1)  {
			if (line.indexOf("register-mini-asterisk") != -1) {
			
				// maybe save this entire line because we are going to replace it later on
				siprego_line = line;
				
				if (line.indexOf(";") != 0) {
				     registerflag = 1;
					 //save rego user:pswd:host
				}
				 else {
				     registerflag = 0;
				}
			}
			if (line.indexOf("sipnat-mini-asterisk") != -1) {
				if (line.indexOf("username") != -1) {
					var s = line.split('=');
					var t = s[1].split[' ');
					sipnat_user = t[0]; //  save user
				} 
				else if (line.indexOf("secret") != -1) {
					sipnat_secret = ""; //  save secret
				} 
				else if (line.indexOf("host") != -1) {
					sipnat_host = ""; //  save host
				}
			}
			if (line.indexOf("sipnormal-mini-asterisk") != -1) {
				if (line.indexOf("username") != -1) {
					sipnormal_user = ""; //  save user
				}
				if (line.indexOf("secret") != -1) {
					sipnormal_secret = ""; //  save secret
				}
				if (line.indexOf("host") != -1) {
					sipnormal_host = ""; //  save host
				}
			}
			if (line.indexOf("jazmin-mini-asterisk") != -1) {
				if (line.indexOf("username") != -1) {
					sipjazmin_user = ""; //  save user
				} else
				if (line.indexOf("secret") != -1) {
					sipjazmin_secret = ""; //  save secret
				} else
				if (line.indexOf("host") != -1) {
					sipjazmin_host = ""; //  save host
				}
			}
		}
	}
	);
	
}


function onClickApply() {

	// All this stuff is synchronous. Must happen in order specified.
	// You will need to cascade calls to downloadUrl

	// take provider, username, password and host. 
	var user = "";
	var host = "";
	var passwd = "";

	selection = document.getElementById('provider').value;
	user = document.getElementById('user').value;
	host = document.getElementById('host').value;
	passwd = document.getElementById('pass').value;
	
	// 1. Save sip.conf	
	
	
	// 2.0 uncomment sip register
	var url = '/cgi-bin/uncommentkey.cgi?file=/etc/asterisk/sip.conf&key=register-mini-asterisk';
		
	downloadUrl(url,uncommentregReturn);	
	
}	
	


function uncommentregReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	//  2.1	insert register command  
	//          ;register => 1234:password@mysipprovider.com   becomes     register => trev:password@192.168.1.30 
	
	var new_register = document.getElementById('user').value+":"+document.getElementById('pass').value+"@"+document.getElementById('host').value;
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=9876:password@mysipprovider.com&that=' + new_register + '&key=register-mini-asterisk';

	// save globals
	lastuser = document.getElementById('user').value;
	lasthost = document.getElementById('host').value;
	lastpasswd = document.getElementById('pass').value;
	lastreg = new_register;
	
	downloadUrl(url,updateregReturn);
	

}

function updateregReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	//  2.15 uncomment sip trunk

	var url = '/cgi-bin/uncommentkey.cgi?file=/etc/asterisk/sip.conf&key='+selection+'-mini-asterisk';
		
	downloadUrl(url,uncommenttrunkReturn);	

}




function uncommenttrunkReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	//  2.2	update sip trunk details  ... user pw host

	//          username=usersipnormal                     username=trev
	//          secret=passwordsip                   secret=password
	//          host=hostsip                         host=192.168.1.30
	
	var url = '/cgi-bin/setlinekey2.cgi?file=/etc/asterisk/sip.conf'
				+'&this=username='
				+'&that='+document.getElementById('user').value
				+'&key='+selection+'-mini-asterisk';

	downloadUrl(url,usernameReturn);
	
}

function usernameReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	var url = '/cgi-bin/setlinekey2.cgi?file=/etc/asterisk/sip.conf'
				+'&this=secret='
				+'&that='+document.getElementById('pass').value
				+'&key='+selection+'-mini-asterisk';

	downloadUrl(url,passwordReturn);
	
}

function passwordReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	var url = '/cgi-bin/setlinekey2.cgi?file=/etc/asterisk/sip.conf'
				+'&this=host='
				+'&that='+document.getElementById('host').value
				+'&key='+selection+'-mini-asterisk';

	downloadUrl(url,hostReturn);
	}

function hostReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//   3.9       [usersipnat]         becomes     [voip]
	//				or [usersip]
	//				or [userjazmin]

	
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=user'+selection+'&that=voip';
	lastselection = selection;
	downloadUrl(url,extReturn);
}

function extReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	// 4. asterisk sip reload ....ORDER? beardy has this at position 4

	downloadUrl("/cgi-bin/asterisk.cgi?cli=sip reload",sipReloadReturn);

}

function sipReloadReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	// 5. asterisk dialplan reload

	downloadUrl("/cgi-bin/asterisk.cgi?cli=dialplan reload",dialplanReloadReturn);

}

function dialplanReloadReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	// initialise page?

	initialise();

}

function changeProvider() {

	// fill the host and username fields based on selection

	selection = document.getElementById('provider').value;
	
	// Disable old provider if used
	//
	

	if (selection == "none" ) {
		// clear fields
		document.getElementById('user').value = "";
		document.getElementById('host').value = "";
		document.getElementById('pass').value = "";
		var temp = document.getElementById('info');
		document.getElementById('info').textContent = "No provider selected";
		
		// may need to comment out previous sip.conf activity
		
	} else if (selection == "sipnat" ) {
		// fill sipnat
		// we should really get this stuff from the sip.conf file....
		document.getElementById('user').value = "user";
		document.getElementById('host').value = "192.168.1.28";
		document.getElementById('pass').value = "xxxxxx";
		document.getElementById('info').textContent = "You reach your ITSP via a NAT router ...the most common setup";
	} else if (selection == "sipnormal" ) {
	    // fill sip
		document.getElementById('user').value = "trev";
		document.getElementById('host').value = "192.168.1.30";
		document.getElementById('pass').value = "supertrev";		
		document.getElementById('info').textContent = "No NAT router between your Phone system and your ITSP";
	} else if (selection == "jazmin" ) {
		// fill jazmin
		document.getElementById('user').value = "username";
		document.getElementById('host').value = "sip.jazmin.net.au";
		document.getElementById('pass').value = "xxxxxx";		
		document.getElementById('info').textContent = "Jazmin are a South Australian ITSP";
	} else {
		// something weird happened
	}
	
}


