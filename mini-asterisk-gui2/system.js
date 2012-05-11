/*
  system.js
  Ian Roberts
  May 7 2012

  System screen for Mini Asterisk GUI2.
*/

var update_time = 10;

var version = "Linux ....";

// Called when we load page

function initialise() {
    mainMenu();

    loadSystemInfo();   // Kick off cgi scripts
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

    html += '<tr><td colspan="2">cat /proc/version</td></tr>' ;
    html += '<tr><td div id="version"></div></td></tr>';

    html += '<tr><td colspan="2">cat /proc/loadavg</td></tr>';
    html += '<tr><td div id="loadavg"></div></td></tr>';

    html += '<tr><td colspan="2">cat /proc/cpuinfo</td></tr>';
    html += '<tr><td div id="cpuinfo"></div></td></tr>';

    html += '<tr><td colspan="2">cat /proc/mtd</td></tr>';
    html += '<tr><td div id="mtd"></div></td></tr>';

    html += '<tr><td colspan="2">cat /proc/meminfo</td></tr>';
    html += '<tr><td div id="meminfo"></div></td></tr>';

    html += '<tr><td colspan="2">cat /proc/yaffs</td></tr>';
    html += '<tr><td div id="yaffs"></div></td></tr>';

    html += '<tr><td colspan="2">uptime</td></tr>';
    html += '<tr><td div id="uptime"></div></td></tr>';

    html += '<tr><td colspan="2">ipkg list_installed</td></tr>';
    html += '<tr><td div id="installed"></div></td></tr>';

    html += '<tr><td>&nbsp</td></tr>';
 
    html += '</table>';

    document.getElementById("system").innerHTML += html;

}

function loadSystemInfo() {

	downloadUrl("/cgi-bin/catproc.cgi?arg=version", loadVersion);

}

function loadVersion(doc, status) {

	document.getElementById('version').innerHTML = doc.substr(doc.indexOf("Linux"), doc.length);

	downloadUrl("/cgi-bin/catproc.cgi?arg=loadavg", loadLoadavg);
}

function loadLoadavg(doc, status) {

	document.getElementById('loadavg').innerHTML = doc.substr(0, doc.length);

	downloadUrl("/cgi-bin/catproc.cgi?arg=cpuinfo", loadCPUinfo);
}

function loadCPUinfo(doc, status) {
	ip = "";

	ip = doc.substr(0, doc.length);
        if (doc.indexOf("CPU") != -1) {

		// Blackfin hardware is ADSP-BF533
		ip = doc.substr(doc.indexOf("CPU"), doc.length);
//		ip = ip.substr(10, ip.length);
//		ip = ip.substr(0, ip.indexOf("power manage"));
    	}

	document.getElementById('cpuinfo').innerHTML = ip;

	downloadUrl("/cgi-bin/catproc.cgi?arg=mtd", loadmtd);
}

function loadmtd(doc, status) {

	document.getElementById('mtd').innerHTML = doc.substr(0, doc.length);

	downloadUrl("/cgi-bin/catproc.cgi?arg=meminfo", loadmeminfo);
}

function loadmeminfo(doc, status) {

	document.getElementById('meminfo').innerHTML = doc.substr(0, doc.length);

	downloadUrl("/cgi-bin/catproc.cgi?arg=yaffs", loadyaffs);
}

function loadyaffs(doc, status) {

	document.getElementById('yaffs').innerHTML = doc.substr(0, doc.length);

	downloadUrl("/cgi-bin/uptime.cgi?arg=nothing", loaduptime);
}

function loaduptime(doc, status) {

	document.getElementById('uptime').innerHTML = doc.substr(0, doc.length);

	downloadUrl("/cgi-bin/ipkg.cgi?arg=list_installed", loadinstalled);
}

function loadinstalled(doc, status) {

	document.getElementById('installed').innerHTML = doc.substr(0, doc.length);

//	downloadUrl("/cgi-bin/catproc.cgi?arg=cpuinfo", loadCPUinfo);
}


