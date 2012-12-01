/*
  ipphones.js
  Ian Roberts
  May 10 2012

  IP Phones screen for Mini Asterisk GUI2.
*/

var update_time = 10;


// Called when we load page

function initialise() {
    mainMenu();

    document.getElementById("ipphones_table").innerHTML = "Checking....";

     // kick off cascading CGIs, initialisePage() will be called when complete
	 
    loadExtensions();
}

 
function initialisePage() {
    // 
     var tick = '<img src="tick.png" alt="IP phone detected" />';
     var cross = '<img src="cross.png" alt="No phone detected" />';
    var html = '';
    var j = 0;
	
    html += '<form name="phones" method="get">';

    html += '<table align="right" width=600 cellspacing=2>';

    // print out IP phones
    //  j is 6011,6012 etc

    for (j in ip_ext) {
        if (ip_ext[j] != '') {
            html += "<tr>";
	        html += "<td>" + j + "</td>" ;
	        html += "<td>" + ip_ext[j] + "</td>";
            html += "<td>" + tick + "</td>";
            html += "</tr>";
	    } 
		else {
            html += "<tr>";
	        html += "<td>" + j + "</td>" ;
	        html += "<td>" + "available" + "</td>";
            html += "<td>" + cross + "</td>";
            html += "</tr>";
		}
    }

    html += '</table>';
	
    html += '</form>';
    document.getElementById("ipphones_table").innerHTML = html;

}

function somefunction() {
    // 
    
    var icon = '<img src="tick.png" alt="Analog Phone OK" />';
    var html = '';

    html += '<table align="right" width=600>';

    // print out info

    html += '<tr><td colspan="3" align="left" valign="top"><h2>IP Phones</h2></td></tr>';
    html += '<tr><td colspan="2"><a href="http://rowetel.com/ucasterisk/mini.html">Mini Asterisk GUI2</a></td><td> </td><td>Revision XXX</td></tr>';
    html += '<tr><td colspan="2">Brought to you by the</td><td> </td><td> <a href="http://rowetel.com/ucasterisk/index.html">Free Telephony Project</a></td></tr>';
    // this image will only come up if internet connection is present
    html += '<tr><td colspan="2"><a href="http://rowetel.com/ucasterisk/ip04.html"><img src="http://rowetel.com/images/ip04/ip04_case.jpg" border="0" /></a></td><td> </td><td></td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    html += '</table>';

//    document.getElementById("ipphones").innerHTML += html;

}

