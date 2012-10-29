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
    var itemcount = 0;
	
    html += '<form name="phones" method="get">';

    html += '<table align="right" width=600 cellspacing=2>';

    // print out internet connection

    html += '<tr><td colspan="3" align="left" valign="top"><h2>Phone System (GUI2)</h2></td></tr>';
    html += '<tr><td colspan="2">Internet Connection:</td><td> </td><td><div id="internet"></div></td></tr>';
    html += '<tr><td colspan="2">Phone System IP Address:</td><td> </td><td div id="ipaddress"></div></td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 

    // print out analog phones

    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phones</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXS') {
            html += "<tr>";
	        html += "<td>" + analog_ext[i] + "</td>" + "<td>" + "Analog Phone" + "</td>";
	        html += "<td>Port" + i  + "</td>";

            html += "<td>" + '<img src="tick.png" alt="tick" />' + "</td>";

            // the reception checkbox code
	        html += "<td>" + '<input name="selected_in[]" ' ;
			html += 'value= "' + ext_code[i-1] + '" ';
            html += 'type="checkbox" ' ;

            if (recept_num.indexOf(analog_ext[i]) != -1) {
		        html += '" checked/>';
		        }
	        else {
		        html += '" unchecked/>';
	        }

	        // save the reception code
	        recept_code[itemcount++]=ext_code[i-1];

            html += "</td>";
	
            html += "</tr>";
	    }
    }



    // print out IP phones
    //  j is 6011,6012 etc

    for (j in ip_ext) {
        if (ip_ext[j] != '') {
            html += "<tr>";
	        html += "<td>" + j + "</td>" + "<td>" + "IP Phone" + "</td>";
	        html += "<td>" + ip_ext[j] + "</td>";

            html += "<td>" + '<img src="tick.png" alt="tick" />' + "</td>";

            // the reception checkbox code
	        html += "<td>" + '<input name="selected_in[]" ';
 			html += 'value="SIP_' + j + '" ';
            html += 'type="checkbox" ' ;
			
            if (recept_num.indexOf(j)!=-1) {
		        html += '" checked/>';
		    }
	        else {
		        html += '" unchecked/>';
	        }

	        // save the reception code
           
	        recept_code[itemcount++]=ext_code[(i-1)];
	        i++;

            html += "</td>";

            html += "</tr>";
	    }
    }

    html += "<tr>";
    html += "<td>";
    html += "<a href=ipphones.html>Add IP Phone</a>";
    html += "</td>";

    // print out submit button
	//doesn't work ... Only use static html buttons

/*    html += "<td>";
    html += "<input type='submit' value='Update Reception' onClick='onUpdateReception()'/>";
    html += "</td>";
*/
    html += "</tr>";





    // print out phone lines

    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phone Lines</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXO') {
            html += "<tr>";
	    html += "<td>" + "0" + "</td>" + "<td>" + "Analog Phone Line" + "</td>";
	    html += "<td>Port" + i + "</td>";
            html += "<td>" + '<img src="tick.png" alt="tick" />' + "</td>";

            // the outgoing radiobox code
/*	    html += "<td>" + '<input name="selected_out[]" type="radio" ' ;

            if (outgoing_num == i) {
		    html += '" checked/>';
		}
	    else {
		    html += '" unchecked/>';
	    }

	    // save the outgoing code
           
	    outgoing_code[itemcount++]=ext_code[(i-1)];

            html += "</td>";

*/            html += "</tr>";
	}


    }

    // add a voip line

    html += "<tr>";
    html += "<td>";
    html += "<a href=voiplines.html>Add Voip line</a>";
    html += "</td>";
    html += "</tr>";

    html += '</table>';
	
    html += '</form>';
    document.getElementById("phones_table").innerHTML += html;

    // fire off fping CGI

    downloadUrl("/cgi-bin/fpingnodes.cgi?ip=google.com", processFping);

    // fire off Ifconfig IP address to get IP Address

    downloadUrl("/cgi-bin/ifconfig.cgi?arg=eth0", processIfconfig);
//    downloadUrl("/cgi-bin/ifconfig.cgi?arg=eth1", processIfconfig);

	// downloadUrl works fine here.
//    downloadUrl("/cgi-bin/setring.cgi?Zap_3=on", null);

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

function onClickUpdateReception() {

	// get the arguments from the form  selected_in[]
	///document.myform.elements['fav[]'].checked
	var arg ="";


	//	number of check boxes in the form
	var total = document.phones.elements['selected_in[]'].length;

	var recept_box = document.phones.elements['selected_in[]'];

	j=0;
	for (i=0;i<total; i++) {

	        // construct new arg from selected
		if(recept_box[i].checked) {
			
			j++;
			if (j>1)
				arg = arg+"&";

			// got to make up new codes
			arg = arg + recept_box[i].value + "=on";
		}

	}

    var arg2 =  "/cgi-bin/setring.cgi?" + arg;
	downloadUrl(arg2,OnSetringReturn);


}

function OnSetringReturn(doc,status) {
    loadHtmlTextFile(doc, function(line) {
	    //parseSipShowPeers(line);
	}
	);

	downloadUrl("/cgi-bin/asterisk.cgi?cli=dialplan reload",GenericReturn);
	//initialisePage();
}

function onClickUpdateOutgoing() {

	// get the arguments from the form  selected_out[]
	///document.myform.elements['fav[]'].checked
	var arg ="";


	//	number of check boxes in the form
	var total = document.phones.elements['selected_out[]'].length;

	var out_box = document.phones.elements['selected_out[]'];

	//j=0;
	for (i=0;i<total; i++) {

	        // construct new arg from selected
		if(out_box[i].checked) {
			
			//j++;
			//if (j>1)
			//	arg = arg+"&";

			// got to make up new codes
			arg = arg + recept_code[i] + "=on";
		}

	}
        var arg2 =  "/cgi-bin/setoutgoing.cgi?" + arg;
//	downloadUrl(arg2,GenericReturn);


}




