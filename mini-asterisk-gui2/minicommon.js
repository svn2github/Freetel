/*
  minicommon.js
  David Rowe 
  Dec 16 2010

  Common functions used by Mini Asterisk GUI.
*/

// index Zaptel port number (1,2,...), value extension (6001,6002...)

var analog_ext = [];

// zaptel port number (1,2,...), value FXS/FXO

var zap = [];

// all IP extensions, index ext, value true|false to indicate if live

var ip_ext = [];

function loadExtensions() {

    /* 
       Kick off cascading CGIs and callbacks to load analog and ip
       extension data.

       From extensions.conf and zaptel.conf, determine which zap ports
       are live and what type they are (FXS or FXO).

       From extensions.conf determine, asterisk -r sip show peers
       to determine which sip extenions are live

       That will do for now......
    */
    
    downloadUrl("/cgi-bin/getconf.cgi?file=../zaptel.conf", loadZapata);
}

// remove leading and trailing whitespace from a string

function trim(stringToTrim) {
	return stringToTrim.replace(/^\s+|\s+$/g,"");
}


/*
  Given extenions.conf lines like this:

  exten => 6011,1,Dial(SIP/6011)
  exten => 6001,1,Dial(Zap/1)		
  exten =>  EXT,1,Dial(TECH1/TECH2)

  returns EXT, TECH1, TECH2
*/

function parseDialPlan(line) {
    var split1 = line.split(",");
    var split2 = split1[0].split("=>");
    var ext = trim(split2[1]);
    var tech = split1[2];
    var index1 = tech.indexOf('(');
    var index2 = tech.indexOf('/');
    var index3 = tech.indexOf(')');

    var tech1 = tech.substr(index1+1, index2-index1-1); 
    var tech2 = tech.substr(index2+1, index3-index2-1);

    return [ext,tech1,tech2];
}


// called to process /etc/zapata.conf
// creates an associative array of active Zap ports

function loadZapata(doc,status) {
    loadHtmlTextFile(
        doc, 
        function(line) {

	    if (line.indexOf('fxoks') != -1) {
		var split1 = line.split("=");
		var ports = split1[1].split(",");
		for(var i=0; i<ports.length; i++) {
		    port = trim(ports[i]);
		    zap[port] = 'FXS';
		}
	    }

	    if (line.indexOf('fxsks') != -1) {
		var split1 = line.split("=");
		var ports = split1[1].split(",");
		for(var i=0; i<ports.length; i++) {
		    port = trim(ports[i]);
		    zap[port] = 'FXO';
		}
	    }

	}
    );
		     
    // now we can load extensions.conf

    downloadUrl("/cgi-bin/getconf.cgi?file=extensions.conf", loadExtensionsConf);
}

// called to process extensions.conf
// creates an associative array of analog and ip extensions

function loadExtensionsConf(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    if ((line.indexOf("exten =>") != -1) &&
		(line.indexOf("mini_ext") != -1)) {
		var ret = parseDialPlan(line);
		var ext = ret[0];
		var tech1 = ret[1];
		var tech2 = ret[2]; 
		if (tech1 == "Zap") {
		    if (zap[tech2] == undefined)
			analog_ext[tech2] = "";
		    else
			analog_ext[tech2] = ext; 		
		}
		if (tech1 == "SIP") {
		    ip_ext[ext] = "";
		}
	    }
	}
    );

    // we get ip extensions from "sip show peer" at asterisk CLI

    downloadUrl("/cgi-bin/asterisk.cgi?cli=sip show peers", loadIpExtensions);
}


function parseSipShowPeers(line) {
    var myregexp = /\s+/;
    var s = line.split(myregexp);
    var ext = s[0].substr(0,4);
    var ip = s[1];
    if (ip_ext[ext] != undefined) {
	if (line.indexOf("OK") != -1)
	    ip_ext[ext] = ip;
    }
}

// called to process asterisk CLI "sip show peers"
// creates an associative array of IP extensions

function loadIpExtensions(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    parseSipShowPeers(line);
	}
	);

    initialisePage();
}


// Load a html encoded text file, strips off the HTML tags
// and calls textLineProcessing() for every line

function loadHtmlTextFile(doc, textLineProcessing) {

    var lines = doc.split("\n");
    var state = "looking for body";
    var next_state;

    for (var j in lines) {
	next_state = state;
	if (state == "looking for body") {
	    if (lines[j].indexOf('<body>') != -1)
		next_state = "body";
	}
	if (state == "body") {
	    if (lines[j].indexOf('</body>') != -1)
		next_state = "finished";
	    else {
		// main processing of text here
		textLineProcessing(lines[j]);
	    }
	}
	state = next_state;
    }
}


/**
 * Returns an XMLHttp instance to use for asynchronous
 * downloading. This method will never throw an exception, but will
 * return NULL if the browser does not support XmlHttp for any reason.
 * @return {XMLHttpRequest|Null}
 */

function createXmlHttpRequest() {
    try {
	if (typeof ActiveXObject != 'undefined') {
	    return new ActiveXObject('Microsoft.XMLHTTP');
	} else if (window["XMLHttpRequest"]) {
	    return new XMLHttpRequest();
	}
    } catch (e) {
	changeStatus(e);
    }
    return null;
};


/**
 * This functions wraps XMLHttpRequest open/send function.
 * It lets you specify a URL and will call the callback if
 * it gets a status code of 200.
 * @param {String} url The URL to retrieve
 * @param {Function} callback The function to call once retrieved.
 */
function downloadUrl(url, callback) {
    var status = -1;
    var request = createXmlHttpRequest();
    if (!request) {
	return false;
    }

    request.onreadystatechange = function() {
	if (request.readyState == 4) {
	    try {
		status = request.status;
	    } catch (e) {
		// Usually indicates request timed out in FF.
	    }
	    if (status == 200) {
		callback(request.responseText, request.status);
		request.onreadystatechange = function() {};
	    }
	}
    }
    request.open('GET', url, true);
    try {
	request.send(null);
    } catch (e) {
	changeStatus(e);
    }
};
