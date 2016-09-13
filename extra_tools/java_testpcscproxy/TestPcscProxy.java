/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2016 FedICT.
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.

**************************************************************************** */
import java.io.File;
import java.io.FileInputStream;
import java.util.List;
import java.util.Map;
import javax.smartcardio.*;

/**
 * Command line program to find out why the PCSC proxy doesn't work with your (Java) program.
 * Author: Zetes
 * Requires Java 1.6 or higher to build/run
 */
public class TestPcscProxy
{
	static String javaHome = null;
	static String binDirPath = null;
	static String bittyNess = null;

	public static void main(String[] args) {
		try {
			System.out.println("\nThis command line program tries to find out why the PCSC proxy doesn't work with your (Java) program");

      // 1. Get some system/java info

			System.out.println("\nFirst some info:");
			javaHome = System.getProperty("java.home");
			bittyNess = System.getProperty("sun.arch.data.model");
			System.out.println("  java.home: " + javaHome);
			System.out.println("  The Java VM is " + bittyNess + " bit (the sun.arch.data.model property)");

      // 2. Check for the PCSC proxy

      checkPcscProxy();

      // 3. Check for the config file

      checkConfigFile();

      // 4. See if we can connect to a virtual card and use it

      useVirtualCard();

      System.out.println();
		}
		catch (Throwable t) {
			t.printStackTrace();
		}
	}

  /** Check if the winscard.dll or libpcsclite.so exists ans looks OK */
  private static void checkPcscProxy() throws Exception {
		if ('\\' == File.separatorChar) {
		  // For Windows
			System.out.println("\nNow see if the proxy winscard.dll is present:");
			File binDir = new File(javaHome, "bin");
			binDirPath = binDir.getAbsolutePath();
			String[] files = binDir.list();
			boolean bProxyFound = false;
			for (String f: files) {
				if (f.equalsIgnoreCase("winscard.dll")) {
					System.out.println("  Found a winscard.dll in " + binDirPath + " => OK");
					bProxyFound = true;
					checkProxyWinscardDll(binDirPath + File.separator + "winscard.dll");
					break;
				}
			}
			if (!bProxyFound) {
				System.out.println("  No proxy winscard.dll found in " + binDirPath + " => NOK");
				System.out.println("You should copy C:\\Program Files\\Eid Test Infrastructure\\proxy" +
					bittyNess + "\\winscard.dll to " + binDirPath);
			}
		}
		else {
			System.out.println("\nNow see if the proxylibpcsclite.so should be loaded:");
		  // For Linux -- TODO: Mac OS X
		  String pcvscProxyDir = "/usr/share/eidtestinfra";
		  String pcscProxyPath = "/usr/share/eidtestinfra/libpcsclite.so";
		  File f = new File(pcscProxyPath);
		  if (!f.exists()) {
		    System.out.println("  No " + pcscProxyPath + " file exists => NOK");
		    System.out.println("You should install the software first");
		  }
		  else {
		    System.out.println("  " + pcscProxyPath + " exists => OK");
		    System.out.println("    (but no 32/64 bit check is done)");
		  }
		  String ldLibPath = System.getenv("LD_LIBRARY_PATH");
		  if (-1 == ldLibPath.indexOf(pcvscProxyDir)) {
		    System.out.println("  The LD_LIBRARY_PATH env. variable doesn't contain " + pcvscProxyDir + " => NOK");
		    System.out.println("You should 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:" + pcvscProxyDir + "'");
		  }
		  else
  		  System.out.println("  The LD_LIBRARY_PATH env. variable contains " + pcscProxyPath + " => OK");
		  String ldPreload = System.getenv("LD_PRELOAD");
		  if (null == ldPreload || -1 == ldPreload.indexOf(pcscProxyPath) || -1 == ldPreload.indexOf("/usr/share/eidtestinfra/libdl_proxy.so")) {
		    System.out.println("  The LD_PRELOAD env. variable isn't set to circumvent dynamic loading of the PCSC (proxy) lib");
		    System.out.println("This is needed for Java programs on Linux (like this one) -- and perhaps some others");
		  }
		  else {
		    System.out.println("  The LD_PRELOAD env. variable is set to circumvent dynamic loading of the PCSC (proxy lib)");
		    System.out.println("  This is needed for Java programs on Linux (like this one) -- and perhaps some others");
		  }
		}
  }

