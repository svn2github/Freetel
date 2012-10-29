/*
  voiplines.js
  Ian Roberts
  May 10 2012

  voiplines screen for Mini Asterisk GUI2.
*/

var update_time = 10;
var selection = "";

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
	var providerfound = 0;

	// If a provider is registered, 
	
    if (doc.indexOf("Registered") != -1) {
		// provider is registered
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
		html += crossicon;
	}

    document.getElementById("voiplinestatus").innerHTML = html;

}

function onClickApply() {

	// All this stuff is synchronous. Must happen in order specified.
	// You will need to cascade calls to downloadUrl

	// take provider, username, password and host. 
	var user = "";
	var host = "";
	var passwd = "";

	
	// 1. Save sip.conf	
	
	selection = document.getElementById('provider').value;
	user = document.getElementById('user').value;
	host = document.getElementById('host').value;
	passwd = document.getElementById('pass').value;
	
	
	// 2. Update sip.conf
	//  2.1	insert register command  
	//          ;register => 1234:password@mysipprovider.com   becomes     register => trev:password@192.168.1.30 
	
	var new_register = user+":"+passwd+"@"+host;
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=9876:password@mysipprovider.com&that=' + new_register ;
//	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=;register&that=register';
		
	downloadUrl(url,semiReturn);
	
	//  2.2	update sip trunk details  ... user pw host
	//          [usersip]             becomes       [trev]
	//          username=usersip                     username=trev
	//          secret=passwordsip                   secret=password
	//          host=hostsip                         host=192.168.1.30
	// 3. Modify extensions.conf for new provider
	//          exten => _1.,1,Dial(SIP/voip/${EXTEN:1})    becomes     exten => _1.,1,Dial(SIP/trev/${EXTEN:1}) 
	// 4. asterisk sip reload ....ORDER? beardy has this at position 4
	// 5. asterisk dialplan reload

}

function semiReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//          username=usersip                     username=trev
    //   need to switch keywords based on selection
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=;dregister=&that=register=';

	downloadUrl(url,registerReturn);
}




function registerReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//   username=usersip                     username=trev
    //   need to switch keywords based on selection
	var url = '/cgi-bin/setlinekey.cgi?file=/etc/asterisk/sip.conf&this=username&that=' 
			+ document.getElementById('user').value 
			+ '&key='+ selection + '-mini-asterisk';

	downloadUrl(url,usernameReturn);
}

function usernameReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//   secret=passwordsip                   secret=password
    //   need to switch keywords based on selection
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=passwordsip&that=' + document.getElementById('pass').value ;

	downloadUrl(url,passwordReturn);
}

function passwordReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//   host=hostsip                         host=192.168.1.30
    //   need to switch keywords based on selection
	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/sip.conf&this=hostsip&that=' + document.getElementById('host').value ;

	downloadUrl(url,hostReturn);
}

function hostReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);
	
	//          exten => _1.,1,Dial(SIP/voip/${EXTEN:1})    becomes     exten => _1.,1,Dial(SIP/trev/${EXTEN:1}) 

	var url = '/cgi-bin/setword.cgi?file=/etc/asterisk/extensions.conf&this=voip&that=' + document.getElementById('user').value ;

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

	if (selection == "none" ) {
		// clear fields
		document.getElementById('user').value = "";
		document.getElementById('host').value = "";
		document.getElementById('pass').value = "";
		var temp = document.getElementById('info');
		document.getElementById('info').textContent = "No provider selected";
	} else if (selection == "sipnat" ) {
		// fill sipnat
		document.getElementById('user').value = "user";
		document.getElementById('host').value = "192.168.1.28";
		document.getElementById('pass').value = "xxxxxx";
		document.getElementById('info').textContent = "You reach your ITSP via a NAT router ...the most common setup";
	} else if (selection == "sip" ) {
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


