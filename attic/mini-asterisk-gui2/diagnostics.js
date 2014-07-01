/*
  diagnostics.js
  Ian Roberts
  May 10 2012

  diagnostics screen for Mini Asterisk GUI2.
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

    html += '<tr><td colspan="3" align="left" valign="top"><h2>Diagnostics</h2></td></tr>';
    html += '<tr><td colspan="2"><a href="http://rowetel.com/ucasterisk/mini.html">Mini Asterisk GUI2</a></td><td> </td><td>Revision XXX</td></tr>';
    html += '<tr><td colspan="2">Brought to you by the</td><td> </td><td> <a href="http://rowetel.com/ucasterisk/index.html">Free Telephony Project</a></td></tr>';
    // this image will only come up if internet connection is present
    html += '<tr><td colspan="2"><a href="http://rowetel.com/ucasterisk/ip04.html"><img src="http://rowetel.com/images/ip04/ip04_case.jpg" border="0" /></a></td><td> </td><td></td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    html += '</table>';

    document.getElementById("diagnostics").innerHTML += html;

}