	/** Check if the proxy winscard.dll that we found is OK */
	private static void checkProxyWinscardDll(String proxyPath) throws Exception {
		// See if it's the proxy winscard.dll
		File f = new File(proxyPath);
		byte[] conts = readFile(f);
		if (contains(conts, "virtual card".getBytes())) {
			System.out.println("  .. and it seems to be the proxy winscard.dll => OK");

			// Check if it's the same bitty-ness as the JVM
			if (contains(conts, "Architecture=\"x86\"".getBytes())) {
				if ("32".equals(bittyNess))
					System.out.println("  .. and it's the 32 bit proxy (= same as the JVM) => OK");
				else {
					System.out.println("  .. but it's the 32 bit proxy while the JVM is 64 bit => NOK");
					System.out.println("You should copy C:\\Program Files\\Eid Test Infrastructure\\proxy64\\winscard.dll to " + binDirPath);
				}
			}
			else if (contains(conts, "Architecture=\"amd64\"".getBytes())) {
				if ("64".equals(bittyNess))
					System.out.println("  .. and it's the 64 bit proxy (= same as the JVM) => OK");
				else {
					System.out.println("  .. but it's the 64 bit proxy while the JVM is 32 bit => NOK");
					System.out.println("You should copy C:\\Program Files\\Eid Test Infrastructure\\proxy32\\winscard.dll");
					System.out.println("or C:\\Program Files(x86)\\Eid Test Infrastructure\\proxy32\\winscard.dll to " + binDirPath);
				}
			}
			else
				System.out.println("  Hm, the proxy winscard.dll couldn't be recognized (perhaps it's more recent then this tool)");
		}
		else {
			System.out.println("  .. but it doesn't seem to be the proxy winscard.dll => NOK");
			System.out.println("You should replace the winscard.dll by the proxy winscard.dll " +
				"that you can find in C:\\Program Files\\Eid Test Infrastructure\\proxy" + bittyNess);
		}
	}

  /** Check if the config file exists and looks OK */
  private static void checkConfigFile() throws Exception {
	  String configDirStr = ('\\' == File.separatorChar) ?
	    System.getenv("ALLUSERSPROFILE") : "/usr/share";
	  if (null != configDirStr) {
	    System.out.println("\nNow see if the config file is OK");
		  File configDir = new File(configDirStr);
		  File configFile = new File(configDir, "BeidTestCtrl.xml");
		  String configFilePath = configFile.getAbsolutePath();
		  if (!configFile.exists()) {
			  System.out.println("  Config file " + configFilePath + " doesn't exist => NOK");
			  System.out.println("You should install it first");
		  }
		  else {
		    System.out.println("  Config file " + configFilePath + " exists => OK");
			  String conts = new String(readFile(configFile));
			  int startIdx = conts.indexOf("<show>");
			  if (-1 == startIdx) {
			    System.out.println("  No <show> element found in the config file => NOK");
			    System.out.println("You should run the PCSC control tool first");
			  }
			  else {
			    startIdx += "<show>".length();
			    int endIdx = conts.indexOf("</show>");
			    String show = conts.substring(startIdx, endIdx).trim();
			    System.out.println("  Reader visibility = " + show);
			    if ("HIDE_REAL".equals(show) || "REAL_LAST".equals(show))
			      System.out.println("    this is OK");
			    else if ("REAL_LAST".equals(show)) {
			      System.out.println("    some progs (like eid-viewer) only use the 1st reader with a card");
			      System.out.println("    so perhaps you should change this to HIDE_REAL or REAL_LAST");
			    }
			    else if ("HIDE_VIRTUAL".equals(show)) {
			      System.out.println("    this means that your virtual reader/card won't be shown => NOK");
			      System.out.println("You should change this to HIDE_REAL or REAL_LAST");
			    }
			    else {
			      System.out.println("    this is an unknown value, something is wrong with your config file => NOK");
			      System.out.println("You should run the PCSC control tool");
			    }
			  }
		  }
	  }
  }

