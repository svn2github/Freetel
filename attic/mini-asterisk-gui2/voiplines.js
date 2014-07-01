/*
  voiplines.js
  Ian Roberts
  May 10 2012

  voiplines screen for Mini Asterisk GUI2.
*/

var update_time = 10;

var user = "";
var host = "";
var passwd = "";
var fromuser= "";
var fromdomain= "";

var selection = "none";
var lastselection="";

// Registration line from sip.conf
var siprego_line="";
var siprego="";

// All from sip.conf
var sipnat_user="";
var sipnat_secret="";
var sipnat_host="";
var sipnat_fromuser="";
var sipnat_fromdomain="";

var sipnormal_user="";
var sipnormal_secret="";
var sipnormal_host="";
var sipnormal_fromuser="";
var sipnormal_fromdomain="";

var sipjazmin_user="";
var sipjazmin_secret="";
var sipjazmin_host="";
var sipjazmin_fromuser="";
var sipjazmin_fromdomain="";

// Statics
var sipnone_info = "No provider selected";
var sipnat_info = "You reach your ITSP via a NAT router ...the most common setup";
var sipnormal_info = "No NAT router between your Phone system and your ITSP";
var sipjazmin_info = "Enter your provider details";

// Called when we load page

function initialise() {
    mainMenu();

    initialisePage();
}


function initialisePage() {
    

	// Sip reload 	
	
	downloadUrl("/cgi-bin/asterisk.cgi?cli=sip reload",InitSipReloadReturn);
	
}

function InitSipReloadReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	// Check sip registry
	// Check if provider is up
	//     asterisk sip show registry
	//     cascades thru to load sip.conf info

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
		html += tickicon;
	} else {
		// no provider or 
		// provider is not registered
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
			
				// save this entire line because we are going to replace it later on
				siprego_line = line;
				var s = line.split(' ');
				siprego = s[2];    //  save registration string
		
				if (line.indexOf(";") != 0) {
				     registerflag = 1;
				}
				 else {
				     registerflag = 0;
				}
			}
			
			var myregexp = /\s|=/;	     // whitespace or equals .... for splitting out user,secret,host
			
			if (line.indexOf("sipnat-mini-asterisk") != -1) {
				if (line.indexOf("username") != -1) {

					var s = line.split(myregexp);
					sipnat_user = s[1]; //  save user
				} 
				else if (line.indexOf("secret") != -1) {
					var s = line.split(myregexp);
					sipnat_secret = s[1]; //  save secret
				} 
				else if (line.indexOf("host") != -1) {
					var s = line.split(myregexp);
					sipnat_host = s[1]; //  save host
				}
				else if (line.indexOf("fromuser") != -1) {
					var s = line.split(myregexp);
					sipnat_fromuser = s[1]; //  save 
				}
				else if (line.indexOf("fromdomain") != -1) {
					var s = line.split(myregexp);
					sipnat_fromdomain = s[1]; //  save 
				}
			}
			if (line.indexOf("sipnormal-mini-asterisk") != -1) {
				if (line.indexOf("username") != -1) {
					var s = line.split(myregexp);
					sipnormal_user = s[1]; //  save user
				}
				else if (line.indexOf("secret") != -1) {
					var s = line.split(myregexp);
					sipnormal_secret = s[1]; //  save secret
				}
				else if (line.indexOf("host") != -1) {
					var s = line.split(myregexp);
					sipnormal_host = s[1]; //  save host
				}
				else if (line.indexOf("fromuser") != -1) {
					var s = line.split(myregexp);
					sipnormal_fromuser = s[1]; //  save 
				}
				else if (line.indexOf("fromdomain") != -1) {
					var s = line.split(myregexp);
					sipnormal_fromdomain = s[1]; //  save
				}
			}
			if (line.indexOf("jazmin-mini-asterisk") != -1) {
				if (line.indexOf("username") != -1) {
					var s = line.split(myregexp);
					sipjazmin_user = s[1]; //  save user
				} 
				else if (line.indexOf("secret") != -1) {
					var s = line.split(myregexp);
					sipjazmin_secret = s[1]; //  save secret
				}
				else if (line.indexOf("host") != -1) {
					var s = line.split(myregexp);
					sipjazmin_host = s[1]; //  save host
				}
				else if (line.indexOf("fromuser") != -1) {
					var s = line.split(myregexp);
					sipjazmin_fromuser = s[1]; //  save 
				}
				else if (line.indexOf("fromdomain") != -1) {
					var s = line.split(myregexp);
					sipjazmin_fromdomain = s[1]; //  save
				}
			}
		}
		

		
	}
	);
	
	// sip info now loaded

	if(siprego_line[0] == ';') { 
		// commented out mean no rego selected
		lastselection = "none";
		document.getElementById('user').value = "";
		document.getElementById('host').value = "";
		document.getElementById('pass').value = "";
		document.getElementById('fromuser').value = "";
		document.getElementById('fromdomain').value = "";
		document.getElementById('info').textContent = sipnone_info;
		document.getElementById('provider').value="none";
	} 
	else if (siprego.indexOf(sipnat_user) != -1) {
		// rego line has sipnat 
		lastselection = "sipnat";
		document.getElementById('user').value = sipnat_user;
		document.getElementById('host').value = sipnat_host;
		document.getElementById('pass').value = sipnat_secret;
		document.getElementById('fromuser').value = sipnat_fromuser;
		document.getElementById('fromdomain').value = sipnat_fromdomain;
		document.getElementById('info').textContent = sipnat_info;
		document.getElementById('provider').value="sipnat";
	}
	else if (siprego.indexOf(sipnormal_user) != -1) {
		// rego line has sipnormal
		lastselection = "sipnormal";
		document.getElementById('user').value = sipnormal_user;
		document.getElementById('host').value = sipnormal_host;
		document.getElementById('pass').value = sipnormal_secret;
		document.getElementById('fromuser').value = sipnormal_fromuser;
		document.getElementById('fromdomain').value = sipnormal_fromdomain;
		document.getElementById('info').textContent = sipnormal_info;
		document.getElementById('provider').value="sipnormal";
	}
	else if (siprego.indexOf(sipjazmin_user) != -1) {
		// rego line has jazmin
		lastselection = "jazmin";
		document.getElementById('user').value = sipjazmin_user;
		document.getElementById('host').value = sipjazmin_host;
		document.getElementById('pass').value = sipjazmin_secret;
		document.getElementById('fromuser').value = sipjazmin_fromuser;
		document.getElementById('fromdomain').value = sipjazmin_fromdomain;
		document.getElementById('info').textContent = sipjazmin_info;
		document.getElementById('provider').value="jazmin";
	}			
}


