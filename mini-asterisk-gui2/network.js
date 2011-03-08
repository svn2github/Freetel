/*
  network.js
  David Rowe 
  March 7 2011

  Network screen for Mini Asterisk GUI.
*/

var dhcp;

// Called when we load page

function initialise() {
    mainMenu();

    // kick off cascading CGIs, initialisePage() will be called when complete

    // determine if DHCP or static

    downloadUrl("/cgi-bin/getdhcp.cgi", processDhcp);
}


// called when DHCP CGI returns

function processDhcp(doc,status) {
    dhcp = "no";

    loadHtmlTextFile(doc, function(line) {
	    parseNetwork(line);
	}
	);

    if (dhcp == "no") {
       downloadUrl("/cgi-bin/getdhcpconf.cgi?file=../init.d/network-static", processDhcp);
    }
    else {
        // grey out fields
        initilaisePage();
    }
}


// called when static IP CGI returns

function processStatic(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    parseNetwork(line);
	}
	);

    initialisePage();
}


function parseNetwork(line) {

    if (line.indexOf("DHCP=") != -1) {
      var s = line.split('"');
      dhcp = "yes";
    }

    if (line.indexOf("IPADDRESS=") != -1) {
      var s = line.split('"');
      var ip = s(2);
      document.getElementById('ip').innerHTML = ip;
    }

    if (line.indexOf("NETMASK=") != -1) {
      var s = line.split('"');
      var netmask = s(2);
      document.getElementById('netmask').innerHTML = netmask;
    }

    if (line.indexOf("GATEWAY=") != -1) {
      var s = line.split('"');
      var gateway = s(2);
      document.getElementById('gateway').innerHTML = gateway;
    }

    if (line.indexOf("DNS=") != -1) {
      var s = line.split('"');
      var dns = s(2);
      document.getElementById('dns').innerHTML = dns;
   }

}


// http://moblog.bradleyit.com/2009/06/javascript-ip-address-validation.html

function isIP(obj) {
    var ary = obj.value.split(".");
    var ip = true;

    ip = (ary.length == 4); 

    if (ip) {
	for (var i=0; i<4; i++) { 
		ip = (!ary[i].match(/^\d{1,3}$/) || (Number(ary[i]) > 255)) ? false : ip; 
    	} 
    }

    if (!ip) {    
	// the value is NOT a valid IP address
	obj.style.background = "red";
	obj.select();
    } 
    else { obj.style.background = ""; } // the value IS a valid IP address

    return ip;
}
