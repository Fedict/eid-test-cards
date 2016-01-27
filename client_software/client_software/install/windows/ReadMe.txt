BE eID Test Infrastructure
==========================

Version 1.2 - September 2011

This software helps you test your developments for the
Belgian eID card by providing virtual cards to use in
your tests.

The 'trick' is to copy the winscard.dll in this directory
to the directory where your test exe is. Your test exe
will now use this 'proxy' winscard.dll instead of the
real winscard.dll in the Windows system library; and this
'proxy' winscard.dll will emulate virtual cards in virtual
smart card readers. The data from those virtual cards is
read from 'virtual card contents files' on the hard disk.

To use the pcsccontrol tool you should have a Java 1.6 or
higher installed.

For more info, see https://env.dev.eid.belgium.be,
e.g. the "eID TE User Guide" under HelpDesk - eSupport.

On this site, you can also order a test card and generate
virtual card content files for that test card.

Quick start guide
=================

This assumes you already have a physical test card.

1. Generate a virtual card contents file for this test card
   e.g. with friendly name "barbara", and save it to e.g. /tmp
   See "Generating virtuals cards" below for more info.

2. Select this virtual card contents file:
   - Open Programs - eID Test Infra - EidTestInfra-control
   - Select Options - Virtual Card Include Directories,
     and add C:\Temp
   - Right-click on "barbara" and select "Set Active"
   - Select Options - Card Reader Visibility - Hide physical
     card reader
   - Select File - Save

3. Download the BE eID middleware (this is the application
   that we will test)

4. Copy the proxy winscard.dll to the directory containing the
   middleware: C:\Program Files\Belgium Identity Card

   The proxy winscard.dll can be found in
     C:\Program Files\Eid Test Infrastructure\proxy32\
   for a 32 bit application; and in
     C:\Program Files\Eid Test Infrastructure\proxy64\
   for a 64 bit application.

5. Tell the middleware to accept test cards: in
   HKEY_LOCAL_MACHINE\SOFTWARE\BEID\certificatevalidation
   add a DWORD "cert_allow_testcard" with value 1.

6. Open Program - "Belgium - eID" - eID-Viewer, you will
   now see the virtual card that you generated. If you
   select Tools - Options and look at the available readers,
   you will only see the virtual smart card reader (whose
   name starts with "!Virtual").

Generating virtual cards
========================

To generate virtual cards you need a test card, that you
can purchase from https://env.dev.eid.belgium.be. 

You can then use that test card to log in to the "Online
Configuration" part of this website to generate and download
virtual cards (which is an xml file).

To log with the test card, you could download the BE eID
middleware from http://eid.belgium.be/. This site also contains
info on how to log in using Firefox, Safari or Internet Explorer.
