import java.security.cert.*;
import java.security.*;
import javax.security.auth.x500.*;
import java.math.BigInteger;
import java.io.*;

public class CertInfo
{
	public static void main(String[] args)
	{
		// Parse command line args
		if (args.length < 3) {
			printHelp("Not enough arguments");
			return;
		}
		String cmd = args[0];
		String inFileName = args[1];
		String outFileName1 = args[2];
		String outFileName2 = null;
		if (!cmd.equals("-c") && !cmd.equals("-x")) {
			printHelp("First argument should be -c or -x");
			return;
		}
		if (cmd.equals("-c") && args.length != 3) {
			printHelp("Need 2 arguments after -c");
			return;
		}
		if (cmd.equals("-x")) {
			if (args.length != 4) {
				printHelp("Need 3 arguments after -x");
				return;
			}
			outFileName2 = args[3];
		}

		try {

			if (cmd.equals("-c"))
				generateCertOutputFile(new FileInputStream(inFileName), outFileName1);
			else if (cmd.equals("-x"))
				generateCertOutputFileFromXml(inFileName, outFileName1, outFileName2);
			else {
				printHelp("Unsupported command \"" + cmd + "\"");
				return;
			}

			System.out.println("Success");

		}
		catch (Exception e) {
			System.out.println("ERR: " + e.toString());
			e.printStackTrace();
		}
	}

	private static void printHelp(String msg)
	{
		if (msg != null)
			System.out.println("ERR: " + msg);
		System.out.println("Parameters:");
		System.out.println("  -c <certIn> <certOut>                    : <certIn> = a certificate");
		System.out.println("  -x <xmlIn> <authCertOut> <signCertOut>   : <xmlIn> = an xml input file");
	}

	public static void generateCertOutputFileFromXml(String fileIn, String authOut, String signOut)
	throws Exception
	{
		// Read the file
		ByteArrayOutputStream baos = new ByteArrayOutputStream(25000);
		FileInputStream fis = new FileInputStream(fileIn);
		byte[] buf = new byte[2048];
		int len = fis.read(buf);
		while (len != -1)  {
			baos.write(buf, 0, len);
				len = fis.read(buf);
		}
		String xml = new String(baos.toByteArray());

		// Get the <authCert>
		int startAuth = xml.indexOf("<authCert>");
		if (startAuth == -1)
			throw new Exception("No <authCert> tag found in the input xml file");
		startAuth += "<authCert>".length();
		int endAuth = xml.indexOf("</authCert>", startAuth);
		if (endAuth == -1)
			throw new Exception("No </authCert> tag found in the input xml file");
		String authCert = xml.substring(startAuth, endAuth);

		// Get the <signCert>
		int startSign = xml.indexOf("<signCert>");
		if (startSign == -1)
			throw new Exception("No <signCert> tag found in the input xml file");
		startSign += "<signCert>".length();
		int endSign = xml.indexOf("</signCert>", startSign);
		if (endSign == -1)
			throw new Exception("No </signCert> tag found in the input xml file");
		String signCert = xml.substring(startSign, endSign);

		generateCertOutputFile(toBinaryStream(authCert), authOut);
		generateCertOutputFile(toBinaryStream(signCert), signOut);
		
	}

	private static void printHelp()
	{
		System.out.println("Parameters:");
		System.out.println("   <fileIn1> <fileOut1> {<fileIn2> <fileOut2> {...}}");
	}

	public static void generateCertOutputFile(InputStream is, String fileOut)
	throws Exception
	{
		CertificateFactory cf = CertificateFactory.getInstance("X509");
		X509Certificate cert = (X509Certificate) cf.generateCertificate(is);
		is.close();

		String xml = generateCertOutput(cert);

		PrintStream ps = new PrintStream(new File(fileOut));
		ps.print(xml);
		ps.close();
	}

	public static String generateCertOutput(String certHex)
	throws Exception
	{
		char[] certChars = certHex.toCharArray();
		int certCharsLen = certChars.length;

		byte[] out = new byte[certCharsLen / 2];
		int outLen = 0;
		boolean firstDigit = true;
		for (int i = 0; i < certCharsLen; i++) {
			int hexChar = getHexChar(certChars[i]);
			if (hexChar > -1) {
				if (firstDigit)
					out[outLen] = (byte) (16 * hexChar);
				else
					out[outLen++] += hexChar;
				if (firstDigit)
					firstDigit = false;
				else
					firstDigit = true;			}
		}

		if (firstDigit)
			throw new Exception("Odd number of hex char present in hex encoded cert");

		return generateCertOutput(out, 0, outLen);
	}

	public static String generateCertOutput(byte[] certBytes)
	throws Exception
	{
		return generateCertOutput(certBytes, 0, certBytes.length);
	}

	public static String generateCertOutput(byte[] certBytes, int offset, int len)
	throws Exception
	{
		CertificateFactory cf = CertificateFactory.getInstance("X509");
		X509Certificate cert = (X509Certificate) cf.generateCertificate(
			new ByteArrayInputStream(certBytes, offset, len));

		return generateCertOutput(cert);
	}

