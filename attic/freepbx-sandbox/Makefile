# Makefile
# David Rowe 12 Feb 2008

# Builds (almost) everything you need for FreePBX with sqlite3 from
# scratch and configures FreePBX to run in a "sandbox" environment.

TOPDIR = $(shell pwd)
BUILD_DIR = $(TOPDIR)
DL_DIR = $(TOPDIR)/dl
ROOT = $(TOPDIR)/root
export PATH:= $(ROOT)/bin:$(PATH)

all: php sqlite pear-db php-sqlite3 xdebug lighttpd freepbx

##############################################################
# PHP --------------------------------------------------------
##############################################################

PHP_VERSION=5.2.5
PHP_DIRNAME=php-$(PHP_VERSION)
PHP_DIR=$(BUILD_DIR)/$(PHP_DIRNAME)
PHP_SITE=http://www.php.net/distributions
PHP_SOURCE=php-$(PHP_VERSION).tar.bz2
PHP_CONFIGURE_OPTS=--disable-all --with-pcre-regex --enable-force-cgi-redirect \
                   --with-config-file-path=$(ROOT)/etc --prefix=$(ROOT) \
		   --with-pear --enable-xml --enable-libxml --enable-session \
	           --with-gettext --enable-debug

$(DL_DIR)/$(PHP_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(PHP_SITE)/$(PHP_SOURCE)

$(PHP_DIR)/.unpacked: $(DL_DIR)/$(PHP_SOURCE)
	bzcat $(DL_DIR)/$(PHP_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(PHP_DIR)/.unpacked

$(PHP_DIR)/.configured: $(PHP_DIR)/.unpacked
	cd $(PHP_DIR); ./configure $(PHP_CONFIGURE_OPTS)
	touch $(PHP_DIR)/.configured

$(PHP_DIR)/.built: $(PHP_DIR)/.configured
	cd $(PHP_DIR); make
	cd $(PHP_DIR); make install
	cp $(PHP_DIR)/php.ini-recommended $(ROOT)/etc/php.ini
	echo include_path=\".:$(ROOT)/lib/php\" >> $(ROOT)/etc/php.ini
	touch $(PHP_DIR)/.built

php: $(PHP_DIR)/.built

##############################################################
# sqlite3 ----------------------------------------------------
##############################################################

SQLITE_VERSION=3.5.6
SQLITE_DIRNAME=sqlite-$(SQLITE_VERSION)
SQLITE_DIR=$(BUILD_DIR)/$(SQLITE_DIRNAME)
SQLITE_SITE=http://www.sqlite.org/
SQLITE_SOURCE=sqlite-amalgamation-$(SQLITE_VERSION).tar.gz
SQLITE_CONFIGURE_OPTS = --prefix=$(ROOT)

$(DL_DIR)/$(SQLITE_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(SQLITE_SITE)/$(SQLITE_SOURCE)

$(SQLITE_DIR)/.unpacked: $(DL_DIR)/$(SQLITE_SOURCE)
	zcat $(DL_DIR)/$(SQLITE_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(SQLITE_DIR)/.unpacked

$(SQLITE_DIR)/.configured: $(SQLITE_DIR)/.unpacked
	cd $(SQLITE_DIR); ./configure $(SQLITE_CONFIGURE_OPTS)
	touch $(SQLITE_DIR)/.configured

$(SQLITE_DIR)/.built: $(SQLITE_DIR)/.configured
	cd $(SQLITE_DIR); make
	cd $(SQLITE_DIR); make install
	touch $(SQLITE_DIR)/.built

sqlite: $(SQLITE_DIR)/.built

##############################################################
# PEAR:DB  ---------------------------------------------------
##############################################################

$(ROOT)/lib/php/.db-installed:
	pear install DB
	cd $(ROOT)/lib/php/DB; patch -p1 < $(TOPDIR)/patch/common.patch 
	touch $(ROOT)/lib/php/.db-installed

pear-db: $(ROOT)/lib/php/.db-installed

##############################################################
# php-sqlite3 ------------------------------------------------
##############################################################

PHP-SQLITE3_VERSION=0.5
PHP-SQLITE3_DIRNAME=sqlite3-$(PHP-SQLITE3_VERSION)
PHP-SQLITE3_DIR=$(BUILD_DIR)/$(PHP-SQLITE3_DIRNAME)
PHP-SQLITE3_SITE=http://optusnet.dl.sourceforge.net/sourceforge/php-sqlite3/
PHP-SQLITE3_SOURCE=sqlite3-$(PHP-SQLITE3_VERSION).tgz
PHP-SQLITE3_CONFIGURE_OPTS=--prefix=$(ROOT) \
	                   --with-sqlite3=$(ROOT) \
	                   --with-php-config=$(ROOT)/bin/php-config

$(DL_DIR)/$(PHP-SQLITE3_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(PHP-SQLITE3_SITE)/$(PHP-SQLITE3_SOURCE)

$(PHP-SQLITE3_DIR)/.unpacked: $(DL_DIR)/$(PHP-SQLITE3_SOURCE)
	zcat $(DL_DIR)/$(PHP-SQLITE3_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(PHP-SQLITE3_DIR)/.unpacked

$(PHP-SQLITE3_DIR)/.configured: $(PHP-SQLITE3_DIR)/.unpacked
	cd $(PHP-SQLITE3_DIR); phpize; ./configure $(PHP-SQLITE3_CONFIGURE_OPTS)
	touch $(PHP-SQLITE3_DIR)/.configured

$(PHP-SQLITE3_DIR)/.built: $(PHP-SQLITE3_DIR)/.configured
	cd $(PHP-SQLITE3_DIR); make
	cd $(PHP-SQLITE3_DIR); make install

	# use modified sqlite3.php for PHP 5.  We use a sym link so that
	# we can edit it in place and capture changes to source control

	ln -sf $(TOPDIR)/files/sqlite3.php $(ROOT)/lib/php/DB/sqlite3.php

	# update php.ini extensions line for sqlite3.so 

	cat $(ROOT)/etc/php.ini | sed '/extension_dir/ d' > $(ROOT)/etc/php.ini.tmp
	mv $(ROOT)/etc/php.ini.tmp $(ROOT)/etc/php.ini
	echo extension_dir=\"`find $(ROOT) -name sqlite3.so | xargs dirname`\" >> $(ROOT)/etc/php.ini

	touch $(PHP-SQLITE3_DIR)/.built

php-sqlite3: $(PHP-SQLITE3_DIR)/.built

##############################################################
# xdebug -----------------------------------------------------
##############################################################

XDEBUG_VERSION=2.0.2
XDEBUG_DIRNAME=xdebug-$(XDEBUG_VERSION)
XDEBUG_DIR=$(BUILD_DIR)/$(XDEBUG_DIRNAME)
XDEBUG_SITE=http://www.xdebug.org/files/
XDEBUG_SOURCE=xdebug-$(XDEBUG_VERSION).tgz
XDEBUG_CONFIGURE_OPTS=--prefix=$(ROOT) --enable-xdebug 

$(DL_DIR)/$(XDEBUG_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(XDEBUG_SITE)/$(XDEBUG_SOURCE)

$(XDEBUG_DIR)/.unpacked: $(DL_DIR)/$(XDEBUG_SOURCE)
	zcat $(DL_DIR)/$(XDEBUG_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(XDEBUG_DIR)/.unpacked

$(XDEBUG_DIR)/.configured: $(XDEBUG_DIR)/.unpacked
	cd $(XDEBUG_DIR); phpize; ./configure $(XDEBUG_CONFIGURE_OPTS)
	touch $(XDEBUG_DIR)/.configured

$(XDEBUG_DIR)/.built: $(XDEBUG_DIR)/.configured
	cd $(XDEBUG_DIR); make
	cp $(XDEBUG_DIR)/modules/xdebug.so $(ROOT)/lib

	# update php.ini extensions line for xdebug.so 

	echo zend_extension=\"$(ROOT)/lib/xdebug.so\" >> $(ROOT)/etc/php.ini
	echo "xdebug.auto_trace=On" >> $(ROOT)/etc/php.ini
	echo "xdebug.show_exception_trace=On" >> $(ROOT)/etc/php.ini

	touch $(XDEBUG_DIR)/.built

xdebug: $(XDEBUG_DIR)/.built

#########################################################################
# lighttpd --------------------------------------------------------------
#########################################################################

LIGHTTPD_VERSION=1.4.18
LIGHTTPD_DIRNAME=lighttpd-$(LIGHTTPD_VERSION)
LIGHTTPD_DIR=$(BUILD_DIR)/$(LIGHTTPD_DIRNAME)
LIGHTTPD_SITE=http://www.lighttpd.net/download
LIGHTTPD_SOURCE=lighttpd-$(LIGHTTPD_VERSION).tar.gz
LIGHTTPD_CONFIGURE_OPTS=--prefix=$(ROOT) --disable-ipv6

$(DL_DIR)/$(LIGHTTPD_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(LIGHTTPD_SITE)/$(LIGHTTPD_SOURCE)

$(LIGHTTPD_DIR)/.unpacked: $(DL_DIR)/$(LIGHTTPD_SOURCE)
	zcat $(DL_DIR)/$(LIGHTTPD_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(LIGHTTPD_DIR)/.unpacked

$(LIGHTTPD_DIR)/.configured: $(LIGHTTPD_DIR)/.unpacked
	cd $(LIGHTTPD_DIR); ./configure $(LIGHTTPD_CONFIGURE_OPTS)
	touch $(LIGHTTPD_DIR)/.configured

$(LIGHTTPD_DIR)/.built: $(LIGHTTPD_DIR)/.configured
	cd $(LIGHTTPD_DIR); make
	cd $(LIGHTTPD_DIR); make install

	cp files/lighttpd.conf $(ROOT)/etc
	mkdir -p $(ROOT)/www
	cp files/test.php $(ROOT)/www

	# change server root and set up cgi-assign for PHP

	cat $(ROOT)/etc/lighttpd.conf | sed '/server.document-root/ d' > $(ROOT)/etc/lighttpd.conf.tmp
	mv $(ROOT)/etc/lighttpd.conf.tmp $(ROOT)/etc/lighttpd.conf
	echo "server.document-root = \"$(ROOT)/www/\"" >> $(ROOT)/etc/lighttpd.conf
	echo "cgi.assign = (\".php\" => \"$(ROOT)/bin/php-cgi\")" >> $(ROOT)/etc/lighttpd.conf

	touch $(LIGHTTPD_DIR)/.built

lighttpd: $(LIGHTTPD_DIR)/.built

#########################################################################
# freepbx ---------------------------------------------------------------
#########################################################################

FREEPBX_VERSION=2.4.0
FREEPBX_DIRNAME=freepbx-$(FREEPBX_VERSION)
FREEPBX_DIR=$(BUILD_DIR)/$(FREEPBX_DIRNAME)
FREEPBX_SITE= http://mirror.freepbx.org
FREEPBX_SOURCE=freepbx-$(FREEPBX_VERSION).tar.gz
FREEPBX_CONFIGURE_OPTS=

# sed command to change hard coded PHP /etc/amportal.conf path

FREEPBX_AMP_CONF='s|"/etc/amportal.conf"|"$(ROOT)/etc/amportal.conf"|'

$(DL_DIR)/$(FREEPBX_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(FREEPBX_SITE)/$(FREEPBX_SOURCE)

$(FREEPBX_DIR)/.unpacked: $(DL_DIR)/$(FREEPBX_SOURCE)
	zcat $(DL_DIR)/$(FREEPBX_SOURCE) | tar -C $(BUILD_DIR) -xf -
	patch -d $(FREEPBX_DIR) -p1 < patch/freepbx.patch

	# additional files to implement retrieve_conf as include

	cp files/retrieve_conf.inc.php files/do_reload.php \
	$(FREEPBX_DIR)/amp_conf/htdocs/admin

	touch $(FREEPBX_DIR)/.unpacked

$(FREEPBX_DIR)/.installed: $(FREEPBX_DIR)/.unpacked

	# initialise databases (freepbx and CDR)

	mkdir -p $(ROOT)/var
	cat $(FREEPBX_DIR)/SQL/newinstall.sqlite3.sql | sqlite3 $(ROOT)/var/freepbx.db
	touch $(ROOT)/var/asteriskcdr.db

	# edit amportal.conf

        #     out with the old......

	cat $(FREEPBX_DIR)/amportal.conf | \
	sed -e '/AMPDBENGINE=/ d' -e '/AMPDBFILE=/ d' \
	-e '/AMPWEBROOT=/ d' -e '/FOPWEBROOT=/ d' \
	-e '/AMPDBHOST=/ d' -e '/AMPDBUSER=/ d' -e '/AMPDBPASS=/ d' \
	> $(FREEPBX_DIR)/amportal.conf.tmp

	mv $(FREEPBX_DIR)/amportal.conf.tmp $(FREEPBX_DIR)/amportal.conf

        #    in with the new........

	echo "AMPDBENGINE=sqlite3" >> $(FREEPBX_DIR)/amportal.conf
	echo "AMPDBFILE=$(ROOT)/var/freepbx.db" >> $(FREEPBX_DIR)/amportal.conf
	echo "AMPWEBROOT=$(ROOT)/www" >> $(FREEPBX_DIR)/amportal.conf
	echo "FOPWEBROOT=$(ROOT)/www/panel" >> $(FREEPBX_DIR)/amportal.conf

	sed -i "s|/var/www/html|$(ROOT)/www|" $(FREEPBX_DIR)/amportal.conf

	# Wholesale hacking of FreePBX source to move AMP_CONF to our local conf dir:
	# FYI to find where AMP_CONF is used:
        #     grep -r "define(\"AMP_CONF\", \"/etc/amportal.conf\")" *

	cd $(FREEPBX_DIR); \
	FILES=`grep -r "\"/etc/amportal.conf\"" * | sed 's/:.*//'`; \
	for f in $$FILES; do \
		sed -i $(FREEPBX_AMP_CONF) $$f; \
	done

	# change recordings/includes/main.conf.php DBENGINE and DBFILE settings which
	# are hard coded for mysql

	sed -i "s|ASTERISKCDR_DBENGINE.*|ASTERISKCDR_DBENGINE=\"sqlite3\";|" \
	$(FREEPBX_DIR)/amp_conf/htdocs/recordings/includes/main.conf.php
	sed -i "s|ASTERISKCDR_DBFILE.*|ASTERISKCDR_DBFILE=\"$(ROOT)/var/asteriskcdr.db\";|" \
	$(FREEPBX_DIR)/amp_conf/htdocs/recordings/includes/main.conf.php

	# change admin/cdr/lib/defines DB_TYPE and DBNAME settings

	sed -i "s|\"DBNAME\".*|\"DBNAME\",\"$(ROOT)/var/asteriskcdr.db\");|" \
	$(FREEPBX_DIR)/amp_conf/htdocs/admin/cdr/lib/defines.php
	sed -i "s|\"DB_TYPE\".*|\"DB_TYPE\",\"sqlite3\");|" \
	$(FREEPBX_DIR)/amp_conf/htdocs/admin/cdr/lib/defines.php

	# switch off mp3 support as I dont have asterisk-addons and Asterisk keeps
	# crashing when I start it

	sed -i "s|load => format_mp3.so|;load => format_mp3.so|" \
	$(FREEPBX_DIR)/amp_conf/astetc/modules.conf

	# cp amportal.conf ourselves to bypass a lot of questions in install script

	cp $(FREEPBX_DIR)/amportal.conf $(ROOT)/etc

	# run installer

	sudo mkdir -p $(ROOT)/var/run/asterisk; sudo chmod 777 $(ROOT)/var/run/asterisk; 
	sudo mkdir -p $(ROOT)/var/lib/asterisk/bin; sudo chmod 777 $(ROOT)/var/lib/asterisk/bin; 
	cd $(FREEPBX_DIR); sudo ./install_amp; sudo chmod 777 $(ROOT)/www -R

	#touch $(FREEPBX_DIR).installed


freepbx: $(FREEPBX_DIR)/.installed

#########################################################################
# clean  ----------------------------------------------------------------
#########################################################################

clean: 
	rm -Rf $(PHP_DIR) $(SQLITE_DIR) $(PHP-SQLITE3_DIR) \
	$(XDEBUG_DIR) $(LIGHTTPD_DIR) $(FREEPBX_DIR) root \
	$(FREEPBX_DIR)-orig

	find . -name '*~' | xargs rm -f
	rm -f *.xml # where do these come from?

#########################################################################
# freepbx-make-patch  ---------------------------------------------------
#########################################################################

# Generate patches between original tar ball $(AO) and either the
# installed files $(AROOT), or modified tar ball $A.
# Run this Makefile target to capture any changes to patches

AO    = $(FREEPBX_DIRNAME)-orig/
AROOT = root/www
A     = $(FREEPBX_DIRNAME)/

freepbx-make-patch:

        # untar original, to save time we check if the orig is already there

	if [ ! -d $(FREEPBX_DIR)-orig ] ; then \
		cd $(DL_DIR); tar xvf $(FREEPBX_SOURCE); \
		mv $(FREEPBX_DIRNAME) $(FREEPBX_DIR)-orig; \
	fi

	# make big-ass SQL statement work with sqlite3

	-diff -uN \
	$(AO)/amp_conf/htdocs/admin/modules/core/functions.inc.php \
	$(AROOT)/admin/modules/core/functions.inc.php \
	> patch/freepbx.patch

	# add $options array to help numRows() support in sqlite3

	-diff -uN \
	$(AO)/amp_conf/htdocs/admin/common/db_connect.php \
	$(AROOT)/admin/common/db_connect.php \
	>> patch/freepbx.patch

	# sqlite3 support for cdr in recordings

	-diff -uN \
	$(AO)/amp_conf/htdocs/recordings/includes/database.php \
	$(AROOT)/recordings/includes/database.php \
	>> patch/freepbx.patch

	# sqlite3 support for cdr

	-diff -uN \
	$(AO)/amp_conf/htdocs/admin/cdr/lib/defines.php \
	$(AROOT)/admin/cdr/lib/defines.php \
	>> patch/freepbx.patch

	# trap missing cdr table error in Reports menu

	-diff -uN \
	$(AO)/amp_conf/htdocs/admin/cdr/lib/Class.Table.php \
	$(AROOT)/admin/cdr/lib/Class.Table.php \
	>> patch/freepbx.patch

	# typo in newinstall.sqlite3.sql

	-diff -uN \
	$(AO)/SQL/newinstall.sqlite3.sql \
	$(A)/SQL/newinstall.sqlite3.sql \
	>> patch/freepbx.patch

	# stop installer warning/errors when sqlite3 and blackfin used

	-diff -uN \
	$(AO)/install_amp \
	$(A)/install_amp \
	>> patch/freepbx.patch

	# use sqlite3 SQL syntax for recording installer

	-diff -uN \
	$(AO)/amp_conf/htdocs/admin/modules/recordings/install.php \
	$(A)/amp_conf/htdocs/admin/modules/recordings/install.php \
	>> patch/freepbx.patch

	# 1/ stop perl script for flash panel being called, as
	# this won't work on Blackfin (might work on x86 with
	# appropriate Perl lib but not tested)
	#
	# 2/ Remove code to set up queues, as it seems to be killing retrive_conf
	# Is this needed or a legacy thing?

	-diff -uN \
	$(AO)/amp_conf/bin/retrieve_conf \
	$(A)/amp_conf/bin/retrieve_conf \
	>> patch/freepbx.patch

	# set up default AMP user and password

	-diff -uN \
	$(AO)/amp_conf/astetc/manager.conf \
	$(A)/amp_conf/astetc/manager.conf \
	>> patch/freepbx.patch

	# make retrieve_conf an include from config.php rather
        # than exec to save about 15M

	-diff -uN \
	$(AO)/amp_conf/htdocs/admin/config.php \
	$(AROOT)/admin/config.php \
	>> patch/freepbx.patch

	# NOTE: common.patch was generated manually from command line

	# undo sandbox path (messy!  Must be a better way....)

	sed -i 's|$(ROOT)||' patch/freepbx.patch



