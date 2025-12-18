/* $Id: config.c,v 1.1 2005/11/17 22:48:40 bercegeay Exp $ */

/**@file*/

/** @page Configfile Configuration of windom

WinDom Configuration is performed via an unique file that we call
the WinDom Configuration file. The main idea is: as WinDom is a
GEM front end, the choice of window, object, ... apparence should
not be fixed by the program  but only by the user. For that purpose, 
WinDom reads an external file, the WinDom configuration file, the
parameters of look and feel of the library.

In addition, WinDom offers to the programmer an easy way to store,
read, and handle a configuration setup. Specific variable can
be written and read from the windom configuration file.

The configuration is a text editable file. The syntaxe is very
simple. However, a special application, WinConf written by 
Dominique BEREZIAT, allows users to create and handle their configuration file.
From WinConf version 2, any variables is handled (from WinDom or
from specific application). The interaction is completly graphical
(with buttons, popup menus, objects selectors). WinDom application
can communicate with WinConf using a GEM protocol and the settings
can be updated in real time.

\section locationconfigfile Location of the configuration file

The configuration file is a file 'windom.cnf' or '.windomrc' if your file
system supports the long file name. This file can be
placed in the following directories :

	    - the application directory,
	    - the $HOME\\Defaults directory,
	    - the $HOME directory,
	    - the $ETCDIR directory,
	    - the $PATH directories	(can be multiple),
	    - the 'C:\\' directory

The application directory allow to have a direct configuration of
your application when a user install a first time your application.

The $HOME directories allow a personnal configuration in a
multi user environnement.

The $ETCDIR directories allow WinDom application to live happily in an 
UniX-like file system organization. Perhaps it is better to read directly
in the U:\\ETC (or /etc) directory, i don't know, i'm waiting for users 
feedbacks.

\section contentofconfigfile Where applications are defined

First of all, commented lines begin by the '#' character. Example:

@verbatim
# This variable forces application to display the windom version
windom.version = true
@endverbatim

The settings of an application are grouped in the same place
between two special keywords: the application tag and the
end tag. An application tag is just the application name
(with higher characters) between brackets. Example:

@verbatim
[WINCONF] # begin of the configuration area of WINCONF application
# body of the application configuration 
[end]     # end of configuration
@endverbatim

Because a configuration addresses a specific application, it could
be interesting to have a global settings addressing all applications.
It is possible with the special tag [Default Settings]. An application
not defined in the configuration file will read its settings in this
area.

\section configfilevar Variables of the config file

For the list of variables, refer to ::_config structure reference documentation.
This structure stores the configuration of the windom application.

\section configuserdef Windom2 applications are skinnable

TODO: add some notes about customisation of form objects thanks to userdef LDG.

 */
