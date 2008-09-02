echo testing Code 11
zint -o bar01.png -b 1 --height=50 -d 87654321
zint -o bar01.eps -b 1 --height=50 -d 87654321
echo testing Code 2 of 5 Standard
zint -o bar02.png -b 2 --height=50 -d 87654321
zint -o bar02.eps -b 2 --height=50 -d 87654321
echo testing Interleaved 2 of 5
zint -o bar03.png -b 3 --height=50 -d 87654321
zint -o bar03.eps -b 3 --height=50 -d 87654321
echo testing Code 2 of 5 IATA
zint -o bar04.png -b 4 --height=50 -d 87654321
zint -o bar04.eps -b 4 --height=50 -d 87654321
echo testing Code 2 of 5 Data Logic
zint -o bar06.png -b 6 --height=50 -d 87654321
zint -o bar06.eps -b 6 --height=50 -d 87654321
echo testing Code 2 of 5 Industrial
zint -o bar07.png -b 7 --height=50 -d 87654321
zint -o bar07.eps -b 7 --height=50 -d 87654321
echo testing Code 39
zint -o bar08.png -b 8 --height=50 -d CODE39
zint -o bar08.eps -b 8 --height=50 -d CODE39
echo testing Extended Code 39
zint -o bar09.png -b 9 --height=50 -d 'Code 39e'
zint -o bar09.eps -b 9 --height=50 -d 'Code 39e'
echo testing EAN8
zint -o bar10.png -b 13 --height=50 -d 7654321
zint -o bar10.eps -b 13 --height=50 -d 7654321
echo testing EAN8 - 2 digits add on
zint -o bar11.png -b 13 --height=50 -d 7654321+21
zint -o bar11.eps -b 13 --height=50 -d 7654321+21
echo testing EAN8 - 5 digits add-on
zint -o bar12.png -b 13 --height=50 -d 7654321+54321
zint -o bar12.eps -b 13 --height=50 -d 7654321+54321
echo testing EAN13
zint -o bar13.png -b 13 --height=50 -d 210987654321
zint -o bar13.eps -b 13 --height=50 -d 210987654321
echo testing EAN13 - 2 digits add-on
zint -o bar14.png -b 13 --height=50 -d 210987654321+21
zint -o bar14.eps -b 13 --height=50 -d 210987654321+21
echo testing EAN13 - 5 digits add-on
zint -o bar15.png -b 13 --height=50 -d 210987654321+54321
zint -o bar15.eps -b 13 --height=50 -d 210987654321+54321
echo testing GS1-128
zint -o bar16.png -b 16 --height=50 -d "[01]98898765432106[3202]012345[15]991231"
zint -o bar16.eps -b 16 --height=50 -d "[01]98898765432106[3202]012345[15]991231"
echo testing CodaBar
zint -o bar18.png -b 18 --height=50 -d D765432C
zint -o bar18.eps -b 18 --height=50 -d D765432C
echo testing Code 128
zint -o bar20.png -b 20 --height=50 -d 'Code 128'
zint -o bar20.eps -b 20 --height=50 -d 'Code 128'
echo testing Deutshe Post Leitcode
zint -o bar21.png -b 21 --height=50 -d 3210987654321
zint -o bar21.eps -b 21 --height=50 -d 3210987654321
echo testing Deutche Post Identcode
zint -o bar22.png -b 22 --height=50 -d 10987654321
zint -o bar22.eps -b 22 --height=50 -d 10987654321
echo testing Code 16k
zint -o bar23.png -b 23 --height=50 -d 'Code 16k is a stacked symbology'
zint -o bar23.eps -b 23 --height=50 -d 'Code 16k is a stacked symbology'
echo testing Code 93
zint -o bar25.png -b 25 --height=50 -d 'Code 93'
zint -o bar25.eps -b 25 --height=50 -d 'Code 93'
echo testing Flattermarken
zint -o bar28.png -b 28 --height=50 -d 87654321
zint -o bar28.eps -b 28 --height=50 -d 87654321
echo testing GS1 DataBar-14
zint -o bar29.png -b 29 --height=33 -d 2001234567890
zint -o bar29.eps -b 29 --height=33 -d 2001234567890
echo testing GS1 DataBar Limited
zint -o bar30.png -b 30 --height=50 -w 2 -d 31234567890
zint -o bar30.eps -b 30 --height=50 -w 2 -d 31234567890
echo testing GS1 DataBar Expanded
zint -o bar31.png -b 31 --height=50 -d "[01]90012345678908[3103]001750"
zint -o bar31.eps -b 31 --height=50 -d "[01]90012345678908[3103]001750"
echo testing Telepen Alpha
zint -o bar32.png -b 32 --height=50 -d 'Telepen'
zint -o bar32.eps -b 32 --height=50 -d 'Telepen'
echo testing UPC A
zint -o bar34.png -b 34 --height=50 -d 10987654321
zint -o bar34.eps -b 34 --height=50 -d 10987654321
echo testing UPC A - 2 digit add-on
zint -o bar35.png -b 34 --height=50 -d 10987654321+21
zint -o bar35.eps -b 34 --height=50 -d 10987654321+21
echo testing UPC A - 5 digit add-on
zint -o bar36.png -b 36 --height=50 -d 10987654321+54321
zint -o bar36.eps -b 36 --height=50 -d 10987654321+54321
echo testing UPC E
zint -o bar37.png -b 37 --height=50 -d 654321
zint -o bar37.eps -b 37 --height=50 -d 654321
echo testing UPC E - 2 digit add-on
zint -o bar38.png -b 37 --height=50 -d 654321+21
zint -o bar38.eps -b 37 --height=50 -d 654321+21
echo testing UPC E - 5 digit add-on
zint -o bar39.png -b 37 --height=50 -d 654321+54321
zint -o bar39.eps -b 37 --height=50 -d 654321+54321
echo testing PostNet-6
zint -o bar41.png -b 40 -d 54321
zint -o bar41.eps -b 40 -d 54321
echo testing PostNet-10
zint -o bar43.png -b 40 -d 987654321
zint -o bar43.eps -b 40 -d 987654321
echo testing PostNet-12
zint -o bar45.png -b 40 -d 10987654321
zint -o bar45.eps -b 40 -d 10987654321
echo testing MSI Code
zint -o bar47.png -b 47 --height=50 -d 87654321
zint -o bar47.eps -b 47 --height=50 -d 87654321
echo testing FIM
zint -o bar49.png -b 49 --height=50 -d D
zint -o bar49.eps -b 49 --height=50 -d D
echo testing LOGMARS
zint -o bar50.png -b 50 --height=50 -d LOGMARS
zint -o bar50.eps -b 50 --height=50 -d LOGMARS
echo testing Pharmacode One-Track
zint -o bar51.png -b 51 --height=50 -d 123456
zint -o bar51.eps -b 51 --height=50 -d 123456
echo testing Pharmazentralnumber
zint -o bar52.png -b 52 --height=50 -d 654321
zint -o bar52.eps -b 52 --height=50 -d 654321
echo testing Pharmacode Two-Track
zint -o bar53.png -b 53 --height=50 -d 12345678
zint -o bar53.eps -b 53 --height=50 -d 12345678
echo testing PDF417
zint -o bar55.png -b 55 -d 'PDF417 is a stacked symbology'
zint -o bar55.eps -b 55 -d 'PDF417 is a stacked symbology'
echo testing PDF417 Truncated
zint -o bar56.png -b 56 -d 'PDF417 is a stacked symbology'
zint -o bar56.eps -b 56 -d 'PDF417 is a stacked symbology'
echo testing Maxicode
zint -o bar57.png -b 57 --primary="999999999840012" -d 'UPS Maxicode with hexagons'
zint -o bar57.eps -b 57 --primary="999999999840012" -d 'UPS Maxicode with hexagons'
echo testing QR Code
zint -o bar58.png -b 58 -d 'QR Code is a matrix symbology'
zint -o bar58.eps -b 58 -d 'QR Code is a matrix symbology'
echo testing Code 128 Subset B
zint -o bar60.png -b 60 --height=50 -d 87654321
zint -o bar60.eps -b 60 --height=50 -d 87654321
echo testing Australian Post Standard Customer
zint -o bar63.png -b 63 -d 87654321
zint -o bar63.eps -b 63 -d 87654321
echo testing Australian Post Customer 2
zint -o bar64.png -b 63 -d 87654321AUSPS
zint -o bar64.eps -b 63 -d 87654321AUSPS
echo testing Australian Post Customer 3
zint -o bar65.png -b 63 -d '87654321 AUSTRALIA'
zint -o bar65.eps -b 63 -d '87654321 AUSTRALIA'
echo testing Australian Post Reply Paid
zint -o bar66.png -b 66 -d 87654321
zint -o bar66.eps -b 66 -d 87654321
echo testing Australian Post Routing
zint -o bar67.png -b 67 -d 87654321
zint -o bar67.eps -b 67 -d 87654321
echo testing Australian Post Redirection
zint -o bar68.png -b 68 -d 87654321
zint -o bar68.eps -b 68 -d 87654321
echo testing ISBN Code
zint -o bar69.png -b 69 --height=50 -d 0333638514
zint -o bar69.eps -b 69 --height=50 -d 0333638514
echo testing Royal Mail 4 State
zint -o bar70.png -b 70 -d ROYALMAIL
zint -o bar70.eps -b 70 -d ROYALMAIL
echo testing Data Matrix
zint -o bar71.png -b 71 -d 'Data Matrix is a matrix symbology'
zint -o bar71.eps -b 71 -d 'Data Matrix is a matrix symbology'
echo testing EAN-14
zint -o bar72.png -b 72 --height=50 -d 3210987654321
zint -o bar72.eps -b 72 --height=50 -d 3210987654321
echo testing NVE-18
zint -o bar75.png -b 75 --height=50 -d 76543210987654321
zint -o bar75.eps -b 75 --height=50 -d 76543210987654321
echo testing GS1 DataBar Truncated
zint -o bar78.eps -b 29 --height=13 -d 1234567890
zint -o bar78.png -b 29 --height=13 -d 1234567890
echo testing GS1 DataBar Stacked
zint -o bar79.eps -b 79 -d 1234567890
zint -o bar79.png -b 79 -d 1234567890
echo testing GS1 DataBar Stacked Omnidirectional
zint -o bar80.eps -b 80 --height=69 -d 3456789012
zint -o bar80.png -b 80 --height=69 -d 3456789012
echo testing GS1 DataBar Expanded Stacked
zint -o bar81.eps -b 81 -d "[01]98898765432106[3202]012345[15]991231"
zint -o bar81.png -b 81 -d "[01]98898765432106[3202]012345[15]991231"
echo testing Planet 12 Digit
zint -o bar82.png -b 82 -d 10987654321
zint -o bar82.eps -b 82 -d 10987654321
echo testing Planet 14 Digit
zint -o bar83.png -b 82 -d 3210987654321
zint -o bar83.eps -b 82 -d 3210987654321
echo testing Micro PDF417
zint -o bar84.png -b 84 -d 'MicroPDF417 is a very small stacked symbology'
zint -o bar84.eps -b 84 -d 'MicroPDF417 is a very small stacked symbology'
echo testing USPS OneCode 4-State Customer Barcode
zint -o bar85.png -b 85 -d 01234567094987654321
zint -o bar85.eps -b 85 -d 01234567094987654321
echo testing Plessey Code with bidirectional reading support
zint -o bar86.png -b 86 --height=50 -d 87654321
zint -o bar86.eps -b 86 --height=50 -d 87654321
echo testing Telepen Numeric
zint -o bar100.png -b 100 --height=50 -d 87654321
zint -o bar100.eps -b 100 --height=50 -d 87654321
echo testing MSI Plessey with Mod-10 check
zint -o bar101.png -b 101 --height=50 -d 87654321
zint -o bar101.eps -b 101 --height=50 -d 87654321
echo testing MSI Plessey with 2 x Mod-10 checks
zint -o bar102.png -b 102 --height=50 -d 87654321
zint -o bar102.eps -b 102 --height=50 -d 87654321
echo testing MSI Plessey with Mod-11 check
zint -o bar103.png -b 103 --height=50 -d 87654321
zint -o bar103.eps -b 103 --height=50 -d 87654321
echo testing MSI Plessey with Mod-10 and Mod-11 check
zint -o bar104.png -b 104 --height=50 -d 87654321
zint -o bar104.eps -b 104 --height=50 -d 87654321
echo testing Code 39 with Modulo 43 check
zint -o bar105.png -b 105 --height=50 -d 'CODE 39 MOD 43'
zint -o bar105.eps -b 105 --height=50 -d 'CODE 39 MOD 43'
echo testing Extended Code 39 with Modulo 43 check
zint -o bar106.png -b 106 --height=50 -d 'Code 39e MOD 43'
zint -o bar106.eps -b 106 --height=50 -d 'Code 39e MOD 43'
echo testing Aztec Code
zint -o bar107.eps -b 107 -d "Demonstration Aztec Code symbol generated by libzint"
zint -o bar107.png -b 107 -d "Demonstration Aztec Code symbol generated by libzint"
echo testing UPC-E Composite with CC-A
zint -o bar116.eps -b 116 --height=100 --mode=1 --primary=121230 -d "[15]021231"
zint -o bar116.png -b 116 --height=100 --mode=1 --primary=121230 -d "[15]021231"
echo testing UPC-A Composite with CC-A
zint -o bar115.eps -b 115 --height=100 --mode=1 --primary=10987654321 -d "[15]021231"
zint -o bar115.png -b 115 --height=100 --mode=1 --primary=10987654321 -d "[15]021231"
echo testing EAN-8 Composite with CC-A
zint -o bar110.eps -b 110 --height=100 --mode=1 --primary=1234567 -d "[21]A12345678"
zint -o bar110.png -b 110 --height=100 --mode=1 --primary=1234567 -d "[21]A12345678"
echo testing EAN-13 Composite with CC-A
zint -o bar110a.eps -b 110 --height=100 --mode=1 --primary=331234567890 -d "[99]1234-abcd"
zint -o bar110a.png -b 110 --height=100 --mode=1 --primary=331234567890 -d "[99]1234-abcd"
echo testing RSS-14 Stacked Composite with CC-A
zint -o bar117.eps -b 117 --mode=1 --primary=341234567890 -d "[17]010200"
zint -o bar117.png -b 117 --mode=1 --primary=341234567890 -d "[17]010200"
echo testing RSS-14 Stacked Omnidirectional Composite with CC-A
zint -o bar118.eps -b 118 --mode=1 --primary=341234567890 -d "[17]010200"
zint -o bar118.png -b 118 --mode=1 --primary=341234567890 -d "[17]010200"
echo testing RSS Limited Composite with CC-B
zint -o bar113.eps -b 113 --height=100 --mode=2 --primary=351234567890 -d "[21]abcdefghijklmnopqrstuv"
zint -o bar113.png -b 113 --height=100 --mode=2 --primary=351234567890 -d "[21]abcdefghijklmnopqrstuv"
echo testing RSS-14 Composite with CC-A
zint -o bar112.eps -b 112 --height=100 --mode=1 --primary=361234567890 -d "[11]990102"
zint -o bar112.png -b 112 --height=100 --mode=1 --primary=361234567890 -d "[11]990102"
echo testing RSS Expanded Composite with CC-A
zint -o bar114.eps -b 114 --height=100 --mode=1 --primary="[01]93712345678904[3103]001234" -d "[91]1A2B3C4D5E"
zint -o bar114.png -b 114 --height=100 --mode=1 --primary="[01]93712345678904[3103]001234" -d "[91]1A2B3C4D5E"
echo testing RSS Expanded Stacked Composite with CC-A
zint -o bar119.eps -b 119 --height=150 --mode=1 --primary="[01]00012345678905[10]ABCDEF" -d "[21]12345678"
zint -o bar119.png -b 119 --height=150 --mode=1 --primary="[01]00012345678905[10]ABCDEF" -d "[21]12345678"
echo testing UCC/EAN-128 Composite with CC-A
zint -o bar111.eps -b 111 --height=100 --mode=1 --primary="[01]03212345678906" -d "[10]1234567ABCDEFG"
zint -o bar111.png -b 111 --height=100 --mode=1 --primary="[01]03212345678906" -d "[10]1234567ABCDEFG"
echo testing UCC/EAN-128 Composite with CC-C
zint -o bar111a.eps -b 111 --height=100 --mode=3 --primary="[00]030123456789012340" -d "[02]130123456789093724[10]1234567ABCDEFG"
zint -o bar111a.png -b 111 --height=100 --mode=3 --primary="[00]030123456789012340" -d "[02]130123456789093724[10]1234567ABCDEFG"
echo testing KIX Code
zint -o bar120.eps -b 120 -d '1231FZ13Xhs'
zint -o bar120.png -b 120 -d '1231FZ13Xhs'
echo testing Code23
zint -o bar121.eps -b 121 -d "12345678"
zint -o bar121.png -b 121 -d "12345678"
echo testing PNG rotation
zint -o barrot0.png -b 110 --height=50 --mode=1 --rotate=0 --primary=331234567890+01234 -d "[99]1234-abcd"
zint -o barrot90.png -b 110 --height=50 --mode=1 --rotate=90 --primary=331234567890+01234 -d "[99]1234-abcd"
zint -o barrot180.png -b 110 --height=50 --mode=1 --rotate=180 --primary=331234567890+01234 -d "[99]1234-abcd"
zint -o barrot270.png -b 110 --height=50 --mode=1 --rotate=270 --primary=331234567890+01234 -d "[99]1234-abcd"
echo testing Extended ASCII support
zint -o barext.png --height=50 -d "größer"