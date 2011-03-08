/*
  phones.js
  David Rowe 
  Dec 12 2010

  Phones screen for Mini Asterisk GUI.
*/

var update_time = 10;


// Called when we load page

function initialise() {
    mainMenu();

    // kick off cascading CGIs, initialisePage() will be called when complete

    loadExtensions();
}


function initialisePage() {
    // construct list of phone extensions
    
    var icon = '<img src="tick.png" alt="Analog Phone OK" />';
    var html = '';

    html += '<table align="right" width=600>';

    // print out internet connection

    html += '<tr><td colspan="3" align="left" valign="top"><h2>Phone System</h2></td></tr>';
    html += '<tr><td colspan="2">Internet Connection:</td><td><div id="internet"></div></td></tr>';
    html += '<tr><td colspan="2">Phone System IP Address:</td><td div id="ipaddress"></div></td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    // print out analog phones

    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phones</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXS') {
	    html += "<tr><td>" + analog_ext[i] + "</td>" + "<td>" + "Analog Phone" + "</td>";
	    html += "<td>Port" + i + "</td></tr>";
	}
    }

    // print out IP phones

    for (j in ip_ext) {
        if (ip_ext[j] != '') {
	    html += "<tr><td>" + j + "</td>" + "<td>" + "IP Phone" + "</td>";
	    html += "<td>" + ip_ext[j] + "</td></tr>";
	}
    }

    // print out phone lines

    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phone Lines</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXO') {
	    html += "<tr><td>" + "0" + "</td>" + "<td>" + "Analog Phone" + "</td>";
	    html += "<td>Port" + i + "</td></tr>";
	}
    }

    html += '</table>';

    document.getElementById("phones_table").innerHTML += html;

    // fire off fping CGI

    downloadUrl("/cgi-bin/fpingnodes.cgi?ip=google.com", processFping);

    // fire off Ifconfig IP address to get IP Address

    downloadUrl("/cgi-bin/ifconfig.cgi?arg=eth0", processIfconfig);
}


// called when Fping CGI returns

function processFping(doc, status) {

    // Change icon based on ping results

    var icon;

    if (doc.indexOf("google.com") != -1)
	icon = '<img src="tick.png" />';
    else
	icon = '<img src="cross.png" />';

    document.getElementById('internet').innerHTML = icon;
}

// called when ifconfig CGI returns

function processIfconfig(doc, status) {

    var ip="";

    if (doc.indexOf("inet addr:") != -1) {
	ip = doc.substr(doc.indexOf("inet addr:"), doc.length);
	ip = ip.substr(10, ip.length);
	ip = ip.substr(0, ip.indexOf(" "));
    }

    document.getElementById('ipaddress').innerHTML = ip;
}