  /** See if we find a virtual reader, connect to it and send a command to the virtual card in it */
  private static void useVirtualCard() throws Exception {
		System.out.println("\nNow see if the proxy winscard.dll is loaded by the JVM");
		TerminalFactory tf = TerminalFactory.getDefault();
		CardTerminals terminals = tf.terminals();
		List<CardTerminal> list = null;
		try {
			list = terminals.list();
		}
		catch (CardException e) {
			System.out.println("Couldn't list the card readers (" + e + ") : perhaps no reader attached to your PC??");
			e.printStackTrace();
			return;
		}
		boolean bFirstReaderIsVirtual = false;
		boolean bFoundVirtualReader = false;
		Card virtualCard = null;
		System.out.println("  Reader list:");
		for (int i = 0; i < list.size(); i++) {
		  String readerName = list.get(i).getName();
			System.out.println("   - " + readerName);
		  if (readerName.startsWith("!Virtual ")) {
		    if (0 == i)
		      bFirstReaderIsVirtual = true;
		    bFoundVirtualReader = true;
		    if (null == virtualCard) {
		      try {
		        virtualCard = list.get(i).connect("*");
		      }
		      catch (CardException e) {
		      }
		    }
		  }
		}
		if (!bFoundVirtualReader) {
		  System.out.println("  Didn't find a virtual reader => NOK");
		  System.out.println("Check the previous messages to see what's not OK there and try to fix it");
		  if ('\\' == File.separatorChar) { // On Windows, there may be a specific issue
  		  System.out.println("If all previous messages look fine and you started java with\n" +
  		    "  java ...\n" +
    		  "then you should instead try to start it with\n" +
    		  "  \"" + binDirPath + "\\java\" ...\n" +
    		  "That should fix the problem.");
    	}
		}
		else {
			// We found a virtual reader
			System.out.println("  Found a virtual reader => OK");
			if (bFirstReaderIsVirtual)
			  System.out.println("    and it's the first reader found => OK");
			else
			  System.out.println("    it's not the first reader found, which may be a problem");
			if (null == virtualCard) {
			  System.out.println("    but no card was found in (any of) the virtual reader(s)");
			  System.out.println("You should insert a card and run this test again");
			}
			else {
			  // There's a virtual card present
			  System.out.println("    and a virtual card was found => OK");
			  byte[] atr = virtualCard.getATR().getBytes();
			  if ((byte) 0xa5 == atr[5] && 0x03 == atr[6] && 0x01 == atr[7] && 0x01 == atr[8]){
			    System.out.println("    and it looks like a BE eID (test) card => OK");
			    // It's a BE eID (test) card, see if we can send a command to it
			    checkSendCommand(virtualCard);
   			}
			  else {
			    System.out.println("    but it doesn't look like you inserted a BE eID (test) card => NOK");
			    System.out.println("You should insert a test BE eID test card");
			  }
			}
		}
	}

  /** Used by useVirtualCard() */
  private static void checkSendCommand(Card virtualCard) throws Exception {
    CardChannel channel = virtualCard.getBasicChannel();
    byte[] cmd = new byte[] {0x00, (byte) 0xa4, 0x08, 0x0C, 0x06, 0x3F, 0x00, (byte) 0xdf, 0x01, 0x40, 0x31};
    ResponseAPDU resp = channel.transmit(new CommandAPDU(cmd));
    int sw = resp.getSW();
    System.out.println("  Response of a 'select ID file' command: SW = " + sw +
      ((0x6d00 == sw) ? " = 0x6d00" : (0x9000 == sw ? " = 0x9000" : "")));
    if (0x9000 == sw) {
      System.out.println("    which is OK");
      System.out.println("\nEverything looks fine. If you (java) program doesn't see\n" +
        "the virtual card, perhaps try to run it from this command line.\n" +
        "If that does work, then perhaps you're using another Java VM?");
    }
    else if (0x6d00 == sw) {
      System.out.println("    which means there was no XML file ('virtual card') selected for this test card => NOK");
      System.out.println("You should run the PCSC control tool and select a virtual card for this test card");
    }
    else {
      System.out.println("    which is an unexpected result (weird..) => NOK");
      System.out.println("Perhaps you should remove and re-insert your card and try again?");
    }
  }

	/** Read a file from disk */
	private static byte[] readFile(File f) throws Exception {
		byte[] ret = new byte[(int) f.length()];
		(new FileInputStream(f)).read(ret);
		return ret;
	}

	/** Returns true is 'searchFor' is present in buf */
	private static boolean contains(byte[] buf, byte[] searchFor) {
		int searchForLen = searchFor.length;
		int endLoop = buf.length - searchForLen;
		for (int i = 0; i < endLoop; i++) {
				boolean bFound = true;
				for (int j = 0; j < searchForLen; j++) {
					if (buf[i + j] != searchFor[j]) {
							bFound = false;
							break;
					}
				}
				if (bFound)
					return true;
		}
		return false;
	}
}