function onClickApply() {

	// All this stuff is synchronous. Must happen in order specified.
	// You will need to cascade calls to downloadUrl

	// save globals
	//lastselection = selection;               This causes a bug under firefox, but not chrome ... why?
	
    document.getElementById("voiplinestatus").innerHTML = '<td>Checking... </td>';
	
	
	// take provider, username, password and host. 
	selection = document.getElementById('provider').value;
	user = document.getElementById('user').value;
	host = document.getElementById('host').value;
	passwd = document.getElementById('pass').value;
	fromuser = document.getElementById('fromuser').value;
	fromdomain = document.getElementById('fromdomain').value;
	
	// 1. Save sip.conf	
	

	
	// 2.0 uncomment sip register
	if(selection == "none") {
	
		if(lastselection == "none") {
			// do nothing but restart
			downloadUrl("/cgi-bin/asterisk.cgi?cli=sip reload",sipReloadReturn);
		}
		else {
			// comment out old rego line
			// reload and restart
			var url = '/cgi-bin/commentkey.cgi?file=/etc/asterisk/sip.conf&key=register-mini-asterisk';
			downloadUrl(url,commentregReturn);			
		}
	
	} 
	else {
		if(lastselection==selection) {
			// fillout
			// restart
				var new_register = user+":"+passwd+"@"+host;
				var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=' + siprego + '&that=' 
							+ new_register + '&key=register-mini-asterisk';
				downloadUrl(url,uncommenttrunkReturn);	

		
		}
		else if (lastselection=="none") {
				// straight into uncomment
				var url = '/cgi-bin/uncommentkey.cgi?file=/etc/asterisk/sip.conf&key=register-mini-asterisk';
				downloadUrl(url,uncommentregReturn);		
		
		
		}
		else {
				// comment old stuff out then straight on
				var url = '/cgi-bin/commentkey.cgi?file=/etc/asterisk/sip.conf&key=register-mini-asterisk';
				downloadUrl(url,commentregReturn);			
		}

		
	}
}	
	
function commentregReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
		// comment out old selection	

		var url = '/cgi-bin/commentkey.cgi?file=/etc/asterisk/sip.conf&key='+lastselection+'-mini-asterisk';
		downloadUrl(url,commentTrunkReturn);	
}

function commentTrunkReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
		// reset voip label	

		var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=voip&that=user'+lastselection;
		downloadUrl(url,resetVoipLabelReturn);	
}

function resetVoipLabelReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
		
		if (lastselection == selection) {
			// asterisk sip reload ..
			// ....check this may have to load new parms
			downloadUrl("/cgi-bin/asterisk.cgi?cli=sip reload",sipReloadReturn);
		}
		else if (selection == "none") {
			// asterisk sip reload  to remove old registration
			downloadUrl("/cgi-bin/asterisk.cgi?cli=sip reload",sipReloadReturn);
		}
		else {
			// uncomment new selection
			// fillout new parms
			// restart
			var url = '/cgi-bin/uncommentkey.cgi?file=/etc/asterisk/sip.conf&key=register-mini-asterisk';
			downloadUrl(url,uncommentregReturn);		
		}


}

function uncommentregReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	//  2.1	insert register command  
	//          ;register => 1234:password@mysipprovider.com   becomes     register => trev:password@192.168.1.30 
	
	var new_register = user+":"+passwd+"@"+host;
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=' + siprego + '&that=' + new_register + '&key=register-mini-asterisk';

	
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
				+'&that='+user
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
				+'&that='+passwd
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
				+'&that='+host
				+'&key='+selection+'-mini-asterisk';

	downloadUrl(url,fromhostReturn);
}

function fromhostReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	var url = '/cgi-bin/setlinekey2.cgi?file=/etc/asterisk/sip.conf'
				+'&this=fromuser='
				+'&that='+fromuser
				+'&key='+selection+'-mini-asterisk';

	downloadUrl(url,fromuserReturn);
}

function fromuserReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	var url = '/cgi-bin/setlinekey2.cgi?file=/etc/asterisk/sip.conf'
				+'&this=fromdomain='
				+'&that='+fromdomain
				+'&key='+selection+'-mini-asterisk';

	downloadUrl(url,fromdomainReturn);
}
	
function fromdomainReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//   3.8      if new provider
	//                replace old [voip] entry back to original [usersipnormal] or [usersipnat] or [userjazmin]
	
	//   3.9      if new provider
	//				[usersipnat]         becomes     [voip]
	//				or [usersipnormal]
	//				or [userjazmin]
	

	
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=user'+selection+'&that=voip';

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

	var sell = document.getElementById('provider').value;
	

	if (sell == "none" ) {
		// clear fields
		document.getElementById('user').value = "";
		document.getElementById('host').value = "";
		document.getElementById('pass').value = "";
		document.getElementById('fromuser').value = "";
		document.getElementById('fromdomain').value = "";
		document.getElementById('info').textContent = sipnone_info;
		
	} else if (sell == "sipnat" ) {
		// fill sipnat
		document.getElementById('user').value = sipnat_user;
		document.getElementById('host').value = sipnat_host;
		document.getElementById('pass').value = sipnat_secret;
		document.getElementById('fromuser').value = sipnat_fromuser;
		document.getElementById('fromdomain').value = sipnat_fromdomain;
		document.getElementById('info').textContent = sipnat_info;
	} else if (sell == "sipnormal" ) {
	    // fill sipnormal
		document.getElementById('user').value = sipnormal_user;
		document.getElementById('host').value = sipnormal_host;
		document.getElementById('pass').value = sipnormal_secret;		
		document.getElementById('fromuser').value = sipnormal_fromuser;
		document.getElementById('fromdomain').value = sipnormal_fromdomain;
		document.getElementById('info').textContent = sipnormal_info;
	} else if (sell == "jazmin" ) {
		// fill jazmin
		document.getElementById('user').value = sipjazmin_user;
		document.getElementById('host').value = sipjazmin_host;
		document.getElementById('pass').value = sipjazmin_secret;		
		document.getElementById('fromuser').value = sipjazmin_fromuser;
		document.getElementById('fromdomain').value = sipjazmin_fromdomain;
		document.getElementById('info').textContent = sipjazmin_info;
	} else {
		// something weird happened
	}
	
}


