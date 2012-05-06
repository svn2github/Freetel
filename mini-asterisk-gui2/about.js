/*
  about.js
  Ian Roberts
  May 7 2010

  About screen for Mini Asterisk GUI.
*/

var update_time = 10;


// Called when we load page

function initialise() {
    mainMenu();

    initialisePage();
}


function initialisePage() {
    // 
    
    var icon = '<img src="tick.png" alt="Analog Phone OK" />';
    var html = '';

    html += '<table align="right" width=600>';

    // print out info

    html += '<tr><td colspan="3" align="left" valign="top"><h2>About</h2></td></tr>';
    html += '<tr><td colspan="2">Mini Asterisk</td><td> </td><td>Revision XXX</td></tr>';
    html += '<tr><td colspan="2">Brought to you by the</td><td> </td><td>Free Telephony Project</td></tr>';
    html += '<tr><td colspan="2">Image goes here</td><td> </td><td>Pic</td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    // print out analog phones

/*    html += '<tr><td colspan="4" align="left" valign="top"><h2>Phones</h2></td></tr>';
    for(var i=0; i<zap.length; i++) {
        if (zap[i] == 'FXS') {
            html += "<tr>";
	    html += "<td>" + analog_ext[i] + "</td>" + "<td>" + "Analog Phone" + "</td>";
	    html += "<td>Port" + i  + "</td>";
            html += "<td>" + '<img src="tick.png" />' + "</td>";
            html += "</tr>";
	}
    }
*/
 

    html += '</table>';

    document.getElementById("about").innerHTML += html;

}



