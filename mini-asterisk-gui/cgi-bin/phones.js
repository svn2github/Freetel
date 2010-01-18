function localInit() {
    if (init_internet == "yes")
	$('internet').innerHTML = '<img src="tick.png" alt="tick" />';
    else
        $('internet').innerHTML = '<img src="cross.png" alt="tick" />';
}

