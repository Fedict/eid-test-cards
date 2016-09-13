import be.eid.testenv.*;
import java.io.*;
import java.security.cert.*;
import com.cec.util.FileHelper;
import java.security.interfaces.RSAPublicKey;
import org.apache.log4j.xml.DOMConfigurator;
import com.zsc.eid.chipfilegenerator.CAEid;
import com.cec.util.Base64;

public class GenVirtual
{
	public static void usage(String mesg) {

		if (null != mesg)
			System.out.println(mesg);
		System.out.println("Parameters:");
		System.out.println("  <virtual info> <physical info> <out file>");
		System.out.println("In which:");
		System.out.println("  <virtual info>: see base.txt, you should modify this file");
		System.out.println("  <physical info>: the .eid file that you get when saving the");
		System.out.println("                   physical card info with eid-viewer");
	}

	public static void main(String[] args) throws Exception {

		if (args.length != 3)
			usage(null);
		else {
			File virtInfoFile = new File(args[0]);
			File physInfoFile = new File(args[1]);
			String outFile = args[2];

			if (!virtInfoFile.exists()) {
				usage("ERR: file " + virtInfoFile.getAbsolutePath() + " doesn't exist");
				return;
			}
			if (!physInfoFile.exists()) {
				usage("ERR: file " + physInfoFile.getAbsolutePath() + " doesn't exist");
				return;
			}

			String outXml = generate(virtInfoFile, physInfoFile);

			FileHelper.writeFile(outFile, outXml.getBytes("UTF-8"));
		}
	}

	private static String generate(File virtInfoFile, File physInfoFile) throws Exception {

		String virtInfo = readFile(virtInfoFile);

		String[] physInfo = parsePhyicalInfo(physInfoFile);

		String xmlIn = generateInputFile(virtInfo, physInfo);

		xmlIn = fixBoolVal(xmlIn, "authCertPresent");
		xmlIn = fixBoolVal(xmlIn, "signCertPresent");

		BeidVirtualCardFileGenerator generator =
			BeidVirtualCardFileGeneratorFactory.getBeidVirtualCardFileGenerator();
		BeidVirtualCardFiles out = generator.generate(xmlIn);

		byte[] authCert = ((BeidVirtualCardFilesV0100) out).fileDF005038;
		byte[] signCert = ((BeidVirtualCardFilesV0100) out).fileDF005039;

		if (out instanceof be.eid.testenv.BeidVirtualCardFilesV0100) {
			be.eid.testenv.BeidVirtualCardFilesV0100 outV0100 = (be.eid.testenv.BeidVirtualCardFilesV0100) out;
			// Fill up the Address file up to 117 bytes
			outV0100.fileDF014033 = appendZeros(outV0100.fileDF014033, 117);
			// Check if the auth and nonrep certs are empty
			// Add 50 zero bytes to the user and citizen certs
			outV0100.fileDF005038 = (null == authCert) ? emptyCert : appendZeros(authCert, authCert.length + 50);
			outV0100.fileDF005039 = (null == signCert) ? emptyCert : appendZeros(signCert, signCert.length + 50);
			outV0100.fileDF00503A = appendZeros(outV0100.fileDF00503A, outV0100.fileDF00503A.length + 50);
			// For a V1.7 (or up) card, the PrKDF file must be changed (key size from 1024 -> 2048 bits)
			if (outV0100.versionNumber[5] >= 0x17)
				outV0100.fileDF005035 = PrKDF_2048;
		}

		return out.toXml();
	}

	private static final String[] parsePhyicalInfo(File physInfoFile) throws Exception {

		String xml = readFile(physInfoFile);

		// Get the physical chip serialnumber
		String chipNr = parseXml(xml, "chipnumber");

		// Get the Auth pubkey
		String certB64 = parseXml(xml, "authentication");
		String authPubKeyHex = getPubKey(certB64);

		// Get the Sign pubkey
		certB64 = parseXml(xml, "signing");
		String signPubKeyHex = getPubKey(certB64);

		return new String[] {chipNr, authPubKeyHex, signPubKeyHex};
	}

