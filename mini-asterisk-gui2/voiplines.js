/*
  voiplines.js
  Ian Roberts
  May 10 2012

  voiplines screen for Mini Asterisk GUI2.
*/

var update_time = 10;


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

	// 1. Save old sip.conf
	// 2. Make new sip.conf with provider uncommented
	//  2.1	insert register command
	//	2.2	uncomment sip trunk entry
	//  2.3	update sip trunk details
	// 3. Modify extensions.conf for new provider
	// 4. asterisk sip reload ....ORDER? beardy has this at position 4
	// 5. asterisk dialplan reload

}

function changeProvider() {

	// fill the host and username fields based on selection

	var selection = document.getElementById('provider').value;

	if (selection == "none" ) {
		// clear fields
		document.getElementById('user').value = "";
		document.getElementById('host').value = "";
		document.getElementById('pass').value = "";
		var temp = document.getElementById('info');
		document.getElementById('info').textContent = "No provider selected";
	} else if (selection == "SIPNAT" ) {
		// fill sipnat
		document.getElementById('user').value = "user";
		document.getElementById('host').value = "192.168.1.28";
		document.getElementById('pass').value = "xxxxxx";
		document.getElementById('info').textContent = "You reach your ITSP via a NAT router ...the most common setup";
	} else if (selection == "SIP" ) {
	    // fill sip
		document.getElementById('user').value = "user";
		document.getElementById('host').value = "192.168.1.28";
		document.getElementById('pass').value = "xxxxxx";		
		document.getElementById('info').textContent = "No NAT router between your Phone system and your ITSP";
	} else if (selection == "JAZMIN" ) {
		// fill jazmin
		document.getElementById('user').value = "username";
		document.getElementById('host').value = "sip.jazmin.net.au";
		document.getElementById('pass').value = "xxxxxx";		
		document.getElementById('info').textContent = "Jazmin are a South Australian ITSP";
	} else {
		// something weird happened
	}
	
}


