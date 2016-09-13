How to create virtual cards (xml files that are loaded by the PCSC proxy lib)

1. Read out the physical card with eid-viewer and save as "XML File" (not: "CSV File").
     Obviously ,this should only be done once for every physical card.

2. Edit base.txt (or first make a copy) and change the values you want.
     To avoid conflicts, it's best to change at least first 4 values:
        friendlyName
        cardNumber
        nationalNumber
        chipNumber

3. Generate the virtual card with the generate.sh or generate.bat script
     E.g. if the XML file saved from eid-viewer is 71715100070.eid
          and the file from point 2 is base_test1.txt, then do:
       generate.bat base_test1.txt 71715100070.eid out_test1.xml

Now you can use this resulting virtual card file out_test1.xml with
the proxy PCSC lib after enabling it with the PCSC control tool,
as explaned in the "TE user manual" (see docs).

