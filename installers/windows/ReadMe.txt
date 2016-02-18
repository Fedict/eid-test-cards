BE eID Test Infrastructure
==========================

Version 1.2.1 - February 2016

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

To use the pcsccontrol tool you should have a Java installed.

Quick start guide
=================

This assumes you already have a physical test card.

1. Generate a virtual card contents file for this test card
   e.g. with friendly name "barbara", and save it to e.g. /tmp
   See "Generating virtuals cards" below for more info.

2. Select this virtual card contents file:
   - Open Programs - 2_PCSCControl - pcsccontrol.jar
   - Select Options - Virtual Card Include Directories,
     and add C:\Temp
   - Right-click on "barbara" and select "Set Active"
   - Select Options - Card Reader Visibility - Hide physical
     card reader
   - Select File - Save

3. Download the BE eID middleware from eid.belgium.be(this is the 
application that we will test)

4. Copy the proxy winscard.dll to the directory containing java.exe: e.g. C:\Program Files (x86)\Java\jre1.8.0_71\bin

   The proxy winscard.dll can be found in
     -)3_PCSCProxy\Win32\ for a 32 bit application;
	-)and in 3_PCSCProxy\x64 for a 64 bit application.

6. Open Program - "Belgium - eID" - eID-Viewer, you will
   now see the virtual card that you generated.

Generating virtual cards
========================

To generate virtual cards you need a test card.

You can then use that test card to create virtual cards (which are an xml files).

You do this by running 1_gen_virtual_cards/generate.
For more info see 1_gen_virtual_cards/generate/readme.txt
