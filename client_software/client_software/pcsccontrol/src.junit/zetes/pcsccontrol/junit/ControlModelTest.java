package zetes.pcsccontrol.junit;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import junit.framework.TestCase;
import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlModel;
import be.eid.eidtestinfra.pcsccontrol.Visibility;

public class ControlModelTest extends TestCase {

	public void testVisibility() {
		ControlModel cm = new ControlModel(null);
		
		cm.setVisibility(Visibility.HIDE_VIRTUAL);
		assertEquals(Visibility.HIDE_VIRTUAL, cm.getVisibility());
		
		cm.setVisibility(Visibility.HIDE_REAL);
		assertEquals(Visibility.HIDE_REAL, cm.getVisibility());
		
		cm.setVisibility(Visibility.REAL_FIRST);
		assertEquals(Visibility.REAL_FIRST, cm.getVisibility());

		cm.setVisibility(Visibility.REAL_LAST);
		assertEquals(Visibility.REAL_LAST, cm.getVisibility());
	}
	
	public void testSave() throws IOException {
		ControlModel cm1= new ControlModel(null);
		
		ControlCardHolder cch1 = new ControlCardHolder("1234");
		cch1.setFile("MYFILE");
		
		File tmpF = File.createTempFile(ControlModelTest.class.getName(), null); 
		tmpF.deleteOnExit();
		
		cm1.setFilePath(tmpF.getAbsolutePath());
		List<ControlCardHolder> items1 = Arrays.asList(new ControlCardHolder[]{cch1});
		cm1.replaceItems(items1);
		
		assertTrue(cm1.isDirty());
		
		cm1.save();
		
		ControlModel cm2 = new ControlModel(tmpF.getAbsolutePath());
		cm2.reload();
		List<ControlCardHolder> items2 = cm2.getItems(new String[0]);
		
		ControlCardHolder cch2 = items2.get(0);
		
		assertEquals(cch1.getFile(), cch2.getFile());
		assertEquals(cch1.getHardchipnr(), cch2.getHardchipnr());
	}
	
	public void testReload() throws IOException {
		ControlModel cm1= new ControlModel(null);
		
		ControlCardHolder cch1 = new ControlCardHolder("1234");
		cch1.setFile("MYFILE");
		
		File tmpF = File.createTempFile(ControlModelTest.class.getName(), null); 
		tmpF.deleteOnExit();
		
		cm1.setFilePath(tmpF.getAbsolutePath());
		List<ControlCardHolder> items1 = Arrays.asList(new ControlCardHolder[]{cch1});
		cm1.replaceItems(items1);
		cm1.setVisibility(Visibility.HIDE_REAL);
		
		assertTrue(cm1.isDirty());
		
		cm1.save();

		assertEquals(cm1.getItems(new String[0]).size(), 1);
				
		items1.get(0).setFile(null);		
		cm1.replaceItems(items1);
		cm1.setVisibility(Visibility.HIDE_VIRTUAL);
		
		assertEquals(cm1.getItems(new String[0]).size(), 0);
		
		cm1.reload();
		
		assertEquals(cm1.getItems(new String[0]).size(), 1);
		assertEquals(cm1.getVisibility(), Visibility.HIDE_REAL);
	}


}
