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
    //initialisePage();
}


function initialisePage() {
    // construct list of phone extensions
    
    var icon = '<img src="tick.png" alt="Analog Phone OK" />';
    var html = '';


    html += '<table align="right" width=600 cellspacing=2>';

    // print out internet connection

    html += '<tr><td colspan="3" align="left" valign="top"><h2>Phone System (GUI2)</h2></td></tr>';
    html += '<tr><td colspan="2">Internet Connection:</td><td> </td><td><div id="internet"></div></td></tr>';
    html += '<tr><td colspan="2">Phone System IP Address:</td><td> </td><td div id="ipaddress"></div></td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    html += '<form name="phones" method="get">';

    // print out analog phones
    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phones</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXS') {
            html += "<tr>";
	    html += "<td>" + analog_ext[i] + "</td>" + "<td>" + "Analog Phone" + "</td>";
//	    html += "<td>Port" + i  + "<td>" +"   <img src=" + "tick.png" + " />"+"</td>" + "</td></tr>";
//	    html += "<td>Port" + i  + "<td>" + '<img src="tick.png" />' + "</td>" + "</td></tr>";
	    html += "<td>Port" + i  + "</td>";
	    //html += "<td>(Reception)</td>";

            // the reception checkbox code
	    html += "<td>" + '<input name="selected[]" type="checkbox" ' ;     
		// Underscore used instead of forward slash to pass thru html request

            if (reception.indexOf(analog_ext[i]) != -1) {
		    html += '" checked/>';
		}
	    else {
		    html += '" unchecked/>';
	    }
            html += "</td>";
	
	    html += "<td>(Reception)</td>";
            html += "<td>" + '<img src="tick.png" />' + "</td>";
            html += "</tr>";
	}
    }



    // print out IP phones

    for (j in ip_ext) {
        if (ip_ext[j] != '') {
            html += "<tr>";
	    html += "<td>" + j + "</td>" + "<td>" + "IP Phone" + "</td>";
	    html += "<td>" + ip_ext[j] + "</td>";

            // the reception checkbox code
	    html += "<td>" + '<input name="selected[]" type="checkbox" ' ;
		// Underscore used instead of forward slash to pass thru html request
 

            if (reception.indexOf(ip_ext[j])!=-1) {
		    html += '" checked/>';
		}
	    else {
		    html += '" unchecked/>';
	    }
            html += "</td>";
	
	    html += "<td>(Reception)</td>";

            html += "<td>" + '<img src="tick.png" />' + "</td>";
            html += "</tr>";
	}
    }

    // print out submit button


    html += "<tr>";
    html += "<td>";
    html += "<a href=ipphones.html    /a>Add IP Phone";
    html += "</td>";
    html += "<td>";
    html += "<input type='submit' value='Update Reception' onClick='onClickApply(event)'/>";
    html += "</td>";
    html += "</tr>";


    html += '</form>';


    // print out phone lines

    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phone Lines</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXO') {
            html += "<tr>";
	    html += "<td>" + "0" + "</td>" + "<td>" + "Analog Phone" + "</td>";
	    html += "<td>Port" + i + "</td>";
            html += "<td>" + '<img src="tick.png" />' + "</td>";
            html += "</tr>";
	}
    }

    // add a voip line

    html += "<tr>";
    html += "<td>";
    html += "<a href=voiplines.html    /a>Add Voip line";
    html += "</td>";
    html += "</tr>";

    html += '</table>';

    document.getElementById("phones_table").innerHTML += html;

    // fire off fping CGI

    downloadUrl("/cgi-bin/fpingnodes.cgi?ip=google.com", processFping);

    // fire off Ifconfig IP address to get IP Address

    downloadUrl("/cgi-bin/ifconfig.cgi?arg=eth0", processIfconfig);
//    downloadUrl("/cgi-bin/ifconfig.cgi?arg=eth1", processIfconfig);
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

function onClickApply(e) {

	// get the arguments from the form  selected[]
///document.myform.elements['fav[]'].checked
var arg ='';
var total = document.getElementByName('selected[]');
	for (i=0;i<total; i++) {
// in document.phones.elements['selected[]'] {
		if (selected[x]<6010)
		arg = arg + 'ZAP_'+i;
        // construct new arg from selected
	//downloadUrl("/cgi-bin/setring.cgi?"+arg1,null);
	}

}
