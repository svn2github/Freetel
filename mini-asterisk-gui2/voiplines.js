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
    
 /*   var icon = '<img src="tick.png" alt="Analog Phone OK" />';
    var html = '';

    html += '<table align="right" width=600>';

    // print out info

    html += '<tr><td colspan="3" align="left" valign="top"><h2>Voip Lines</h2></td></tr>';
    html += '<tr><td colspan="2"><a href="http://rowetel.com/ucasterisk/mini.html">Mini Asterisk GUI2</a></td><td> </td><td>Revision XXX</td></tr>';
    html += '<tr><td colspan="2">Brought to you by the</td><td> </td><td> <a href="http://rowetel.com/ucasterisk/index.html">Free Telephony Project</a></td></tr>';
    // this image will only come up if internet connection is present
    html += '<tr><td colspan="2"><a href="http://rowetel.com/ucasterisk/ip04.html"><img src="http://rowetel.com/images/ip04/ip04_case.jpg" border="0" /></a></td><td> </td><td></td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    html += '</table>';

 //   document.getElementById("voiplines").innerHTML += html;
*/
}


function onClickApply() {

//take username, password and host. Insert to file. Restart sip.


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
		document.getElementById('info').value = "No provider selected";
	} else if (selection == "SIPNAT" ) {
		// fill sipnat
		document.getElementById('user').value = "sipnat";
		document.getElementById('host').value = "sipnathost";
		document.getElementById('pass').value = "xxxxxx";
	} else if (selection == "SIP" ) {
	    // fill sip
		document.getElementById('user').value = "sip";
		document.getElementById('host').value = "siphost";
		document.getElementById('pass').value = "xxxxxx";		
	} else if (selection == "JAZMIN" ) {
		// fill jazmin
		document.getElementById('user').value = "jazmin";
		document.getElementById('host').value = "jazminhost";
		document.getElementById('pass').value = "xxxxxx";		
	} else {
		// something weird happened
	}
	
}


