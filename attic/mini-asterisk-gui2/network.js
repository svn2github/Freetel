/*
  network.js
  David Rowe 
  March 7 2011

  Network screen for Mini Asterisk GUI.
*/

var dhcp;
var ipaddress;
var netmask;
var gateway;
var dns;
var backdoor;

// Called when we load page

function initialise() {
    mainMenu();
    var html = toolTips[0] + toolTips[1];
    document.getElementById('tooltips').innerHTML = html;

    // kick off cascading CGIs, initialisePage() will be called when complete

    // determine if DHCP or static

    downloadUrl("/cgi-bin/getdhcp.cgi", processDhcp);
//    initialisePage();
}

function doDHCP() {
    greyout("yes");
}

function doStatic() {
    greyout("no");
}


function greyout(dhcp) {

    if (dhcp == "yes") {
	document.getElementById("static").checked = 0;
	document.getElementById("dhcp").checked = 1;
	document.network.ipaddress.disabled = 1;
	document.network.netmask.disabled = 1;
	document.network.gateway.disabled = 1;
	document.network.dns.disabled = 1;
    }
    else {
	document.getElementById("static").checked = 1;
	document.getElementById("dhcp").checked = 0;
	document.network.ipaddress.disabled = 0;
	document.network.netmask.disabled = 0;
	document.network.gateway.disabled = 0;
	document.network.dns.disabled = 0;
    }
}

// called when DHCP CGI returns

function processDhcp(doc,status) {
    dhcp = "no";

    loadHtmlTextFile(doc, function(line) {
	    parseNetwork(line);
	}
	);

    greyout(dhcp);

    if (dhcp == "no") {
        downloadUrl("/cgi-bin/getconf.cgi?file=../init.d/network-static", processStatic);
    } else {
	    //initialise(); endless loop
	}
}


// called when static IP CGI returns

function processStatic(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    parseNetwork(line);
	}
	);
        
    downloadUrl("/cgi-bin/getemergencyip.cgi", processEmergencyIp);
}

// called when Emergency CGI returns

function processEmergencyIp(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    parseNetwork(line);
	}
	);
        
    // fire off fping CGI

    downloadUrl("/cgi-bin/fpingnodes.cgi?ip=google.com", processFping);
}

function parseNetwork(line) {

    if (line.indexOf("DHCP=") != -1) {
      var s = line.split('"');
      dhcp = s[1];
    }

    if (line.indexOf("IPADDRESS=") != -1) {
      var s = line.split('"');
      ipaddress = s[1];
      document.network.ipaddress.value = ipaddress;
    }

    if (line.indexOf("NETMASK=") != -1) {
      var s = line.split('"');
      netmask = s[1];
      document.network.netmask.value = netmask;
    }

    if (line.indexOf("GATEWAY=") != -1) {
      var s = line.split('"');
      gateway = s[1];
      document.network.gateway.value = gateway;
    }

    if (line.indexOf("DNS=") != -1) {
      var s = line.split('"');
      dns = s[1];
      document.network.dns.value = dns;
   }

   if (line.indexOf("BACKDOOR=") != -1) {
      var s = line.split('"');
      backdoor = s[1];
      document.network.backdoor.value = backdoor;
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


// called when Apply button pressed

function onClickApply() {

    // change network type, DHCP or static

    if (document.getElementById("dhcp").checked == 1) {

		if (dhcp = "no") {
			// disable network-static service and endable network service
			downloadUrl("/cgi-bin/setservice.cgi?action=enabledhcp",GenericReturn);
		}
		else {
			// dhcp already running, restart service
			downloadUrl("/cgi-bin/setservice.cgi?action=restartdhcp",GenericReturn);
		}
    }
    else {

		// start cgi cascade
		// disable network service and enable network-static service
		downloadUrl("/cgi-bin/setservice.cgi?action=enablestatic",dhcpReturn);




	}
}

function dhcpReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	var new_ipaddress = document.network.ipaddress.value;
	var url = '/cgi-bin/setline.cgi?file=/etc/init.d/network-static&this=IPADDRESS=&that="' + new_ipaddress + '"';
		
	downloadUrl(url,ipaddressReturn);
}

function ipaddressReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	var new_netmask = document.network.netmask.value;
	var url = '/cgi-bin/setline.cgi?file=/etc/init.d/network-static&this=NETMASK=&that="' + new_netmask + '"';
	downloadUrl(url,netmaskReturn);
}

function netmaskReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	var new_gateway = document.network.gateway.value;
	var url = '/cgi-bin/setline.cgi?file=/etc/init.d/network-static&this=GATEWAY=&that="' + new_gateway + '"';
	downloadUrl(url,gatewayReturn);
}

function gatewayReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	var new_dns = document.network.dns.value;
	var url = '/cgi-bin/setline.cgi?file=/etc/init.d/network-static&this=DNS=&that="' + new_dns + '"';
	downloadUrl(url,dnsReturn);
}


//	var new_backdoor = document.network.backdoor.value;
//	var url = '/cgi-bin/setline.cgi?file=/etc/init.d/network-backup&this=BACKDOOR=&that="' + new_backdoor + '"';
//	downloadUrl(url,GenericReturn);



//	downloadUrl("/cgi-bin/setservice.cgi?action=restartbackup",GenericReturn);
//	


function dnsReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	// finally restart network-static service with new parameters

	downloadUrl("/cgi-bin/setservice.cgi?action=restartstatic",restartstaticReturn);
}

function restartstaticReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	// finally reload the page (this updates the fping....crudely)

	initialise();
}