	static private String getPubKey(String certB64) throws Exception
	{
		String certHex = Base64.decodeToHexStr(certB64.replaceAll(" ", ""));
		byte[] certBytes = hex2bin(certHex);

		CertificateFactory cf = CertificateFactory.getInstance("X509");
		java.security.cert.Certificate cert = cf.generateCertificate(
			new ByteArrayInputStream(certBytes));

		RSAPublicKey pubKey = (RSAPublicKey) cert.getPublicKey();
		byte[] mod = pubKey.getModulus().toByteArray();
		int offs = mod[0] == 0 ? 1 : 0; // skip leading 00, if present
		int modLen = mod.length - offs;

		byte[] start;
		if (modLen < 256) {
			start = new byte[] {
				0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
				0x03, (byte) 0x81, (byte) modLen};
		}
		else {
			start = new byte[] {
				0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
				0x03, (byte) 0x82, (byte) (modLen / 256), (byte) (modLen % 256)};			
		}
		byte[] tot = new byte[start.length + modLen];
		System.arraycopy(start, 0, tot, 0, start.length);
		System.arraycopy(mod, offs, tot, start.length, modLen);

		return toHex(tot, tot.length);
	}


	private static String parseXml(String xml, String tag) {

		int idx = xml.indexOf("<" + tag + ">");
		if (-1 != idx) {
			idx += 2 + tag.length();
			int endIdx = xml.indexOf("</" + tag + ">", idx);
			return xml.substring(idx, endIdx);
		}
		else {
			idx = xml.indexOf(tag + "=\"");
			idx += 2 + tag.length();
			int endIdx = xml.indexOf("\"", idx);
			return xml.substring(idx, endIdx);
		}
	}

	private static String readFile(File f) throws Exception {

		byte[] buf = new byte[(int) f.length()];
		FileInputStream fis = new FileInputStream(f);
		fis.read(buf);
		fis.close();

		return new String(buf);
	}

