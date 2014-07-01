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


//    document.getElementById("system").innerHTML += html;

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

//	end of script cascade
}


