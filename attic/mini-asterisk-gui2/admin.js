/*
  admin.js
  Ian Roberts
  May 10 2012

  Admin screen for Mini Asterisk GUI2.
*/

var update_time = 10;
var firmwareurl = "hi";
var newpassword = "hello";



function initialise() {
    mainMenu();

    initialisePage();
}


function initialisePage() {
    
    

}

function onClickSetPassword() {

        newpassword= document.pass.value;
        downloadUrl("/cgi-bin/setpassword.cgi?newpassword="+newpassword,OnSetPasswordReturn);

//      if not x86 (developement environment)
//	    pass=`echo "$QUERY_STRING" | grep -oe "pass=[^&?]*" | sed -n "s/pass=//p"`
//	    passwd_cmdline $pass

}


function onClickInstall() {

	firmwareurl=document.firmwareurl.value;
	downloadUrl("/cgi-bin/install.cgi?file="+firmwareurl,OnInstallReturn);

//      if not x86 (developement environment)
//    firmwareurl=`echo "$QUERY_STRING" | grep -oe "firmwareurl=[^&?]*" | sed -n "s/firmwareurl=//p"`
//    # convert URL encoded string, this can just handle : and / so be careful
//    # with script names
//    firmwareurl=`echo $firmwareurl | sed -e "s/%3A/:/" -e "s_%2F_/_g"`
//    wget $firmwareurl
//    filename=`echo $firmwareurl | sed 's_^.*/__'`
//    sh $filename
//    exit

}

function onClickUpgrade() {

	downloadUrl("/cgi-bin/upgrade.cgi?arg=",OnUpgradeReturn);

//      if not x86 (developement environment)
//    rev_before=`grep -oe "Revision: [0-9]*" /www/about.sh`
//    echo "<strong>Current $rev_before</strong><br>"
//    echo "<strong>Removing $ver_before</strong><br>"
//    ipkg remove mini-asterisk-gui | tr '\n' '#' | sed -n 's/\#/<br>/pg'
//    echo "<strong>Installing...</strong><br>"
//    ipkg update | tr '\n' '#' | sed -n 's/\#/<br>/pg'
//    ipkg install mini-asterisk-gui | tr '\n' '#' | sed -n 's/\#/<br>/pg'


}


function onClickReset() {

	downloadUrl("/cgi-bin/reset.cgi?arg=",OnResetReturn);

//      if not x86 (developement environment)
//	cp /etc/asterisk/extensions.conf.def /etc/asterisk/extensions.conf
//	cp /etc/asterisk/sip.conf.def /etc/asterisk/sip.conf
//	asterisk -rx "sip reload" 2>/dev/null 1 > /dev/null
//	asterisk -rx "dialplan reload" 2>/dev/null 1 > /dev/null

}


function onClickRestart() {

	downloadUrl("/cgi-bin/restart.cgi?arg=",OnRestartReturn);

//      if not x86 (developement environment)
//          reboot
}