	public static String generateCertOutput(X509Certificate cert)
	throws Exception
	{
		byte[] certBytes = cert.getEncoded();

		BigInteger serialNr = cert.getSerialNumber();

		byte[] ext = cert.getExtensionValue("2.5.29.35");
		// ext = 04 18 30 16 80 14 <issuerKeyHash>
		byte[] issuerKeyHash = new byte[20];
		if (ext.length == issuerKeyHash.length + 6 &&
			ext[0] == 0x04 && ext[1] == 0x18 && ext[2] == 0x30 &&
			ext[3] == 0x16 && ext[4] == (byte) 0x80 && ext[5] == 0x14)
		{
			System.arraycopy(ext, 6, issuerKeyHash, 0, issuerKeyHash.length);
		}
		else
			throw new Exception("unknown or no authorityKeyIdentifier");

		X500Principal issuer = cert.getIssuerX500Principal();
		String issuerDN = dn2String(issuer);

		String subjStr = cert.getSubjectX500Principal().toString();
		String subject = getDnValue(subjStr, "CN=") + ", serial=" + serialNr.toString(16);

		MessageDigest md = MessageDigest.getInstance("SHA1");
		byte[] issuerNameHash = md.digest(issuer.getEncoded());

		return
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
			"<certInfo>\n" +
			"\t<contents>" + toHex(certBytes, certBytes.length) + "</contents>\n" +
			"\t<issuerKeyHash>" + toHex(issuerKeyHash, issuerKeyHash.length) + "</issuerKeyHash>\n" +
			"\t<issuerNameHash>" + toHex(issuerNameHash, issuerNameHash.length) + "</issuerNameHash>\n" +
			"\t<serialNumber>" + bigint2Hex(serialNr) + "</serialNumber>\n" +
			"\t<issuerDN>" + issuerDN + "</issuerDN>\n" +
			"\t<subject>" + subject + "</subject>\n" +
			"</certInfo>\n";
	}

	/**
	 * E.g. if dn = "SERIALNUMBER=200510, CN=Citizen CA, C=BE"
	 * and dn = "C=" then return "BE".
	 */
	private static String getDnValue(String dn, String k)
	{
		int idx = dn.indexOf(k);
		if (idx == -1)
			return null;

		idx += k.length();
		int dnLen = dn.length();
		int end = idx;
		while (end < dnLen && dn.charAt(end) != ',')
			end++;

		return dn.substring(idx, end);		
	}

	private static String dn2String(X500Principal issuer)
	{
		// Output of the X500Principal.toString() method:
		// "SERIALNUMBER=200510, CN=Citizen CA, C=BE"
		String str = issuer.toString();

		// What we need (i.e. the format for Apache's SSL_CLIENT_I_DN):
		//   "/C=BE/CN=Citizen CA/serialNumber=200510"

		String ret = "";
		String country = getDnValue(str, "C=");
		if (country != null)
			ret += "/C=" + country;
		String cn = getDnValue(str, "CN=");
			ret += "/CN=" + cn;
		String serialnr = getDnValue(str, "SERIALNUMBER=");
		if (serialnr != null)
			ret += "/serialNumber=" + serialnr;

		return ret;
	}

	private static String getSerialNrFromDn(String dn)
	{
		return ", serial=xx";
	}

	///////////////////////////////////////////////////////////////////////

	private static int getHexChar(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;
		return -1; // no hex char
	}


	private static final char[] HEX_TABLE =
		{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	static private String toHex(byte[] data, int len)
	{
		char out[] = new char[4 * len];

		int j = 0;
		for (int i = 0; i < len; i++) {
			int x = data[i] < 0 ? data[i] + 256 : data[i];
			out[j++] = HEX_TABLE[x / 16];
			out[j++] = HEX_TABLE[x % 16];
			if (i != 0 && i % 40 == 0) {
				out[j++] = '\r';
				out[j++] = '\n';
			}
		}

		return new String(out, 0, j);
	}

	private static InputStream toBinaryStream(String hex)
	throws Exception
	{
		char[] hexChars = hex.toCharArray();
		int hexCharsLen = hexChars.length;
		byte[] out = new byte[hexCharsLen / 2];
		int outLen = 0;
		boolean firstDigit = true;
		for (int i = 0; i < hexCharsLen; i++) {
			int hexChar = getHexChar(hexChars[i]);
			if (hexChar > -1) {
				if (firstDigit)
					out[outLen] = (byte) (16 * hexChar);
				else
					out[outLen++] += hexChar;
				if (firstDigit)
					firstDigit = false;
				else
					firstDigit = true;
			}
		}

		if (!firstDigit)
			throw new Exception("Odd number of hex char present in hex encoded data");

		return new ByteArrayInputStream(out, 0, outLen);
	}

	/**
	 * Convert to String, making sure there's an even number of hex chars
	 */
	private static String bigint2Hex(BigInteger bi)
	{
		String hexStr = bi.toString(16);
		if (hexStr.length() % 2 == 0)
			return hexStr;
		else
			return "0" + hexStr;
	}
}