	static public String generateInputFile(String baseFileStr, String[] physInfo) throws Exception {

		String physicalChipNr = physInfo[0];
		String authPubKeyStr =  physInfo[1];
		String signPubKeyStr =  physInfo[2];
		String versionNr = (authPubKeyStr.length() < 500) ?
			"A5 03 01 01 01 11 00 02 00 01 01" :              // V1.1, key size = 128 bytes
			"F3 36 01 22 01 17 00 03 00 21 01";               // V1.7, key size = 256 bytes

		return
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" +
			"<BeidFileInput>\r\n" +
			"<version> V0100 </version>\r\n" +
			"<physicalCardChipNumber>" + physicalChipNr + "</physicalCardChipNumber>\r\n" +
			"<versionNumber>" + versionNr + "</versionNumber>\r\n" +
			"<atr>3B 98 13 40 0A A5 03 01 01 01 AD 13 11</atr>\r\n" +
			baseFileStr +
			"<photo>" + getPhoto(null) + "</photo>\r\n" +
			"<basicPubKey>02 08 00 00 00 00 00 01 00 01 03 81 80 84 2A 3A\r\n" +
			"86 90 15 FB 42 F7 27 FE 51 43 DC 22 FA B8 5C 56\r\n" +
			"D2 F9 69 DC BD 38 4C 6D A1 BE C3 9F 8D 0D 22 ED\r\n" +
			"48 14 C2 E6 9B BF 06 A3 DE 4C D5 3D DF B9 F6 6B\r\n" +
			"09 EE 29 84 3A D7 98 DD 54 13 B5 5D 56 02 71 FA\r\n" +
			"E2 A2 A2 89 97 DF 12 AC 2A 75 03 54 00 85 E3 73\r\n" +
			"CF E0 99 47 3B 42 B0 5B 66 C2 66 E2 98 02 86 5B\r\n" +
			"3B 3F B7 9F 11 AA DB 80 84 EE 86 BA 31 7F 9B CC\r\n" +
			"49 A9 6E A9 AB 89 E7 3F 67 3D 49 09 97</basicPubKey>\r\n" +
			"<authPubKey>" + authPubKeyStr + "</authPubKey>\r\n" +
			"<signPubKey>" + signPubKeyStr + "</signPubKey>\r\n" +
			"</BeidFileInput>";
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	private static final char[] HEX_TABLE =
		{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	static private String toHex(byte[] data, int len)
	{
		char out[] = new char[4 * len];

		int j = 0;
		for (int i = 0; i < len; i++) {
			int x = data[i] < 0 ? data[i] + 256 : data[i];
			out[j++] = HEX_TABLE[x / 16];
			out[j++] = HEX_TABLE[x % 16];
			if (i != 0 && i % 16 == 0) {
				out[j++] = '\r';
				out[j++] = '\n';
			}
			else
				out[j++] = ' ';
		}

		return new String(out, 0, j);
	}

	static byte[] appendZeros(byte[] ba, int fillSize)
	{
		if (ba.length >= fillSize)
			return ba;

		byte[] ret = new byte[fillSize];
		System.arraycopy(ba, 0, ret, 0, ba.length);
		for (int i = ba.length; i < fillSize; i++)
			ret[i] = 0x00;

		return ret;
	}

        private static int nibble(char h) {
                if (h >= '0' && h <= '9')
                        return h - '0';
                if (h >= 'a' && h <= 'f')
                        return h - 'a' + 10;
                if (h >= 'A' && h <= 'F')
                        return h - 'A' + 10;
                throw new RuntimeException("Invalid hex char " + h);
        }
        
        private static byte[] hex2bin(String hex) {
                int len = hex.length() / 2;
                byte[] res = new byte[len];
                for (int i = 0; i < len; i++)
                        res[i] = (byte) (16 * nibble(hex.charAt(2 * i)) + nibble(hex.charAt(2 * i + 1)));
                return res;
        }

	////////////////////////////////////////////////////////////////////////////////////////////////

	private static final byte[] emptyCert = new byte[1300];

	private static final String emptyCertXml =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" +
		"<certInfo>\r\n" +
		"\t<contents>00</contents>\r\n" +
		"\t<issuerKeyHash>0000000000000000000000000000000000000000</issuerKeyHash>\r\n" +
		"\t<issuerNameHash>0000000000000000000000000000000000000000</issuerNameHash>\r\n" +
		"\t<serialNumber>00000000000000000000000000000000</serialNumber>\r\n" +
		"\t<issuerDN>/C=BE/CN=no such CA</issuerDN>\r\n" +
		"\t<subject>THIS IS NO CERT</subject>\r\n" +
		"</certInfo>\r\n";

	// Parsing something like
	//  <authCertPresent>false</authCertPresent>
	// always results in authCertPresent being false for some reason.
	// The work-around is to remove this xml element unless it
	// has the value 'false' or 'FALSE'
	private static String fixBoolVal(String xmlIn, String certPresent) {
		int idx1 = xmlIn.indexOf("<" + certPresent + ">");
		int idx2 = xmlIn.indexOf("</" + certPresent + ">");
		if (-1 == idx1 || -1 == idx2)
			return xmlIn;
		else {
			String val = xmlIn.substring(idx1 + 2 + certPresent.length(), idx2).toLowerCase();
			if ("false".equals(val))
				return xmlIn.substring(0, idx1 + 2 + certPresent.length()) + "false" + xmlIn.substring(idx2);
			else
				return xmlIn.substring(0, idx1) + xmlIn.substring(idx2 + 3 + certPresent.length());
		}
	}
	                                                        
	static private byte[] PrKDF_2048 = {
		(byte)0x30,(byte)0x3a,(byte)0x30,(byte)0x17,(byte)0x0c,(byte)0x0e,(byte)0x41,(byte)0x75,(byte)0x74,(byte)0x68,(byte)0x65,(byte)0x6e,(byte)0x74,(byte)0x69,(byte)0x63,(byte)0x61,
		(byte)0x74,(byte)0x69,(byte)0x6f,(byte)0x6e,(byte)0x03,(byte)0x02,(byte)0x06,(byte)0xc0,(byte)0x04,(byte)0x01,(byte)0x01,(byte)0x30,(byte)0x0f,(byte)0x04,(byte)0x01,(byte)0x02,
		(byte)0x03,(byte)0x02,(byte)0x05,(byte)0x20,(byte)0x03,(byte)0x02,(byte)0x03,(byte)0xb8,(byte)0x02,(byte)0x02,(byte)0x00,(byte)0x82,(byte)0xa1,(byte)0x0e,(byte)0x30,(byte)0x0c,
		(byte)0x30,(byte)0x06,(byte)0x04,(byte)0x04,(byte)0x3f,(byte)0x00,(byte)0xdf,(byte)0x00,(byte)0x02,(byte)0x02,(byte)0x08,(byte)0x00,(byte)0x30,(byte)0x39,(byte)0x30,(byte)0x15,
		(byte)0x0c,(byte)0x09,(byte)0x53,(byte)0x69,(byte)0x67,(byte)0x6e,(byte)0x61,(byte)0x74,(byte)0x75,(byte)0x72,(byte)0x65,(byte)0x03,(byte)0x02,(byte)0x06,(byte)0xc0,(byte)0x04,
		(byte)0x01,(byte)0x01,(byte)0x02,(byte)0x01,(byte)0x01,(byte)0x30,(byte)0x10,(byte)0x04,(byte)0x01,(byte)0x03,(byte)0x03,(byte)0x03,(byte)0x06,(byte)0x00,(byte)0x40,(byte)0x03,
		(byte)0x02,(byte)0x03,(byte)0xb8,(byte)0x02,(byte)0x02,(byte)0x00,(byte)0x83,(byte)0xa1,(byte)0x0e,(byte)0x30,(byte)0x0c,(byte)0x30,(byte)0x06,(byte)0x04,(byte)0x04,(byte)0x3f,
		(byte)0x00,(byte)0xdf,(byte)0x00,(byte)0x02,(byte)0x02,(byte)0x08,(byte)0x00,
	};

	static private String getPhoto(String fileName) throws Exception
	{
		if (fileName != null) {
			return "not impl";
		}
		else  {
			return
				"ff d8 ff e0 00 10 4a 46 49 46 00 01 02 01 01 2c\r\n" +
				"01 2c 00 00 ff db 00 43 00 07 05 06 06 06 05 07\r\n" +
				"06 06 06 08 08 07 09 0b 12 0c 0b 0a 0a 0b 17 10\r\n" +
				"11 0d 12 1b 17 1c 1c 1a 17 1a 19 1d 21 2a 24 1d\r\n" +
				"1f 28 20 19 1a 25 32 25 28 2c 2d 2f 30 2f 1d 23\r\n" +
				"34 38 34 2e 37 2a 2e 2f 2e ff c0 00 0b 08 00 c8\r\n" +
				"00 8c 01 01 11 00 ff c4 00 d2 00 00 01 05 01 01\r\n" +
				"01 01 01 01 00 00 00 00 00 00 00 00 01 02 03 04\r\n" +
				"05 06 07 08 09 0a 0b 10 00 02 01 03 03 02 04 03\r\n" +
				"05 05 04 04 00 00 01 7d 01 02 03 00 04 11 05 12\r\n" +
				"21 31 41 06 13 51 61 07 22 71 14 32 81 91 a1 08\r\n" +
				"23 42 b1 c1 15 52 d1 f0 24 33 62 72 82 09 0a 16\r\n" +
				"17 18 19 1a 25 26 27 28 29 2a 34 35 36 37 38 39\r\n" +
				"3a 43 44 45 46 47 48 49 4a 53 54 55 56 57 58 59\r\n" +
				"5a 63 64 65 66 67 68 69 6a 73 74 75 76 77 78 79\r\n" +
				"7a 83 84 85 86 87 88 89 8a 92 93 94 95 96 97 98\r\n" +
				"99 9a a2 a3 a4 a5 a6 a7 a8 a9 aa b2 b3 b4 b5 b6\r\n" +
				"b7 b8 b9 ba c2 c3 c4 c5 c6 c7 c8 c9 ca d2 d3 d4\r\n" +
				"d5 d6 d7 d8 d9 da e1 e2 e3 e4 e5 e6 e7 e8 e9 ea\r\n" +
				"f1 f2 f3 f4 f5 f6 f7 f8 f9 fa ff da 00 08 01 01\r\n" +
				"00 00 3f 00 f5 9a 28 a2 8a 28 a3 da 97 07 19 c7\r\n" +
				"1e b4 60 fa 52 77 c7 7a 0f 1d 78 a2 8a 28 a2 8a\r\n" +
				"28 a2 8a 28 ab 96 56 32 dc b0 3b 4a c7 9e 5f fc\r\n" +
				"3d 6b 66 0d 36 d6 2e 76 b3 9f 57 39 ab a0 05 18\r\n" +
				"51 81 e8 28 20 1e a0 1f a8 a6 79 30 e4 93 12 12\r\n" +
				"7d 54 55 7b 8d 3e d6 71 83 1e c3 ea 9c 56 6d ce\r\n" +
				"91 2a 9c db 9f 30 7a 12 01 ac c7 56 47 64 60 43\r\n" +
				"29 c1 07 a8 a6 d1 45 14 51 45 14 e8 d1 a4 70 88\r\n" +
				"a5 98 f4 02 b6 ec b4 a5 43 be e7 0c dd 90 1e 07\r\n" +
				"f8 d6 a8 50 a0 00 00 03 b0 a2 8a 28 a2 8a cd d6\r\n" +
				"2c 1a e5 04 d0 85 f3 d3 d7 f8 87 a5 73 ca c1 86\r\n" +
				"45 2d 14 51 45 14 77 03 d7 8a ea 34 eb 74 b7 b6\r\n" +
				"8f 08 03 b2 82 cc 47 27 3c e2 ad 51 45 14 51 45\r\n" +
				"14 57 3f ae da f9 32 2d d4 60 04 90 85 70 07 7e\r\n" +
				"c7 f4 ac da 28 a2 8a 2a ee 95 6e b7 17 43 78 3b\r\n" +
				"10 6e 3f d2 ba 5a 28 a2 8a 28 a2 8a 2a 2b a8 56\r\n" +
				"e2 da 48 5b 1f 3a 90 09 ec 7d 6b 8e 8d 5d 09 8a\r\n" +
				"45 da c8 70 41 a9 28 a2 8a 2b a4 d2 6d d6 1b 45\r\n" +
				"7d b8 92 4e 5b fa 7e 95 7a 8a 28 a2 8a 28 a2 8a\r\n" +
				"2b 9c f1 0c 5e 55 dc 57 3c 00 e3 07 ea 3b fe 44\r\n" +
				"55 03 ed 45 14 53 64 38 46 23 d2 ba db 16 dd 67\r\n" +
				"01 ff 00 a6 6b fc aa 7a 28 a2 8a 28 a2 8a 28 ae\r\n" +
				"6f c5 32 0f 3a d6 2e 72 15 9b f3 c7 f8 55 01 d0\r\n" +
				"52 d1 45 23 72 a4 57 4b a2 1c e9 56 ff 00 43 fc\r\n" +
				"cd 5e a2 8a 28 a2 8a 28 a2 8a e5 7c 4b ce ab 0a\r\n" +
				"ff 00 d3 01 ff 00 a1 1a ae 38 02 8a 28 a4 6f ba\r\n" +
				"6b a7 d1 d1 e3 d3 20 49 14 ab 00 72 0f 51 c9 ab\r\n" +
				"94 51 45 14 51 45 14 51 5c 9f 88 19 5f 58 50 9c\r\n" +
				"84 89 54 fd 72 4f f5 a8 9b ad 25 15 35 97 94 6f\r\n" +
				"20 59 81 28 cd 8c 76 cf 6f d7 15 d3 b5 bc 0e c1\r\n" +
				"de 18 cb 0e 84 a8 c8 a9 68 a2 8a 28 a2 8a 28 a2\r\n" +
				"8a c3 d5 34 a9 64 b8 96 f2 27 0d d0 f9 78 e7 81\r\n" +
				"8f c6 b2 73 9e c4 76 c1 eb 45 44 26 5e e3 06 9a\r\n" +
				"66 02 78 e5 03 ee 10 6b b9 53 90 0f ad 14 51 45\r\n" +
				"14 51 45 14 51 45 2d 71 da d4 85 75 8b 94 5c 00\r\n" +
				"36 f4 ff 00 74 66 a0 13 2f a1 1f 5a ad 49 27 dc\r\n" +
				"61 ea 2b b7 d3 26 fb 45 85 bc bc 64 a0 07 1e a3\r\n" +
				"83 fa 8a b5 45 14 51 45 14 51 45 14 52 33 2a 29\r\n" +
				"77 60 a8 a3 24 93 80 05 79 da cc 6e 2e 65 b8 3d\r\n" +
				"65 66 7f a6 4d 4d 83 55 3c d9 3f bd fa 52 34 8e\r\n" +
				"c3 05 89 ae cf c2 f2 ac 9a 52 a0 ff 00 96 4e ca\r\n" +
				"78 ef 9c ff 00 5a d8 a2 8a 28 a2 8a 28 a2 8a 2b\r\n" +
				"1f c5 57 02 df 44 9f e7 da d2 e2 25 18 eb 93 c8\r\n" +
				"ff 00 be 41 ae 1e dd 88 8d 48 38 3c d4 e2 69 07\r\n" +
				"46 fd 2a b7 9c 9e 86 8f 3d 71 c0 39 ad 4d 07 5d\r\n" +
				"4d 30 ca 97 11 33 41 23 6e ca 75 53 8c 74 27 a7\r\n" +
				"02 bb 7b 2b bb 7b db 64 b9 b5 93 cc 89 fa 36 31\r\n" +
				"d3 ad 4f 45 14 51 45 14 51 45 61 6a fe 28 d3 34\r\n" +
				"ab a6 b5 b8 f3 da 65 00 95 8e 3c f5 19 f5 ae 5b\r\n" +
				"c4 3a e2 6b 12 da ad b2 4d 1c 31 65 99 64 00 12\r\n" +
				"c4 63 a0 27 a0 fe 66 a8 c7 2a aa 05 20 e4 7a 53\r\n" +
				"bc e4 f4 35 53 7a 7f 7c 7e 74 6f 4f ef 0f ce 91\r\n" +
				"8c 6c 30 59 79 f7 ad 6f 08 6a c2 c2 ff 00 ec d7\r\n" +
				"12 62 da e0 85 1b 8f 0a fd 8f e3 d3 f2 af 48 a2\r\n" +
				"8a 28 a2 8a 28 a5 af 27 f1 1c 86 5f 11 6a 0e e0\r\n" +
				"8c 49 b3 df 00 63 fa 54 0b e5 a7 00 8e 7b e7 ad\r\n" +
				"29 74 c1 3b 87 e7 46 f4 fe f0 fc ea 98 e9 eb 47\r\n" +
				"7c 51 8e 7a 73 4c 9b 06 36 04 e0 81 9e b5 ea 3e\r\n" +
				"10 d4 46 a3 a1 5b b9 6c cb 10 f2 a4 cf 5c 81 d7\r\n" +
				"f1 18 3f 8d 6d d1 45 14 51 45 14 63 3d f1 5e 25\r\n" +
				"25 f9 be d4 2e 2e 0e 31 34 ac c3 1f 53 52 70 7d\r\n" +
				"28 e3 19 c0 fa d2 e2 b3 03 30 18 0c 40 f6 34 bb\r\n" +
				"df 39 dc 73 d3 39 a4 24 b7 24 93 f5 a4 ce d0 49\r\n" +
				"21 57 1c 93 d3 15 7b e1 ef 8f 7c 3f 6b e2 a3 a0\r\n" +
				"b6 a2 0c d7 ac b0 2a 88 dc 8f 34 67 68 dc 06 3b\r\n" +
				"91 d7 b8 af 6f a4 a2 8a 28 a2 8a e2 be 28 78 be\r\n" +
				"c7 c2 5a 0a f9 f7 3e 4d dd f9 30 5b 00 09 39 fe\r\n" +
				"26 e0 1e 80 8f 4e 48 af 32 b5 3f bb c8 3c e7 38\r\n" +
				"a9 b2 4f 5a 50 cc 06 01 38 f4 cd 1b 8f 7e 7e b5\r\n" +
				"9d 3d ea db c6 64 9a 78 e3 40 09 25 c8 1f ce b9\r\n" +
				"5d 4f e2 06 91 66 a4 5a bc b7 93 e0 e1 11 4a ae\r\n" +
				"7d d8 ff 00 30 0d 73 57 ff 00 12 f5 89 c6 cb 0b\r\n" +
				"4b 7b 41 8f be c3 cd 6c fe 38 1f a5 72 1a 86 ab\r\n" +
				"aa 6a 00 8b db fb 9b 90 cf bb 6b c8 4a 83 ec bd\r\n" +
				"05 6b fc 3d 87 cf f1 f7 86 63 f3 3c b3 fd a7 6e\r\n" +
				"43 63 38 3e 62 e3 f5 c5 7d e9 49 45 14 51 45 15\r\n" +
				"f3 df ed 5f 8f b1 f8 63 8c 9f 32 e4 7e 91 d7 9a\r\n" +
				"78 03 c5 8e 5a 3d 23 52 99 cb 92 44 33 c8 f9 dd\r\n" +
				"fe c1 cf e3 83 f8 57 a3 89 a4 18 f9 a8 13 48 3b\r\n" +
				"d1 e7 3f ad 7c e0 c0 bb 17 77 66 62 72 49 3c d2\r\n" +
				"81 81 8c d2 d1 5e 8f f0 0e cb ed bf 13 f4 90 46\r\n" +
				"56 01 24 cd c7 4d a8 48 fd 71 5f 67 51 45 14 51\r\n" +
				"45 15 f3 c7 ed 5c ea 20 f0 b2 1c ee 67 ba 23 e8\r\n" +
				"04 59 fe 62 be 70 39 c8 2a 48 20 f0 45 76 fe 1f\r\n" +
				"f1 dd cd a7 97 6b aa 27 9f 10 18 f3 c1 25 c0 f5\r\n" +
				"23 bf 6f 4f c6 bb eb 0d 6e cb 50 84 4b 67 2c 72\r\n" +
				"af 7d af ca fd 47 51 f8 d5 af b5 ff 00 d3 3f fc\r\n" +
				"7a bc 06 8a 28 af 6c fd 97 2c d6 6f 1b 6a 57 8c\r\n" +
				"dc db e9 ec 14 67 bb 3a 73 f9 03 f9 d7 d5 34 51\r\n" +
				"45 14 51 45 7c d9 fb 59 dc c4 d7 5e 14 b4 0d fb\r\n" +
				"e8 a3 b9 91 87 a2 b1 8c 0f fd 01 bf 2a f9 f6 90\r\n" +
				"80 7a d5 8b 2b db bd 3e 71 71 63 33 45 2e 30 48\r\n" +
				"ee 3d 08 3d 6b aa b4 f1 dc f1 db a2 5e 5a ac b3\r\n" +
				"0e ae a7 6e ef c2 b8 ea 28 a2 b6 3c 2d e2 4d 5b\r\n" +
				"c2 ba cd be ad a3 cc 63 b8 8c 8d ca 49 db 22 e4\r\n" +
				"12 8d 82 32 a7 1c 8a fb b3 c3 da ce 9f e2 1d 16\r\n" +
				"cf 5a d2 e6 f3 6c ee e3 12 21 c8 ca fa ab 60 9c\r\n" +
				"30 39 04 76 20 d6 8d 14 51 45 15 1d c4 d1 5b 5b\r\n" +
				"cb 71 3c a9 14 31 21 79 24 91 82 aa 28 19 24 93\r\n" +
				"c0 00 77 35 f0 ff 00 c5 6f 19 3f 8d 7c 65 75 a9\r\n" +
				"21 06 c2 11 f6 6b 21 8d bf b9 56 24 31 07 07 2c\r\n" +
				"49 6e 79 e4 0e d5 c7 51 45 26 01 ea 05 2d 14 51\r\n" +
				"45 7b 2f ec fb f1 0b 4f f0 ad e5 de 83 ad dc bc\r\n" +
				"36 1a 84 a8 f0 cc c7 f7 70 4b 82 0e ef 40 c3 60\r\n" +
				"cf 6d a3 3c 72 3e ae 8d d2 58 d2 48 9d 5e 37 50\r\n" +
				"ca c8 72 18 1e 84 1e e2 9d 45 14 52 8e 7a 57 84\r\n" +
				"fe d1 fe 37 b3 b6 f0 f8 f0 9e 97 7f 1c 97 f7 92\r\n" +
				"e2 f4 41 22 b1 86 24 e4 a3 e0 e5 59 9b 6f 18 e4\r\n" +
				"06 cf bf cb c0 60 01 4b 45 14 51 45 14 51 4d 23\r\n" +
				"83 cf 26 bd 1b e1 8f c5 5d 77 c0 c1 74 f3 12 5f\r\n" +
				"68 8f 2e f9 2d 9c e1 e3 cf 53 1b 67 e5 e7 9c 10\r\n" +
				"41 f6 c9 35 ef da 67 c7 4f 87 b7 96 e2 5b bd 46\r\n" +
				"eb 4e 93 bc 57 36 8e c7 f3 8c 30 fd 6b a6 b1 f8\r\n" +
				"8f e0 3b eb 41 77 0f 8b 74 95 8c e7 e5 9e e5 61\r\n" +
				"7f fb e1 f0 df a5 65 4b f1 93 e1 ac 52 3c 6d e2\r\n" +
				"88 b2 87 04 ad ac ec 3f 02 13 07 f0 ae 67 c5 bf\r\n" +
				"1f 7c 2f a7 da 3a 78 6d 66 d5 ef 5d 0f 96 e6 16\r\n" +
				"8a 14 6c 71 bb 76 d6 3c e0 e0 0e 47 71 5e 23 e2\r\n" +
				"df 8b 7e 37 f1 44 6d 6d 75 7d 1d 85 93 80 1a de\r\n" +
				"c1 3c b5 6c 7a b1 25 f9 f4 dd 8f 6a e0 0f 2c 58\r\n" +
				"92 58 f5 24 e7 34 51 45 14 51 45 14 51 45 14 84\r\n" +
				"03 d6 90 8c 8d bd a8 da 30 07 5c 53 a8 a2 8a 28\r\n" +
				"a2 8a e8 ff 00 e1 06 f1 b7 fd 09 da ff 00 fe 0b\r\n" +
				"66 ff 00 e2 68 ff 00 84 1b c6 df f4 27 6b ff 00\r\n" +
				"f8 2d 9b ff 00 89 a3 fe 10 6f 1b 7f d0 9d af ff\r\n" +
				"00 e0 b6 6f fe 26 8f f8 41 bc 6d ff 00 42 76 bf\r\n" +
				"ff 00 82 d9 bf f8 9a 3f e1 06 f1 b7 fd 09 da ff\r\n" +
				"00 fe 0b 66 ff 00 e2 68 ff 00 84 1b c6 df f4 27\r\n" +
				"6b ff 00 f8 2d 9b ff 00 89 a3 fe 10 6f 1b 7f d0\r\n" +
				"9d af ff 00 e0 b6 6f fe 26 8f f8 41 bc 6d ff 00\r\n" +
				"42 76 bf ff 00 82 d9 bf f8 9a 3f e1 06 f1 b7 fd\r\n" +
				"09 da ff 00 fe 0b 66 ff 00 e2 68 ff 00 84 1b c6\r\n" +
				"df f4 27 6b ff 00 f8 2d 9b ff 00 89 a3 fe 10 6f\r\n" +
				"1b 7f d0 9d af ff 00 e0 b6 6f fe 26 8f f8 41 bc\r\n" +
				"6d ff 00 42 76 bf ff 00 82 d9 bf f8 9a 3f e1 06\r\n" +
				"f1 b7 fd 09 da ff 00 fe 0b 66 ff 00 e2 68 ff 00\r\n" +
				"84 1b c6 df f4 27 6b ff 00 f8 2d 9b ff 00 89 a3\r\n" +
				"fe 10 6f 1b 7f d0 9d af ff 00 e0 b6 6f fe 26 8f\r\n" +
				"f8 41 bc 6d ff 00 42 76 bf ff 00 82 d9 bf f8 9a\r\n" +
				"3f e1 06 f1 b7 fd 09 da ff 00 fe 0b 66 ff 00 e2\r\n" +
				"68 ff 00 84 1b c6 df f4 27 6b ff 00 f8 2d 9b ff\r\n" +
				"00 89 af ff d9";
		}
	}
}
