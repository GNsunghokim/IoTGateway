package kr.dms.iot.lifx.desktop;


public class LifxConsoleStarter {
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("Hello World");
		
		LifxNonStaticStarter lifxNonStaticStarter = new LifxNonStaticStarter();
		lifxNonStaticStarter.startTheLifx();
		
		
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		lifxNonStaticStarter.turnItOFF();
		
	}
	
}
