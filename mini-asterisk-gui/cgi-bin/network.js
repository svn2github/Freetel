var script_path = "cgi-bin/";

function doStatic() {    
    $('ipaddress').disabled = 0;
    $('netmask').disabled = 0;
    $('gateway').disabled = 0;
    $('dns').disabled = 0;
}

function doDHCP() {
    $('ipaddress').disabled = 1;
    $('netmask').disabled = 1;
    $('gateway').disabled = 1;
    $('dns').disabled = 1;
}

// http://moblog.bradleyit.com/2009/06/javascript-ip-address-validation.html

function isIP(obj) {
    var ary = obj.value.split(".");
    var ip = true;

    ip = (ary.length == 4); 

    if (ip) {
	for (var i=0; i<4; i++) { 
		ip = (!ary[i].match(/^\d{1,3}$/) || (Number(ary[i]) > 255)) ? false : ip; 
    	} 
    }

    if (!ip) {    
	// the value is NOT a valid IP address
	obj.style.background = "red";
	obj.select();
    } 
    else { obj.style.background = ""; } // the value IS a valid IP address

    return ip;
}

function localInit() {

    if (init_dhcp == "yes") {
	$('dhcp').checked = true;
	$('ipaddress').disabled = 1;
	$('netmask').disabled = 1;
	$('gateway').disabled = 1;
	$('dns').disabled = 1;
    }
    else {
	$('static').checked = true;
	$('ipaddress').disabled = 0;
	$('netmask').disabled = 0;
	$('gateway').disabled = 0;
	$('dns').disabled = 0;
    }
    $('ipaddress').value = init_ipaddress;
    $('netmask').value = init_netmask;
    $('gateway').value = init_gateway;
    $('dns').value = init_dns;
    $('backdoor').value = init_backdoor;    
    if (init_internet == "yes")
	$('internet').innerHTML = '<img src="tick.png" alt="tick" />';
    else
        $('internet').innerHTML = '<img src="cross.png" alt="tick" />';

}

function validate_form(form)
{
    var valid = true;

    if ($('dhcp').checked == false)
	valid = isIP(form.ipaddress) && isIP(form.ipaddress) && isIP(form.gateway) && isIP(form.dns);

    return valid;
}
