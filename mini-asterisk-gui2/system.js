/*
  about.js
  Ian Roberts
  May 7 2012

  System screen for Mini Asterisk GUI2.
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

    html += '<tr><td colspan="3" align="left" valign="top"><h2>System</h2></td></tr>';
    html += '<tr><td colspan="2">Display results of the following shell commands</td></tr>';
    html += '<tr><td colspan="2">cat /proc/version</td></tr>';
    html += '<tr><td colspan="2">ipkg list_installed</td></tr>';
    html += '<tr><td colspan="2">cat /proc/loadavg</td></tr>';
    html += '<tr><td colspan="2">uptime</td></tr>';
    html += '<tr><td colspan="2">cat /proc/cpuinfo</td></tr>';
    html += '<tr><td colspan="2">cat /proc/mtd</td></tr>';
    html += '<tr><td colspan="2">cat /proc/yaffs</td></tr>';
    html += '<tr><td>&nbsp</td></tr>';
 
    html += '</table>';

    document.getElementById("system").innerHTML += html;

}



